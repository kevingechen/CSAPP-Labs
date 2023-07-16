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
#include <stdbool.h>
#include "cachelab.h"

int is_transpose(int M, int N, int A[N][M], int B[M][N]);

void transpose_32_32_helper(int M, int N, int A[N][M], int B[M][N]) {
    int i, j, tmp;
    int k, tmp_diagnol;
    for (k = 0; k < M / 8; k++) {
        for (i = 0; i < N; i++) {
            for (j = 8*k; j < 8*k + 8; j++) {
                if (i == j) {
                    tmp_diagnol = A[i][j];
                } else {
                    tmp = A[i][j];
                    B[j][i] = tmp;
                }
            }
            if (i >= 8*k && i < 8*k + 8) {
                B[i][i] = tmp_diagnol;
            }
        }    
    }

}

bool is_conflict_64_64(int i, int j) {
    if (i == j) {
        return true;
    }
    int ix = i / 4;
    bool isEvenIx = (ix % 2) == 0;
    if (isEvenIx && i+4 == j) {
        return true;
    }
    if (!isEvenIx && i == j+4) {
        return true;
    }

    return false;
}

void transpose_64_64_helper(int M, int N, int A[N][M], int B[M][N]) {
    int i, j, tmp;
    int bn, bm;
    int tmp_11, tmp_12, tmp_21, tmp_22;
    for (bn = 0; bn < N / 8; bn++) {
        for (bm = 0; bm < M / 8; bm++) {

            for (i = 8*bn; i < 8*bn + 4; i++) {
                for (j = 8*bm; j < 8*bm + 4; j++) {
                    if (is_conflict_64_64(i, j)) continue;
                    tmp = A[i][j];
                    B[j][i] = tmp;
                }
                for (j = 8*bm; j < 8*bm + 4; j++) {
                    if (is_conflict_64_64(i+4, j)) continue;
                    tmp = A[i+4][j];
                    B[j][i+4] = tmp;
                }
            }    

            for (i = 8*bn + 7; i >= 8*bn + 4; i--) {
                for (j = 8*bm + 4; j < 8*bm + 8; j++) {
                    if (is_conflict_64_64(i, j)) continue;
                    tmp = A[i][j];
                    B[j][i] = tmp;
                }
                for (j = 8*bm + 4; j < 8*bm + 8; j++) {
                    if (is_conflict_64_64(i-4, j)) continue;
                    tmp = A[i-4][j];
                    B[j][i-4] = tmp;
                }

            }    

        }
    }


    // handle diagnals and square conflicts
    for (bn = 0; bn < N / 8; bn++) {
        for (tmp = 0; tmp < 4; tmp++) {
            i = 8*bn + tmp;
            j = i;

            tmp_11 = A[i][j];
            tmp_12 = A[i][j+4];
            tmp_21 = A[i+4][j];
            tmp_22 = A[i+4][j+4];

            B[j][i] = tmp_11;
            B[j][i+4] = tmp_21;
            B[j+4][i] = tmp_12;
            B[j+4][i+4] = tmp_22;
        }
    }

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
    if (M == 32) {
        transpose_32_32_helper(M, N, A, B);
    } else if (M == 64) {
        transpose_64_64_helper(M, N, A, B);
    } else {
        for (i = 0; i < N; i++) {
            for (j = 0; j < M; j++) {
                tmp = A[i][j];
                B[j][i] = tmp;
            }
        }    
    }

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

