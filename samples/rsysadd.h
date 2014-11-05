#ifndef RSYSADD_H
#define RSYSADD_H

int register_syscall(void *function);
void unregister_syscall(void *function);

#endif
