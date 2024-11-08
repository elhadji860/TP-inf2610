#include "merge_sort.h"
#include <pthread.h>

char* texte_entier;

typedef struct {
    int start;
    int end;
    int nbProcessus;
} exec_merg_arg;

void* execute_merge_sort_threads (void*);


int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <array_size> <num_processes>\n", argv[0]);
        exit(1);
    }


    mutex = sem_open(SEM_NAME, O_CREAT, 0644, 1);
    if (mutex == SEM_FAILED) {
        fprintf(stderr, "Erreur de création du sémaphore");
        exit(1);
    }

    int nbProcessus = atoi(argv[1]);
    
    // traitement des attribut de shared_data
    int array_size = atoi(argv[2]);
    if (array_size > MAX_ARRAY_SIZE){
        fprintf(stderr, "nombre d'arguments trop grand");
        exit(1);
    }
    
    shared_data = malloc(sizeof(SharedData)*2);
    shared_data->array = malloc(sizeof(int)*(array_size + 10));

    /* Populate the array to test the sort */
    srand(time(NULL));
    for (int i = 0; i < array_size; i++) {
        shared_data->array[i] = rand() % MAX_NUM_SIZE;
    }

    texte_entier = malloc(sizeof(char)*array_size*10000);

    gettimeofday(&start_time, NULL);
    sprintf(texte_entier, "test");

    write_array_into_file();
    
    pthread_t main_thread;
    exec_merg_arg main_arg;
    main_arg.start = 0;
    main_arg.end = array_size;
    main_arg.nbProcessus = nbProcessus;

    pthread_create(&main_thread, NULL, execute_merge_sort_threads, (void*)&main_arg);

    pthread_join (main_thread, NULL);
    
    gettimeofday(&end_time, NULL);
    long secondes = end_time.tv_sec - start_time.tv_sec;
    long microsecondes = end_time.tv_usec - start_time.tv_usec;

    if (microsecondes < 0) {
        microsecondes += 1000000;
        secondes--;
    }
    FILE *fichier = fopen("sorted_array.txt", "w");

    fprintf(fichier, "%s", texte_entier); 

    fprintf(fichier, "\n \n Time taken: %ld seconds and %ld microseconds\n", sizeof(texte_entier), microsecondes);

    fclose(fichier);

    printf("Time taken: %ld seconds and %ld microseconds\n", secondes, microsecondes);

    free(texte_entier);
    free(shared_data->array);
    free(shared_data);

    sem_unlink(SEM_NAME);
    return 0;
}


void log_array(int start, int end) {
    

    sprintf(texte_entier + strlen(texte_entier) , "start= %d, end = %d, array = [", start, end);
    for (int i = start; i < end - 1; ++i){
        char str[20];
        sprintf(texte_entier + strlen(texte_entier),"%d", shared_data->array[i]);
    }
    
    sprintf(texte_entier + strlen(texte_entier),"%d", shared_data->array[end-1]);
    strcat(texte_entier, "]\n\n");
    
}

void * execute_merge_sort_threads(void * p){

    exec_merg_arg* arguments = (exec_merg_arg*)p;

    int start = arguments->start;
    int end = arguments->end;
    int nbProcessus = arguments->nbProcessus;
    
    if (nbProcessus <= 1 ){
        sem_wait(mutex);
        merge_sort(start, end);
        sem_post(mutex);
        pthread_exit(NULL);
    }

    if (start < end){   

        int mid = start + (end - start)/2;

        pthread_t thread_left;
        pthread_t thread_right;

        exec_merg_arg arg_right;
        arg_right.start = mid + 1;
        arg_right.end = end;
        arg_right.nbProcessus = nbProcessus/2;

        exec_merg_arg arg_left;
        arg_left.start = start;
        arg_left.end = mid;
        arg_left.nbProcessus = nbProcessus/2;

        pthread_create(&thread_left, NULL, execute_merge_sort_threads, (void*) &arg_left);
        pthread_create(&thread_right, NULL, execute_merge_sort_threads, (void*)&arg_right);

        pthread_join(thread_left, NULL);
        pthread_join(thread_right, NULL);

        sem_wait(mutex);
        merge(start, mid ,end);
        log_array(start, end);
        sem_post(mutex);

        }
        pthread_exit(NULL);


}

void merge_sort( int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;
        merge_sort(left, mid);
        merge_sort(mid + 1, right);
        merge(left, mid, right);
        log_array(left, right);
    }
}

void merge(int left, int mid, int right) {
    int i, j, k;
    int n1 = mid - left + 1;
    int n2 = right - mid;

    int L[n1], R[n2];

    for (i = 0; i < n1; i++)
        L[i] = shared_data->array[left + i];
    for (j = 0; j < n2; j++)
        R[j] = shared_data->array[mid + 1 + j];

    i = 0;
    j = 0;
    k = left;

    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) {
            shared_data->array[k] = L[i];
            i++;
        } else {
            shared_data->array[k] = R[j];
            j++;
        }
        k++;
    }

    while (i < n1) {
        shared_data->array[k] = L[i];
        i++;
        k++;
    }

    while (j < n2) {
        shared_data->array[k] = R[j];
        j++;
        k++;
    }
}

void show_array(){
    printf("Sorted array: ");
    for (int i = 0; i < shared_data->size; i++) {
        printf("%d ", shared_data->array[i]);
    }
    printf("\n");
}

void write_array_into_file(){

    sprintf(texte_entier + strlen(texte_entier), "array = [");
    for (int i = 0; i < shared_data->size - 1; ++i){
        sprintf(texte_entier + strlen(texte_entier), "%d, ", shared_data->array[i]);
    }
    
    sprintf(texte_entier + strlen(texte_entier), "%d]", shared_data->array[shared_data->size-1]);
    strcat(texte_entier, "\n\n");

}


