#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <crypt.h>
#include <unistd.h>

pthread_mutex_t lock;
int currentChar;
int charsComplete = 0;
int pwdCracked = 0;

struct threadArgs{
    int done;
    int keysize;
    char start[2];
    char *salt;
    char *ans;
};

void iterate(char *str, int idx, int len, int keysize, char *start, char* ans, char* salt) {
    char c;
    struct crypt_data data;
    data.initialized = 0;
    int ret;

    if(pwdCracked == 1){
        pthread_exit(&ret);
    }
    if (idx < (len - 1)) {
        for (c = 'a'; c <= 'z'; ++c) {
            str[idx] = c;

            iterate(str, idx + 1, len, keysize, start, ans, salt);
        }
    } else {
        for (c = 'a'; c <= 'z'; ++c) {
            str[idx] = c;

            char comb[10];
            sprintf(comb, "%s", start);
            strcat(comb,str);
            char* res = crypt_r(comb, salt, &data);
            if(strcmp(res, ans) == 0){
                printf("%s%s\n", start, str);
                pwdCracked = 1;
                pthread_exit(&ret);
            }
        }
    }
}

void* thread_entry(void* args){
    struct threadArgs* argPtr = (struct threadArgs*)args;
    struct crypt_data data;
    data.initialized = 0;
    int ret;
    char str[2];

    //while a starting character still needs to be computed
    while(currentChar < 123){
        //locking and unlocking currentChar between threads
        pthread_mutex_lock(&lock);
        sprintf(argPtr->start, "%c", currentChar);
        currentChar++;
        if(currentChar == 123){
            charsComplete = 1;
        }
        pthread_mutex_unlock(&lock);

        //start of computation
        printf("Thread starting at: %s\n", argPtr->start);

        //checking if single letter
        char* res = crypt_r(argPtr->start, argPtr->salt, &data);
        if(strcmp(res, argPtr->ans) == 0){
            printf("%s\n", res);
            pwdCracked = 1;
            pthread_exit(&ret);
        }

        //check for all combinations with start up to keysize - 1
        for(int i = 1; i < argPtr->keysize; i ++){
            char str[i + 1];
            memset(str, 0, i + 1);
            iterate(str, 0, i, argPtr->keysize, argPtr->start, argPtr->ans, argPtr->salt);
        }

        if (charsComplete == 1){
            pthread_exit(&ret);
        }
    }
}

int main(int argc, char* argv[]){
    //parsing user arguments and checking valid input
    if(argc != 4){
        printf("The correct usage is crack <numThreads> <keysize> <target>.\n");
        exit(0);
    }

    int numThreads = atoi(argv[1]);
    if (numThreads < 0 || numThreads > 26){
        printf("numThreads must be in the range 0 - 26\n");
        exit(0);
    }

    int keysize = atoi(argv[2]);
    if (keysize < 1 || keysize > 8){
        printf("keysize must be in the range 1 - 8\n");
        exit(0);
    }

    //getting salt
    unsigned buffSize = 100;
    char salt[3];
    salt[0] = argv[3][0];
    salt[1] = argv[3][1];
    salt[2] = '\0';
    
    //initializing threads and matching threadArgs
    pthread_t threads[numThreads];
    struct threadArgs structs[numThreads];

    pthread_mutex_init(&lock, NULL);

    //creating threads starting at currentChar
    currentChar = 97;
    for (int i = 0; i < numThreads; i++){
        structs[i].done = 0;
        structs[i].keysize = keysize;
        structs[i].salt = salt;
        structs[i].ans = argv[3];

        pthread_create(&threads[i], NULL, thread_entry, &structs[i]);
    }

    //joining threads
    for (int i = 0; i < numThreads; i++){
        pthread_join(threads[i], NULL);
    }

    return 0;
}