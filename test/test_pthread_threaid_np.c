#include <stdio.h>
#include <stdint.h>
#include <pthread.h>

#ifndef FEEDBACK
#define FEEDBACK 1
#endif

int main() {

#if defined(FEEDBACK) && FEEDBACK != 0
    printf("\n*** Testing pthread_threadid_np functionality...\n\n");
#endif

 int ret = 0;

 uint64_t tid = 0;
 ret = pthread_threadid_np(NULL, &tid);
 if ((ret != 0) && (tid != 0)) {
	printf("pthread_threadid failed with passed in NULL.\n\n");
 	return -1;
 }

 uint64_t my_tid = 0;
 ret = pthread_threadid_np(pthread_self(), &my_tid);
 if ((ret != 0) && (tid != 0)) {
	printf("my_pthread_threadid_np fails with passed in thread\n\n");
 	return -1;
 }

 if ((tid != 0) && (tid == my_tid)) {
#if defined(FEEDBACK) && FEEDBACK != 0
	printf("The thread_id references all match: %llu, %llu\n", tid, my_tid);
    printf("All good!\n\n");
//  std::cout << "All Good!" << std::endl;
#endif
  return 0;
 } else {
  printf("ERROR BROKEN my_pthread_threadid_np IMPLEMENTATION NOTED\n\n");
  return -1;
 }
}
