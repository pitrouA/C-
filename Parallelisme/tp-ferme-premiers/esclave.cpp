#include <mpi.h>
#include <iostream>
#include <cmath>
using namespace std;

#define RESULT_IS_PRIME 1
#define RESULT_NOT_PRIME 2
#define RESULT_TODO -1
#define RESULT_DIE -256

// we need const aswell as macros to have pointers for communications
const int result_is_prime = RESULT_IS_PRIME;
const int result_not_prime = RESULT_NOT_PRIME;

constexpr bool is_prime(const int x) {
    for (int divisor = 2; divisor <= sqrt(x); divisor++) {
        if ((x % divisor) == 0) {
            // We found a divisor of x, so it's not prime
            return false;
        }
    }
    return true;
}

int main(int argc, char** argv){
    int nslaves, pid, nmasters,flag;
    MPI_Comm intercom;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &nslaves);
    MPI_Comm_rank(MPI_COMM_WORLD, &pid);
    MPI_Comm_get_parent(&intercom); // obtention de l'intercommunicateur vers le/les parents
    MPI_Comm_remote_size(intercom, &nmasters); // permet de connaître le nombre de parents
    // code de l'esclave

    MPI_Comm intracom;
    // true to have processes ordered after
    MPI_Intercomm_merge(intercom, true, &intracom);

    // int pid_intra;
    // MPI_Comm_rank(intracom ,&pid_intra);
    // cout << "[SLAVE "<< pid << "]: intra_pid = " << intra_pid << endl;

    // creating windows with master
    MPI_Win win_tasks, win_results;
    MPI_Win_create(nullptr, nslaves*sizeof(int), sizeof(int), MPI_INFO_NULL, intracom, &win_tasks);
    MPI_Win_create(nullptr, nslaves*sizeof(int), sizeof(int), MPI_INFO_NULL, intracom, &win_results);
    // cout << "[SLAVE "<< pid << "]: Window created." << endl;

    int result, task;
    bool prime;
    do {
        // Updating state
        MPI_Win_lock(MPI_LOCK_EXCLUSIVE, 0, 0, win_results);
        // cout << "[SLAVE "<< pid << "]: Window locked. Attempting Get..." << endl;
        MPI_Get(&result, 1, MPI_INT, 0, pid, 1, MPI_INT, win_results);
        // cout << "[SLAVE "<< pid << "]: Get successful." << endl;
        // cout << "[SLAVE "<< pid << "]: result = " << result << endl;
        MPI_Win_unlock(0, win_results);
        // cout << "[SLAVE "<< pid << "]: Window unlocked." << endl;


        if (result == RESULT_TODO) {
            // We have work to do
            MPI_Win_lock(MPI_LOCK_EXCLUSIVE, 0, 0, win_tasks);
            MPI_Get(&task,
                1, MPI_INT, 0, pid, 1, MPI_INT, win_tasks);
            MPI_Win_unlock(0, win_tasks);

            // cout << "[SLAVE "<< pid << "]: task = " << task << endl;
            prime = is_prime(task); // Do the work

            // Put result in master's window
            MPI_Win_lock(MPI_LOCK_EXCLUSIVE, 0, 0, win_results);
            MPI_Put(&(prime?result_is_prime:result_not_prime),
                1, MPI_INT, 0, pid, 1, MPI_INT, win_results);
            MPI_Win_unlock(0, win_results);
        }
        // Loop if not told not to do so
    } while (result != RESULT_DIE);

    MPI_Win_free(&win_tasks);
    MPI_Win_free(&win_results);
    MPI_Comm_free(&intracom);
    MPI_Comm_free(&intercom);
    MPI_Finalize();
    return 0;
}
