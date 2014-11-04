#ifndef RSYSADD_H
#define RSYSADD_H

int register_syscall(void *ptr);
void unregister_syscall(void);

#endif