#include <mpi.h>
#include <iostream>
#include <set>
using namespace std;

#define RESULT_IS_PRIME 1
#define RESULT_NOT_PRIME 2
#define RESULT_TODO -1
#define RESULT_DIE -256

const int root = 0;

constexpr bool result_needs_work(const int result) {
    // If result > 0; work was done and new work should be given
    return result > 0;
}

int main(int argc, char** argv){
    int nmasters, pid;
    MPI_Comm intercom, intercom2;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &nmasters);
    MPI_Comm_rank(MPI_COMM_WORLD, &pid);

    const int first_to_test = atoi(argv[1]);
    const int amount_to_test = atoi(argv[2]);
    const int nslaves = atoi(argv[3]);
    const int last_to_test  = first_to_test + amount_to_test;

    MPI_Comm_spawn("esclave", // exécutable
                     argv, // arguments à passer sur la ligne de commande
                     nslaves, // nombre de processus esclave
                     MPI_INFO_NULL, //permet de préciser où et comment lancer les processus
                     root, // rang du processus maître qui effectue réellement le spawn
                     MPI_COMM_WORLD, // Monde dans lequel est effectué le spwan
                     &intercom, // l'intercommunicateur permettant de communiquer avec les esclaves
                     MPI_ERRCODES_IGNORE // tableau contenant les erreurs
    );


    int* tasks = new int[nslaves];
    int* results = new int[nslaves];
    for (int window_index = 0; window_index < nslaves; window_index++) {
        results[window_index] = RESULT_TODO;
        tasks[window_index] = first_to_test + window_index;
    }

    MPI_Comm intracom;
    MPI_Intercomm_merge(intercom, false, &intracom);

    // int pid_intra;
    // MPI_Comm_rank(intracom ,&pid_intra);
    // cout << "[MASTER]: intra_pid = " << pid_intra << endl;
    // cout << "[MASTER]: results = [";
    // for (int i=0; i<nslaves; i++)
        // cout << results[i] << ", ";
    // cout << "]" << endl;

    MPI_Win win_tasks, win_results;
    MPI_Win_create(tasks, nslaves*sizeof(int), sizeof(int), MPI_INFO_NULL, intracom, &win_tasks);
    MPI_Win_create(results, nslaves*sizeof(int), sizeof(int), MPI_INFO_NULL, intracom, &win_results);


    // used to store the answer
    set<int> primes;

    int window_index = 0;
    int next_to_test = first_to_test + nslaves;
    // `+ nslaves - 1` to give us time to tell every slave to stop
    // We could do it after the loop, but we risk a slave overrides its order
    // to stop, so we would need to use a flag system for `results`.
    while (next_to_test < last_to_test + nslaves - 1) {
        MPI_Win_lock(MPI_LOCK_EXCLUSIVE, 0, 0, win_results);
        if (result_needs_work(results[window_index])) {
            // Process result
            if (results[window_index] == RESULT_IS_PRIME) {
                cout << tasks[window_index] << " is prime" << endl;
                primes.insert(tasks[window_index]);
            }
            // Give new task, or signal to stop
            tasks[window_index] = next_to_test++;
            results[window_index] =
                (next_to_test < last_to_test) ?
                RESULT_TODO : RESULT_DIE;
        }
        MPI_Win_unlock(0, win_results);

        // Looping the window_index
        if (++window_index >= nslaves) window_index = 0;
        // cout << "[MASTER]: window_index = " << window_index << endl;
    }

    cout << "Primes : {";
    for (const int& prime : primes) cout << prime << ", ";
    cout << "}" << endl;

    MPI_Win_free(&win_tasks);
    MPI_Win_free(&win_results);
    MPI_Comm_free(&intracom);
    MPI_Comm_free(&intercom);
    MPI_Finalize();
    return 0;
}
