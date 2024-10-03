/*
 * Ecole Polytechnique Montreal - GIGL
 * Automne 2024
 * Challenges - part2.c
 *
 * Ajoutez vos noms, prénoms et matricules
*/
#include "challenges_part2.h"

const int INTERVALLE_X = 100;
const int INTERVALLE_Y = 10;

void passed(){
    printf("passed \n");
}

void printMatrix(Matrix* matrix) {
    for (int i = 0; i < matrix->rows; i++) {
        for (int j = 0; j < matrix->cols; j++) {
            printf("%d ", matrix->matrix[i][j]);
        }
        printf("\n");
    }
}

// void freeMatrix(Matrix* mat) {
//     for (int i = 0; i < mat->rows; i++) {
//         free(mat->matrix[i]);
//     }
//     free(mat->matrix);
// }

typedef struct {
    Matrix* A;
    Matrix* B;
    Matrix* C;
    int rowPosition;
    int colPosition;
} ArgsThread;

typedef struct{
    Matrix* A;
    Matrix* B;
    Matrix* C;
} MatrixGroup;

typedef struct{
    short minRows;
    short maxRows;
    short minCols;
    short maxCols;
}   Limites;

typedef struct{
    MatrixGroup matrixes;
    Limites limites;
} CalculsSeparationArgs;


Matrix* matrixGenerator(short rows, short cols){

    Matrix* C = (Matrix*)malloc (sizeof(Matrix));
    C->cols = cols;
    C->rows = rows;

    C->matrix = (short**) malloc(sizeof(short*) * C->rows );

    for(int i = 0; i < C->rows; ++i){
        C->matrix[i] = (short*) malloc(sizeof(short) * C->cols);
    } 
    return C;
}



void* LineRowProduct(void* argStructure){

    ArgsThread args = *(ArgsThread*) argStructure; 

    short produit = 0;
    for(int i = 0; i < args.B->rows; ++i){
        produit += args.A->matrix[args.rowPosition][i] * args.B->matrix[i][args.colPosition];
    }

    args.C->matrix[args.rowPosition][args.colPosition] = produit;
    

    pthread_exit((void *)&produit);

}

void* calculSeparation(void* arguments){


    CalculsSeparationArgs args = *(CalculsSeparationArgs*)arguments ;
    short longueur = args.limites.maxRows - args.limites.minRows;
    short largeur = args.limites.maxCols - args.limites.minCols;

    pthread_t tabTid[longueur* largeur];
    ArgsThread tabArgs[longueur * largeur];
    
    for (int i = 0; i < longueur; ++i){
        
            for(int j = 0; j < largeur; ++j){
                ArgsThread argsThread = { args.matrixes.A, args.matrixes.B, args.matrixes.C, i + args.limites.minRows, j + args.limites.minCols};
                tabArgs[i*largeur +j] = argsThread;
                
                pthread_create(&tabTid[i*(largeur) + j], NULL, LineRowProduct, (void *)&tabArgs[i*largeur + j]);
                
            }
            
        }
        for(int i = 0; i < longueur; ++i){
            for(int j = 0; j < largeur; ++j){
                    pthread_join(tabTid[i*largeur + j], NULL);
            }
        }
    
    

    pthread_exit(NULL);

    
}

void* largeMatrixMultply(void* arguments){

    CalculsSeparationArgs args = *(CalculsSeparationArgs*)arguments ;
    short longueur = args.limites.maxRows - args.limites.minRows;
    short largeur = args.limites.maxCols - args.limites.minCols;

    ArgsThread tabArgs[longueur * largeur];

    
    for (int i = 0; i < longueur; ++i){
        
            for(int j = 0; j < largeur; ++j){
                ArgsThread argsThread = { args.matrixes.A, args.matrixes.B, args.matrixes.C, i + args.limites.minRows, j + args.limites.minCols};
                //tabArgs[i*largeur +j] = argsThread;
                //passed();
                short produit = 0;
                 for(int k = 0; k < args.matrixes.B->rows; ++k){
                      produit += args.matrixes.A->matrix[i + args.limites.minRows][k] * args.matrixes.B->matrix[k][j + args.limites.minCols];
                }

                args.matrixes.C->matrix[i + args.limites.minRows][j + args.limites.minCols] = produit;
                //args.matrixes.C->matrix[i + args.limites.minRows][j+args.limites.minCols] = 1;
                //printf("%d", args.matrixes.C->matrix[i + args.limites.minRows][j+args.limites.minCols]);
                
            }
            
        }
    pthread_exit(NULL);
}

