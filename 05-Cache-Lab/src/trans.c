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

void transpose_64_64_helper(int M, int N, int A[N][M], int B[M][N]) {
    int i, j, tmp;
    int bn, bm;
    int tmp_up, tmp_down;
    int t4, t5, t6, t7;
    for (bn = 0; bn < N / 8; bn++) {
        for (bm = 0; bm < M / 8; bm++) {

            for (i = 8*bn; i < 8*bn + 4; i++) {
                for (j = 8*bm; j < 8*bm + 4; j++) {
                    if (i == j) {
                        tmp_up = A[i][j];
                        continue;
                    }
                    tmp = A[i][j];
                    B[j][i] = tmp;
                }
                if (i == 8*bn) {
                    t4 = A[i][8*bm+4];
                    t5 = A[i][8*bm+5];
                    t6 = A[i][8*bm+6];
                    t7 = A[i][8*bm+7];
                }
                for (j = 8*bm; j < 8*bm + 4; j++) {
                    if (i == j) {
                        tmp_down = A[i+4][j];
                        continue;
                    }
                    tmp = A[i+4][j];
                    B[j][i+4] = tmp;
                }
                if (bn == bm) {
                    B[i][i] = tmp_up;
                    B[i][i+4] = tmp_down;
                }
            }    

            for (i = 8*bn + 4; i < 8*bn + 8; i++) {
                for (j = 8*bm + 4; j < 8*bm + 8; j++) {
                    if (i == j) {
                        tmp_down = A[i][j];
                        continue;
                    }

                    tmp = A[i][j];
                    B[j][i] = tmp;
                }
                if (i-4 == 8*bn) {
                    B[8*bm+4][i-4] = t4;
                    B[8*bm+5][i-4] = t5;
                    B[8*bm+6][i-4] = t6;
                    B[8*bm+7][i-4] = t7;
                } else {
                    for (j = 8*bm + 4; j < 8*bm + 8; j++) {
                        if (i == j) {
                            tmp_up = A[i-4][j];
                            continue;
                        }

                        tmp = A[i-4][j];
                        B[j][i-4] = tmp;
                    }
                }

                if (bn == bm) {
                    B[i][i] = tmp_down;
                    if (i-4 != 8*bn) {
                        B[i][i-4] = tmp_up;
                    }
                }
            }    

        }
    }

}

void transpose_61_67_helper(int M, int N, int A[N][M], int B[M][N]) {
    int i, j, tmp;
    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            tmp = A[i][j];
            B[j][i] = tmp;
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
    } else if (N == 67 && M == 61) {
        transpose_61_67_helper(M, N, A, B);
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

