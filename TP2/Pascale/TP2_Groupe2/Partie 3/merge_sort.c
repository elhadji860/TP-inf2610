#include "merge_sort.h"

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <array_size> <num_processes>\n", argv[0]);
        exit(1);
    }

    int num_processes = atoi(argv[1]);
    int array_size = atoi(argv[2]);

    if (array_size > MAX_ARRAY_SIZE) {
        fprintf(stderr, "Erreur: la taille du tableau dépasse la limite\n");
        exit(1);
    }

    fd = shm_open("/shm_merge_sort", O_CREAT | O_RDWR, 0666);
    if (fd < 0) {
        perror("Erreur d'ouverture de la mémoire partagé");
        exit(1);
    }
    ftruncate(fd, sizeof(SharedData) + array_size * sizeof(int));
    shared_data = mmap(NULL, sizeof(SharedData) + array_size * sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (shared_data == MAP_FAILED) {
        perror("Erreur de mappage de la mémoire partagé");
        exit(1);
    }

    shared_data->array = (int *)(shared_data + 1);
    shared_data->size = array_size;

    sem_unlink(SEM_NAME);
    mutex = sem_open(SEM_NAME, O_CREAT, 0644, 1);
    if (mutex == SEM_FAILED) {
        perror("Erreur de création du sémaphore");
        exit(1);
    }
    
    /* Populate the array to test the sort */
    srand(time(NULL));
    for (int i = 0; i < array_size; i++) {
        shared_data->array[i] = rand() % MAX_NUM_SIZE;
    }

    FILE *file = fopen("sorted_array.txt", "a");
    if (file == NULL) {
        perror("Erreur d'ouverture du fichier de logging");
        exit(1);
    }
    fprintf(file, "Nombre de processus: %d\n", num_processes);
    fclose(file);

    execute_merge_sort(0, array_size - 1, num_processes);

    show_array();

    sem_close(mutex);
    sem_unlink(SEM_NAME);
    munmap(shared_data, sizeof(SharedData) + array_size * sizeof(int));
    close(fd);
    shm_unlink("/shm_merge_sort");

    return 0;
}

void log_sort(int start, int end, int *array, int size, long execution_time) {
    FILE *file = fopen("sorted_array.txt", "a");
    if (file == NULL) {
        perror("Erreur d'ouverture du fichier de logging");
        exit(1);
    }

    fprintf(file, "array = [");
    for (int i = 0; i < size; i++) {
        fprintf(file, "%d", array[i]);
        if (i < size - 1) fprintf(file, ",");
    }
    fprintf(file, "]\n");

    fprintf(file, "Start = %d, End = %d, Sorted = [", start, end);
    for (int i = start; i <= end; i++) {
        fprintf(file, "%d", array[i]);
        if (i < end) fprintf(file, ",");
    }
    fprintf(file, "], Execution time = %ld microseconds \n\n", execution_time);

    fclose(file);
}

void execute_merge_sort(int start, int end, int num_processes) {
    if (num_processes <= 1) {
        merge_sort(start, end);
        return;
    }

    int mid = start + (end - start)/2;

    pid_t pid_left = fork();
    if (pid_left == 0) {
        execute_merge_sort(start, mid, num_processes / 2);
        exit(0);
    }

    pid_t pid_right = fork();
    if (pid_right == 0) {
        execute_merge_sort(mid+1, end, num_processes / 2);
        exit(0);
    }

    waitpid(pid_left, NULL, 0);
    waitpid(pid_right, NULL, 0);

    sem_wait(mutex);
    merge(start, mid, end);
    sem_post(mutex);
}

void merge_sort( int left, int right) {
    struct timeval start_time, end_time;
    gettimeofday(&start_time, NULL);

    if (left < right) {
        int mid = left + (right - left) / 2;
        merge_sort(left, mid);
        merge_sort(mid + 1, right);
        merge(left, mid, right);
    }

    gettimeofday(&end_time, NULL);
    long execution_time = (end_time.tv_sec - start_time.tv_sec) * 1000000L + (end_time.tv_usec - start_time.tv_usec);
    log_sort(left, right, shared_data->array, shared_data->size, execution_time);
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
