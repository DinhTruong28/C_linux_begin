Chương trình đếm số lẻ từ 1 đến N, so sánh thời gian giữa sử dụng single thread và 4 thread.





/*
thread - returns the thread id. (unsigned long int defined in bits/pthreadtypes.h)
attr - Set to NULL if default thread attributes are used. (else define members of the struct pthread_attr_t defined in bits/pthreadtypes.h) Attributes include:
detached state (joinable? Default: PTHREAD_CREATE_JOINABLE. Other option: PTHREAD_CREATE_DETACHED)
scheduling policy (real-time? PTHREAD_INHERIT_SCHED,PTHREAD_EXPLICIT_SCHED,SCHED_OTHER)
scheduling parameter
inheritsched attribute (Default: PTHREAD_EXPLICIT_SCHED Inherit from parent thread: PTHREAD_INHERIT_SCHED)
scope (Kernel threads: PTHREAD_SCOPE_SYSTEM User threads: PTHREAD_SCOPE_PROCESS Pick one or the other not both.)
guard size
stack address (See unistd.h and bits/posix_opt.h _POSIX_THREAD_ATTR_STACKADDR)
stack size (default minimum PTHREAD_STACK_SIZE set in pthread.h),
void * (*start_routine) - pointer to the function to be threaded. Function has a single argument: pointer to void.
*arg - pointer to argument of function. To pass multiple arguments, send a pointer to a structure.

*/
