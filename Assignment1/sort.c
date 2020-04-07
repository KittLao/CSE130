#include "merge.h"
#include <sys/types.h> 
#include <sys/ipc.h> 
#include <sys/wait.h>
#include <sys/shm.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h>

/* LEFT index and RIGHT index of the sub-array of ARR[] to be sorted */
void singleProcessMergeSort(int arr[], int left, int right) 
{
  if (left < right) {
    int middle = (left+right)/2;
    singleProcessMergeSort(arr, left, middle); 
    singleProcessMergeSort(arr, middle+1, right); 
    merge(arr, left, middle, right); 
  } 
}

void displayArray(int arr[], int left, int right) {
  for(int i = left; i <= right; i++)
    printf("%i%s", arr[i], " ");
  printf("\n");
}

void multiProcessMergeSort(int arr[], int left, int right) {
  if(left < right) {
    int middle = (left + right) / 2;
    // size_t shm_size = (right - left + 1);
    key_t key = ftok("kilao", 123);
    int shm_id = shmget(key, 1024, 0666 | IPC_CREAT);
    if(shm_id < 0) {
      printf("%s\n", "shmget error.");
      exit(-1);
    }
    int *shm_arr = (int*)shmat(shm_id, (void*)0, 0);
    if(&shm_arr < 0) {
      printf("%s\n", "Error on shmat");
      exit(-1);
    }
    for(int i = 0; i <= right; i++) {
      shm_arr[i] = arr[i];
    }
    pid_t child = fork();
    if(child < 0) {
      printf("%s\n", "Error creating child.");
      exit(-1);
    }
    if(child == 0) {
      // printf("%s\n", "In child");
      int *mem = (int*)shmat(shm_id, (void*)0, 0);
      if(&mem < 0) {
        printf("%s\n", "Error on shmat");
        exit(-1);
      }
      // printf("%s\n", "In child, before sort:");
      // displayArray(mem, left, right);
      singleProcessMergeSort(mem, left, middle);
      // printf("%s\n", "In child, after sort:");
      // displayArray(mem, left, right);
      if(shmdt(mem) < 0) {
        printf("%s\n", "Error on shmdt.");
        exit(-1);
      }
      exit(1);
    }
    if(child > 0) {
      // int status;
      // waitpid(shm_id, &status, 0);
      // printf("%s\n", "In parent");
      // printf("%s\n", "In parent, before sort:");
      // displayArray(shm_arr, left, right);
      singleProcessMergeSort(shm_arr, middle + 1, right);
      // printf("%s\n", "In parent, after sort:");
      // displayArray(shm_arr, left, right);
      // int status;
      // waitpid(shm_id, &status, WNOHANG);
      wait(0);
      // printf("%s\n", "Array before merge");
      // displayArray(shm_arr, left ,right);
      // for(int i = left; i <= right; i++) {
      //   arr[i] = shm_arr[i];
      // }
      merge(shm_arr, left, middle, right);
      for(int i = left; i <= right; i++) {
        arr[i] = shm_arr[i];
      }
      if(shmdt(shm_arr) < 0) {
        printf("%s\n", "Error on shmdt.");
        exit(-1);
      }
      if(shmctl(shm_id, IPC_RMID, NULL) < 0) {
        printf("%s\n", "Error on shmctl.");
        exit(-1);
      }
    }
  }
}

/* 
 * This function stub needs to be completed
 */

// void multiProcessMergeSort(int arr[], int left, int right) {
//   if(left < right) {
//     int middle = (left + right) / 2;
//     /*
//     Create shared memory and attach it. Then copy
//     right side of array into it.
//     */
//     size_t shm_size = ((right - left))/2;
//     key_t key = ftok("kilao", 42069);
//     int shm_id = shmget(key, shm_size, IPC_CREAT);
//     if(shm_id < 0) {
//       printf("%s\n", "shmget error.");
//       exit(-1);
//     }
//     int *shm_arr = shmat(shm_id, NULL, 0);
//     if(&shm_arr < 0) {
//       printf("%s\n", "shmat error.");
//       exit(-1);
//     }
//     for(int i = left; i < middle + 1; i++) {
//       shm_arr[i] = arr[i + middle + 1];
//     }
//     /*
//     Create child to sort the shared memeory. After
//     it is sorted, detatch from it.
//     */
//     pid_t child = fork();
//     if(child < 0) { // error
//       printf("%s\n", "Error creating child.");
//       exit(-1);
//     } else if(child == 0) {
//       // If child then sort the right side of the array
//       // and exit
//       int *x = shmat(shm_id, NULL, 0);
//       if(&x < 0) {
//         printf("%s\n", "shmat error");
//         exit(-1);
//       }
//       singleProcessMergeSort(x, middle, right);
//       for(int i = left; i < middle + 1; i++) {
//        shm_arr[i] = x[i];
//       }
//       if(shmdt(x) < 0) {
//         printf("%s\n", "Error on shmdt");
//         exit(-1);
//       }
//       exit(1);
//     } else { // is parent
//       singleProcessMergeSort(arr, left, middle + 1);
//       int status;
//       waitpid(child, &status, 0);
//      for(int i = middle + 1; i < right; i++) {
//         arr[i] = shm_arr[i - middle - 1];
//       }
//       /*
//       Delete shared memory.
//       */
//       if(shmdt(shm_arr) < 0) {
//         printf("%s\n", "Error on shmdt.");
//         exit(-1);
//       }
//       merge(arr, left, middle + 1, right);
//       if(shmctl(shm_id, IPC_RMID, NULL) < 0) {
//         printf("%s\n", "Error on shmctl.");
//         exit(-1);
//       }
//     }
//   }
// }
