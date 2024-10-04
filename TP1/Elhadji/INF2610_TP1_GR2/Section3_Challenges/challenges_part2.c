/*
 * Ecole Polytechnique Montreal - GIGL
 * Automne 2024
 * Challenges - part2.c
 *
 * Ajoutez vos noms, prénoms et matricules
*/
#include "challenges_part2.h"

const int INTERVALLE_X = 500;
const int INTERVALLE_Y = 500;

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
    args.C->matrix[args.rowPosition][args.colPosition] = 0;
    for(int i = 0; i < args.B->rows; ++i){
         args.C->matrix[args.rowPosition][args.colPosition] += args.A->matrix[args.rowPosition][i] * args.B->matrix[i][args.colPosition];
    }

    

    pthread_exit(NULL);

}

void* calculSeparation(void* arguments){


    CalculsSeparationArgs args = *(CalculsSeparationArgs*)arguments ;
    short longueur = args.limites.maxRows - args.limites.minRows;
    short largeur = args.limites.maxCols - args.limites.minCols;

    pthread_t* tabTid = (pthread_t*)malloc(sizeof(pthread_t)*longueur* largeur);
    ArgsThread* tabArgs = (ArgsThread*) malloc(sizeof(ArgsThread) *longueur * largeur);
    
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

        free(tabTid);
        free(tabArgs);
    
    

    pthread_exit(NULL);

    
}

void* largeMatrixMultply(void* arguments){

    CalculsSeparationArgs args = *(CalculsSeparationArgs*)arguments ;
    short longueur = args.limites.maxRows - args.limites.minRows;
    short largeur = args.limites.maxCols - args.limites.minCols;

    ArgsThread tabArgs[longueur * largeur];

    
    for (int i = 0; i < longueur; ++i){
        
            for(int j = 0; j < largeur; ++j){
                 args.matrixes.C->matrix[i + args.limites.minRows][j + args.limites.minCols] = 0;
                for(int k = 0; k < args.matrixes.B->rows; ++k){
                       args.matrixes.C->matrix[i + args.limites.minRows][j + args.limites.minCols] += args.matrixes.A->matrix[i + args.limites.minRows][k] * args.matrixes.B->matrix[k][j + args.limites.minCols];
                 }

                
            }
            
        }
    pthread_exit(NULL);
}

void* gestionLargeMatrix (void* arguments){

    MatrixGroup matrixes = *(MatrixGroup*) arguments;
    int limIteration_X = matrixes.C->cols/INTERVALLE_X;
    int limIteration_Y = matrixes.C->rows/INTERVALLE_Y;

    pthread_t* tabTid = (pthread_t*)malloc(sizeof(pthread_t)*limIteration_X * limIteration_Y);
    CalculsSeparationArgs* tabArgs = (CalculsSeparationArgs*) malloc(sizeof(CalculsSeparationArgs) * limIteration_X * limIteration_Y);

   for (int i = 0; i < limIteration_Y; ++i){
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
    free(tabTid);
    free(tabArgs);

    pthread_exit(NULL);

}

Matrix* multiply(Matrix* A, Matrix* B) {
    
    // TODO
    if (A->cols != B->rows || A->cols == 0 || B->cols == 0 || A->rows == 0 || B->rows == 0 ){
        return NULL;
    }

    Matrix* C = matrixGenerator(A->rows, B->cols);

    if (C->cols * C->rows > 60*60){
        const int num_matrices = 5;
        Matrix* matrices[num_matrices];
        MatrixGroup args[num_matrices];
        pthread_t tabTid[num_matrices];


        for (int j = 0; j < num_matrices; ++j) {
            matrices[j] = matrixGenerator(500, 2500);
            for (int i = 0; i < 500; ++i) {
                matrices[j]->matrix[i] = C->matrix[500 * j + i];
            }
        args[j] = (MatrixGroup){ A, B, matrices[j] }; 
        }

    for (int j = 0; j < num_matrices; ++j) {
        if (pthread_create(&tabTid[j], NULL, gestionLargeMatrix, (void*)&args[j]) != 0) {
        }
    }

        for(int i = 0; i < num_matrices; ++i){
            pthread_join(tabTid[i], NULL);
        }
        
        return (void *)C;
        
    }

    
    MatrixGroup matrixes = { A, B, C};
    Limites limites = {0, C->rows, 0, C->cols};
    CalculsSeparationArgs calculsSeparationArgs = {matrixes, limites};

    pthread_t tid;
    pthread_create(&tid, NULL, calculSeparation, &calculsSeparationArgs);
    pthread_join(tid, NULL);


    return (void*)C;
}


// This main is to help you in your development. 
// Don't forget to comment it out when you run the tests.
//  int main(int argc, char*argv[])
// {    
//      return 0;
// }
