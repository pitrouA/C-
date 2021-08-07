//
// Created by sophie on 18/02/2020.
//
#include <iostream>
#include "syracuse.h"

// Fonction pour tester si un tableau représente ou pas une suite de syracuse
int test_syracuse(int n, int *tab) {
    int test = 1;
    for (int i = 1; i < n; i++)
        if (tab[i - 1] % 2 == 0) {
            if (tab[i] != (tab[i - 1] / 2)) {
                test = 0;
                break;
            }
        } else {
            if (tab[i] != 3 * tab[i - 1] + 1) {
                test = 0;
                break;
            }
        }
    return test;
}

// Génération du suite de Syracuse dans tab
void syracuse(int n, int *tab) {
    srand(time(NULL));
    tab[0] = rand() % 50;
    for (int i = 1; i < n; i++) {
        if (tab[i - 1] % 2 == 0)
            tab[i] = tab[i - 1] / 2;
        else
            tab[i] = 3 * tab[i - 1] + 1;
    }
}

// Génération d'un tableau aléatoire
void non_syracuse(int n, int *tab) {
    srand(time(NULL));
    for (int i = 0; i < n; i++)
        tab[i] = rand() % 50;
}


// Génération d'un tableau dont la 1ère moitié est de Syracuse et la 2eme aléatoire.
void demi_syracuse(int n, int *tab) {
    int mi = n / 2;
    syracuse(mi, tab);
    non_syracuse(n - mi, tab + mi);
}