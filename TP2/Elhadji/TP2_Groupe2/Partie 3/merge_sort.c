#include "merge_sort.h"



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
    char* shm_name = "/shared_data";
    int shm_size = sizeof(SharedData);

    int shm_fd = shm_open(shm_name, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        fprintf(stderr, "erreur shm_open");
        exit(1);
    }

    if (ftruncate(shm_fd, shm_size) == -1) {
        fprintf(stderr, "erreur ftruncate");
       exit(1);
    }
    
    shared_data = mmap(NULL, shm_size,PROT_READ | PROT_WRITE, MAP_SHARED , shm_fd, 0);
    if (shared_data == MAP_FAILED){
        fprintf(stderr, "erreur mmap");
        exit(1);
    }
    
    shared_data->size = array_size;
    
    char* shm_array_name = "/shared_data_array";
    int shm_array_size = sizeof(int) * array_size;

    int shm_array_fd = shm_open(shm_array_name, O_CREAT | O_RDWR, 0666);
    if (shm_array_fd == -1) {
        fprintf(stderr, "erreur shm_open");
        exit(1);
    }

    if (ftruncate(shm_array_fd, shm_array_size) == -1) {
         fprintf(stderr, "erreur ftruncate");
        exit(1);
    }
    
    shared_data->array = mmap(NULL, shm_array_size,PROT_READ | PROT_WRITE, MAP_SHARED , shm_array_fd, 0);
    if (shared_data == MAP_FAILED){
         fprintf(stderr, "erreur mmap");
        exit(1);
    }

    /* Populate the array to test the sort */
    srand(time(NULL));
    for (int i = 0; i < array_size; i++) {
        shared_data->array[i] = rand() % MAX_NUM_SIZE;
    }

    int fd_std_out = dup(STDOUT_FILENO);
    write_array_into_file();
    show_array();
    fflush(stdout);
    
    gettimeofday(&start_time, NULL);

    execute_merge_sort(0, shared_data->size - 1, nbProcessus);
    
    gettimeofday(&end_time, NULL);
    dup2(fd_std_out, STDOUT_FILENO); 
    
    long secondes = end_time.tv_sec - start_time.tv_sec;
    long microsecondes = end_time.tv_usec - start_time.tv_usec;

    if (microsecondes < 0) {
        microsecondes += 1000000;
        secondes--;
    }

    printf("Time taken: %ld seconds and %ld microseconds\n", secondes, microsecondes);

    munmap(shared_data->array, sizeof(int)*shared_data->size);
    munmap(shared_data, sizeof(shared_data));
    shm_unlink(shm_array_name);
    shm_unlink(shm_name);
    sem_unlink(SEM_NAME);
    close(fd);
    close(fd_std_out);


    return 0;
}

void log_array(int start, int end) {
    
    char array_elements[100] = "[";
    for (int i = start; i < end - 1; ++i){
        char num_str[30];  
        sprintf(num_str, "%d", shared_data->array[i]);
        strcat(array_elements, num_str);
        strcat(array_elements, " ,");
    }
    char num_str[30];
    sprintf(num_str, "%d", shared_data->array[end - 1]);
    strcat(array_elements, num_str);
    strcat(array_elements, "] \n");

    printf("start= %d, end = %d, array = %s", start, end, array_elements);
}

void execute_merge_sort(int start, int end, int nbProcessus){


    
    if (nbProcessus <= 1 ){
        merge_sort(start, end);
        log_array(start, end);
        fflush(stdout);
        return;
    }

    if (start < end){   

        int mid = start + (end - start)/2;

        if(fork() == 0){
            execute_merge_sort(start, mid, nbProcessus/2);
            munmap(shared_data->array, sizeof(int)*shared_data->size);
            munmap(shared_data, sizeof(shared_data));
            exit(0);
        }
        if(fork() == 0){
            execute_merge_sort(mid + 1, end, nbProcessus/2);
            munmap(shared_data->array, sizeof(int)*shared_data->size);
            munmap(shared_data, sizeof(shared_data));
            exit(0);
        }

        while(wait(NULL) > 0);

        sem_wait(mutex);
        merge(start, mid ,end);
        sem_post(mutex);

        log_array(start, end);
        }


}

void merge_sort( int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;
        merge_sort(left, mid);
        merge_sort(mid + 1, right);
        merge(left, mid, right);
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
    int fd = open("sorted_arrays.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    dup2(fd, STDOUT_FILENO);
}

