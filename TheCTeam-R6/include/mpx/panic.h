#ifndef MPX_PANIC_H
#define MPX_PANIC_H

#include <stdnoreturn.h>


noreturn __attribute__((no_caller_saved_registers)) void kpanic(const char *msg);

#endif
