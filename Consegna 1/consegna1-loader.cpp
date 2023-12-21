#include <fstream>
#include <iostream>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
using namespace std;

// compilazione: g++ consegna1-loader.c
//

// Il programma carica il file data.txt contenente 100 righe con dati da ordinare in modo crescente
// ./a.out
// In output viene mostrato il numero di accessi in read alla memoria per eseguire il sorting di ciascuna riga

// Obiettivo:
// Creare un algoritmo di sorting che minimizzi la somma del numero di accessi per ogni sorting di ciascuna riga del file

int ct_swap = 0;
int ct_cmp = 0;
int ct_read = 0;

int max_dim = 0;
int ntests = 100;
int ndiv = 1;
int details = 0;
int graph = 0;

int n = 0; /// dimensione dell'array

void print_array(int *A, int dim) {
    for (int j = 0; j < dim; j++) {
        printf("%d ", A[j]);
    }
    printf("\n");
}

void swap(int &a, int &b) {
    int tmp = a;
    a = b;
    b = tmp;
    /// aggiorno contatore globale di swap
    ct_swap++;
}

int partition(int *A, int p, int r) {

    /// copia valori delle due meta p..q e q+1..r
    ct_read++;
    int x = A[r];
    int i = p - 1;

    for (int j = p; j < r; j++) {
        ct_cmp++;
        ct_read++;
        if (A[j] <= x) {
            i++;
            ct_read++;
            ct_read++;
            swap(A[i], A[j]);
        }
    }
    ct_read++;
    ct_read++;
    swap(A[i + 1], A[r]);

    return i + 1;
}

void quick_sort(int *A, int p, int r) {
    /// gli array L e R sono utilizzati come appoggio per copiare i valori: evita le allocazioni nella fase di merge
    if (p < r) {
        int q = partition(A, p, r);
        quick_sort(A, p, q - 1);
        quick_sort(A, q + 1, r);
    }
}



int parse_cmd(int argc, char **argv) {

    /// parsing argomento
    max_dim = 1000;

    for (int i = 1; i < argc; i++) {
        if (argv[i][1] == 'd')
            ndiv = atoi(argv[i] + 3);
        if (argv[i][1] == 't')
            ntests = atoi(argv[i] + 3);
        if (argv[i][1] == 'v')
            details = 1;
        if (argv[i][1] == 'g') {
            graph = 1;
            ndiv = 1;
            ntests = 1;
        }
    }

    return 0;
}

/**
 * Unisce due sottoarray dell'array dato.
 * 
 * @param arr L'array da unire.
 * @param start L'indice di inizio del primo sottoarray.
 * @param mid L'indice di fine del primo sottoarray.
 * @param end L'indice di fine del secondo sottoarray.
 */
void merge(int *arr, int start, int mid, int end) {
    int leftSize = mid - start + 1; // Dimensione del sottoarray sinistro
    int rightSize = end - mid; // Dimensione del sottoarray destro

    int *leftArray = new int[leftSize]; // Array temporaneo per il sottoarray sinistro
    int *rightArray = new int[rightSize]; // Array temporaneo per il sottoarray destro

    // Copia gli elementi dall'array originale al sottoarray sinistro
    for (int i = 0; i < leftSize; i++) {
        leftArray[i] = arr[start + i];
        ct_read++; // Incrementa il contatore degli accessi in lettura per ogni operazione di lettura
    }

    // Copia gli elementi dall'array originale al sottoarray destro
    for (int j = 0; j < rightSize; j++) {
        rightArray[j] = arr[mid + 1 + j];
        ct_read++; // Incrementa il contatore degli accessi in lettura per ogni operazione di lettura
    }

    int i = 0; // Indice per il sottoarray sinistro
    int j = 0; // Indice per il sottoarray destro
    int k = start; // Indice per l'array unito

    // Unisce i sottoarray sinistro e destro nell'array originale
    while (i < leftSize && j < rightSize) {
        if (leftArray[i] <= rightArray[j]) {
            swap(arr[k], leftArray[i]);
            i++;
        } else {
            swap(arr[k], rightArray[j]);
            j++;
        }
        k++;
        ct_swap++; // Incrementa il contatore degli accessi in memoria per ogni operazione di scambio
    }

    // Copia gli eventuali elementi rimanenti dal sottoarray sinistro all'array originale
    while (i < leftSize) {
        swap(arr[k], leftArray[i]);
        i++;
        k++;
    }

    // Copia gli eventuali elementi rimanenti dal sottoarray destro all'array originale
    while (j < rightSize) {
        swap(arr[k], rightArray[j]);
        j++;
        k++;
    }

    delete[] leftArray; // Libera la memoria allocata per il sottoarray sinistro
    delete[] rightArray; // Libera la memoria allocata per il sottoarray destro
}

void merge_sort(int *A, int p, int r) {
    if (p < r) {
        int q = (p + r) / 2;
        merge_sort(A, p, q);
        merge_sort(A, q + 1, r);
        merge(A, p, q, r);
    }
}

void block_sort(int *A, int p, int r) {
    if (p < r) {
        int q = partition(A, p, r);
        block_sort(A, p, q - 1);
        block_sort(A, q + 1, r);
    }
}


int main(int argc, char **argv) {
    int i, test;
    int *A;
    int *B; /// buffer per visualizzazione algoritmo

    if (parse_cmd(argc, argv))
        return 1;

    /// allocazione array
    A = new int[max_dim];

    n = max_dim;

    ifstream input_data;
    input_data.open("data.txt");

    int read_min = -1;
    int read_max = -1;
    long read_avg = 0;

    //// lancio ntests volte per coprire diversi casi di input random
    for (test = 0; test < ntests; test++) {

        /// inizializzazione array: numeri random con range dimensione array
        for (i = 0; i < n; i++) {
            char comma;
            input_data >> A[i];
            input_data >> comma;
        }

        if (details) {
            printf("caricato array di dimensione %d\n", n);
            print_array(A, n);
        }

        ct_swap = 0;
        ct_cmp = 0;
        ct_read = 0;

        /// algoritmo di sorting
        merge_sort(A, 0, n - 1);
        if (details) {
            printf("Output:\n");
            print_array(A, n);
        }

        /// statistiche
        read_avg += ct_read;
        if (read_min < 0 || read_min > ct_read)
            read_min = ct_read;
        if (read_max < 0 || read_max < ct_read)
            read_max = ct_read;
        printf("Test %d %d\n", test, ct_read);
    }

    
    printf("Numero dei test: %d\n", ntests);
    printf("Letture minime eseguite: %d\n", read_min);
    printf("Media delle letture eseguite: %.1f\n", (0.0 + read_avg) / ntests);
    printf("Letture massime eseguite: %d\n", read_max);

    delete[] A;

    return 0;
}
