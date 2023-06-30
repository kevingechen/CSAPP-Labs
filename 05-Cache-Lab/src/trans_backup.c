/* 
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */ 
#include <stdio.h>
#include "cachelab.h"

int is_transpose(int M, int N, int A[N][M], int B[M][N]);

unsigned long calculateSetIndex(unsigned int s, unsigned int b, unsigned long addr_val) {
    return ((addr_val) >> b) & ((1ul << s) - 1);
}

unsigned long calculateTag(unsigned int s, unsigned int b, unsigned long addr_val) {
    return addr_val >> (s + b);
}

/* 
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded. 
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, tmp;
    char file_name[30];
    unsigned long addr_A, addr_B, set_A, set_B, tag_A, tag_B;
    sprintf(file_name, "addr_conflicts_%d_%d.txt", M, N);
    FILE *out_file = fopen(file_name, "w");
    // Check if the file was successfully opened
    if (out_file == NULL) {
        printf("Failed to open the file.\n");
        return;
    }

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            tmp = A[i][j];
            B[j][i] = tmp;
            addr_A = (unsigned long) &A[i][j];
            addr_B = (unsigned long) &B[j][i];
            set_A = calculateSetIndex(5, 5, addr_A);
            tag_A = calculateTag(5, 5, addr_A);
            set_B = calculateSetIndex(5, 5, addr_B);
            tag_B = calculateTag(5, 5, addr_B);
            fprintf(out_file, "info between A[%d][%d] and B[%d][%d], set_A=%lu, tag_A=%lu, set_B=%lu, tag_B=%lu\n",
                    i,j,
                    j,i,
                    set_A, set_B,
                    tag_A, tag_B);
            
        }
    }    

    fclose(out_file);

}

/* 
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started. 
 */ 

/* 
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, tmp;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }    

}

/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions()
{
    /* Register your solution function */
    registerTransFunction(transpose_submit, transpose_submit_desc); 

    /* Register any additional transpose functions */
    registerTransFunction(trans, trans_desc); 

}

/* 
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N])
{
    int i, j;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; ++j) {
            if (A[i][j] != B[j][i]) {
                return 0;
            }
        }
    }
    return 1;
}

