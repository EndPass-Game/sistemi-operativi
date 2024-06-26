#ifndef _DEF_SYSCALL_H
#define _DEF_SYSCALL_H

/**
 * @brief Prepend SYSCALL in modo che sia più chiaro che si tratta di una system call
 */

/* create thread */
#ifndef CREATETHREAD
#define SYSCALL_CREATEPROCESS 1
#else
#define SYSCALL_CREATEPROCESS CREATETHREAD
#endif

/* terminate thread */
#ifndef TERMPROCESS
#define SYSCALL_TERMPROCESS 2
#else
#define SYSCALL_TERMPROCESS TERMPROCESS
#endif

/* P a semaphore */
#ifndef PASSERN
#define SYSCALL_PASSEREN 3
#else
#define SYSCALL_PASSEREN PASSERN
#endif

/* V a semaphore */
#ifndef VERHOGEN
#define SYSCALL_VERHOGEN 4
#else
#define SYSCALL_VERHOGEN VERHOGEN
#endif

#ifndef DOIO
#define SYSCALL_DOIO 5
#else
#define SYSCALL_DOIO DOIO
#endif

#ifndef GETTIME
#define SYSCALL_GETTIME 6
#else
#define SYSCALL_GETTIME GETTIME
#endif

#ifndef CLOCKWAIT
#define SYSCALL_CLOCKWAIT 7
#else
#define SYSCALL_CLOCKWAIT CLOCKWAIT
#endif

#ifndef GETSUPPORTPTR
#define SYSCALL_GETSUPPORTPTR 8
#else
#define SYSCALL_GETSUPPORTPTR GETSUPPORTPTR
#endif

#ifndef GETPROCESSID
#define SYSCALL_GETPROCESSID 9
#else
#define SYSCALL_GETPROCESSID GETPROCESSID
#endif

#ifndef GETCHILDREN
#define SYSCALL_GETCHILDREN 10
#else
#define SYSCALL_GETCHILDREN GETCHILDREN
#endif

#endif /* _DEF_SYSCALL_H */