
#ifndef __STACKTRACE_H_INCLUDE__
#define __STACKTRACE_H_INCLUDE__

#ifndef WIN32

#include <stdio.h>

struct stacktrace;

struct stacktrace *stacktrace_get(unsigned skip);
void stacktrace_resolve(struct stacktrace *st);
void stacktrace_free(struct stacktrace *st);

void stacktrace_print(struct stacktrace *st);
void stacktrace_fprint(struct stacktrace *st, FILE *);

struct stacktrace *stacktrace_get_exc();

#endif

#endif // __STACKTRACE_H_INCLUDE__