void gestionLargeMatrix (MatrixGroup matrixes){

    int limIteration_X = matrixes.C->cols/INTERVALLE_X;
    int limIteration_Y = matrixes.C->rows/INTERVALLE_Y;

    pthread_t tabTid[limIteration_X * limIteration_Y];
    CalculsSeparationArgs tabArgs[limIteration_X * limIteration_Y];

    printf("%d \n", limIteration_X);

   for (int i = 0; i < limIteration_Y; ++i){
        printf("%d \n", i);
        for(int j = 0; j < limIteration_X; ++j){
            
            MatrixGroup matrices = { matrixes.A, matrixes.B, matrixes.C};
            Limites limites = {i*INTERVALLE_Y, i*INTERVALLE_Y + INTERVALLE_Y  , j*INTERVALLE_X, j*INTERVALLE_X + INTERVALLE_X};
            CalculsSeparationArgs calculsSeparationArgs = {matrices, limites};

            tabArgs[i*limIteration_X + j] = calculsSeparationArgs;
            pthread_create(&tabTid[i*limIteration_X + j], NULL, largeMatrixMultply, (void *)&tabArgs[i*limIteration_X + j]);
            
        }

        
    }
    
    for(int i = 0; i < limIteration_Y; ++i)
        for(int k = 0; k < limIteration_X; ++k){
            pthread_join(tabTid[i*limIteration_X + k], NULL);
        }



}

Matrix* multiply(Matrix* A, Matrix* B) {
    
    // TODO
    if (A->cols != B->rows || A->cols == 0 || B->cols == 0 || A->rows == 0 || B->rows == 0 ){
        return NULL;
    }

    printf("A: %d\n", A->rows);
    printf("B: %d\n", B->cols);
    printf("\n");
    //printMatrix(B);

    Matrix* C = matrixGenerator(A->rows, B->cols);

    if (C->cols * C->rows > 60*60){
        MatrixGroup matrixes = { A, B, C};
        gestionLargeMatrix(matrixes);
        //printMatrix(C);
        return (void *)C;
        
    }


    // printf("C: %d\n", C->cols);
    // printf("C: %d\n", C->rows);
    // printMatrix(C);
    // printf("passed \n");
    
    MatrixGroup matrixes = { A, B, C};
    Limites limites = {0, C->rows, 0, C->cols};
    CalculsSeparationArgs calculsSeparationArgs = {matrixes, limites};

    pthread_t tid;
    pthread_create(&tid, NULL, calculSeparation, &calculsSeparationArgs);
    pthread_join(tid, NULL);


    //printMatrix(C);

    return (void*)C;
}


// This main is to help you in your development. 
// Don't forget to comment it out when you run the tests.
//  int main(int argc, char*argv[])
// {
//     Matrix* A = matrixGenerator(60,4);
//     Matrix* B = matrixGenerator(60,60);
//     fillMatrixWithRandomValues(A,0, 100);
//     fillMatrixWithRandomValues(B, 0, 100);
//     printMatrix(B);
//     printf("\n");
//     printMatrix(A);
//     printf("\n");
//     Matrix* C = (Matrix*) multiply(B, A);
//     printMatrix(C);
//     freeMatrix(C);
//     freeMatrix(A);
//     freeMatrix(B);
    
//      return 0;
// }
