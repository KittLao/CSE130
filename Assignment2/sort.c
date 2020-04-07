#include "merge.h"
#include <pthread.h>

/* LEFT index and RIGHT index of the sub-array of ARR[] to be sorted */
void singleThreadedMergeSort(int arr[], int left, int right) 
{
  if (left < right) {
    int middle = (left+right)/2;
    singleThreadedMergeSort(arr, left, middle); 
    singleThreadedMergeSort(arr, middle+1, right); 
    merge(arr, left, middle, right); 
  } 
}

/* 
 * This function stub needs to be completed
 */
struct arg_struct {
  int *arr;
  int left;
  int right;
  int part;
};

void* thread_merge_sort(void* args) {
  struct arg_struct *args_ = (struct arg_struct*)args;
  int part = args_->part;
  int left = (args_->right + 1) / 4 * part;
  int right = left + (args_->right / 4);
  args_->left = left;
  args_->right = right;
  // printf("%s%i\n", "thread_merge_sort part: ", part);
  // printf("%s%i\n", "thread_merge_sort: ", left);
  // printf("%s%i\n", "thread_merge_sort: ", right);
  singleThreadedMergeSort(args_->arr, left, right);
  return NULL;
}

void* thread_merge(void* args) {
  struct arg_struct *args_ = (struct arg_struct*)args;
  int mid = (args_->left + args_->right) / 2;
  if(args_->part == 0) {
    int left = args_->left;
    int right = mid;
    int middle = mid / 2;
    args_->left = left;
    args_->right = right;
    // printf("%s%i\n", "thread_merge part: ", args_->part);
    // printf("%s%i\n", "thread_merge: ", left);
    // printf("%s%i\n", "thread_merge: ", right);
    merge(args_->arr, left, middle, right);
  } else {
    int left = mid + 1;
    int right = args_->right;
    int middle = (right + mid) / 2;
    args_->left = left;
    args_->right = right;
    // printf("%s%i\n", "thread_merge part: ", args_->part);
    // printf("%s%i\n", "thread_merge: ", left);
    // printf("%s%i\n", "thread_merge: ", right);
    merge(args_->arr, left, middle, right);
  }
  return NULL;
}

// void display_array(int arr[], int left, int right) {
//   // int size = 0;
//   for (int i = left; i <= right; ++i)
//   {
//     printf("%i%s", arr[i], " ");
//     // size++;
//   }
// }

void multiThreadedMergeSort(int arr[], int left, int right) 
{
  // struct arg_struct args_sort;
  // args_sort.arr = arr;
  // args_sort.left = left;
  // args_sort.right = right;
  // args_sort.part = 0;

  int num_threads_sort = 4;
  pthread_t threads_sort[num_threads_sort];
  struct arg_struct args_sort[num_threads_sort];
  for(int i = 0; i < num_threads_sort; i++) {
    args_sort[i].left = left;
    args_sort[i].right = right;
    args_sort[i].part = i;
    args_sort[i].arr = arr;
  	pthread_create(&threads_sort[i], NULL, thread_merge_sort, (void*)(&args_sort[i]));
  }
  for(int i = 0; i < num_threads_sort; i++)
  	pthread_join(threads_sort[i], NULL);

  // struct arg_struct args_merge;
  // args_merge.arr = args_sort.arr;
  // args_merge.left = left;
  // args_merge.right = right;
  // args_merge.part = 0;

  for(int i = 0; i < num_threads_sort; i++) {
    for(int j = args_sort[i].left; j <= args_sort[i].right; j++)
      arr[j] = args_sort[i].arr[j];
  }

  int num_threads_merge = 2;
  pthread_t threads_merge[num_threads_merge];
  struct arg_struct args_merge[num_threads_merge];
  for(int i = 0; i < num_threads_merge; i++) {
    args_merge[i].left = left;
    args_merge[i].right = right;
    args_merge[i].part = i;
    args_merge[i].arr = arr;
    pthread_create(&threads_merge[i], NULL, thread_merge, (void*)(&args_merge[i]));
  }
  for(int i = 0; i < num_threads_merge; i++)
    pthread_join(threads_merge[i], NULL);

  for(int i = 0; i < num_threads_merge; i++) {
    for(int j = args_merge[i].left; j <= args_merge[i].right; j++)
      arr[j] = args_merge[i].arr[j];
  }

  int middle = (left + right) / 2;
  merge(arr, left, middle, right);
  // for(int i = left; i <= right; i++) {
  //   arr[i] = args_merge.arr[i];
  // }
}














