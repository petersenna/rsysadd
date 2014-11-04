/*
 * rsysadd.c
 *
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/errno.h>
#include <asm/pgtable.h>
#include <asm/asm-offsets.h> //__NR_syscall_max


#define RET_OPOCDE	'\xc3'

static pte_t *pte = NULL;
static unsigned long **sys_call_table = NULL;
static size_t no_syscall_len = 0;
static long *old_ptr = NULL;
static int sysnum = -1;


/* A function like this used when a syscal is not implemented on the present architecture */
static asmlinkage long no_syscall(void){
	return -ENOSYS;
}

/* Restore kernel memory page protection */
static inline void protect_memory(void){
	set_pte_atomic(pte, pte_clear_flags(*pte, _PAGE_RW));
}

/* Unprotected kernel memory page containing for writing */
static inline void unprotect_memory(void){
	set_pte_atomic(pte, pte_mkwrite(*pte));
}

/* search by __NR_close in all kernel memory */
static inline unsigned long **find_syscall_table(void){
	unsigned long **sys_table;
	unsigned long offset = PAGE_OFFSET;

	while(offset < ULONG_MAX){
		sys_table = (unsigned long **)offset;

		if(sys_table[__NR_close] == (unsigned long *)sys_close)
			return sys_table;

		offset += sizeof(void *);
	}

	return NULL;
}


static int register_syscall(void *ptr){
	int i;
	
	/* sanity checks */
	if((!sys_call_table) || (!pte) || (!no_syscall_len))
		return -1;
	
	for(i = 0; i <= __NR_syscall_max; i++){
		if(memcmp(sys_call_table[i], no_syscall, no_syscall_len) == 0){
			old_ptr = sys_call_table[i];
			
			unprotect_memory();
			sys_call_table[i] = (long *)ptr;
			protect_memory();
			
			sysnum = i;
			break;
		}
	}
	
	return sysnum;
}
EXPORT_SYMBOL(register_syscall);


static void unregister_syscall(void){
	
	/* sanity checks */
	if((!sys_call_table) || (!pte) || (sysnum == -1))
		return;
	
	unprotect_memory();
	sys_call_table[sysnum] = old_ptr;
	protect_memory();
	
	sysnum = -1;
}
EXPORT_SYMBOL(unregister_syscall);


static int __init rsysadd_init(void){
	unsigned int level;
	size_t i = 0;
	
	sys_call_table = find_syscall_table();
	if(!sys_call_table){
		printk(KERN_INFO "sys_call_table was not found\n");
		return -1;
	}
	
	if(!(pte = lookup_address((unsigned long)sys_call_table, &level)))
		return -1;
	
	/* figure out the size of function */
	while(((char *)no_syscall)[i] != RET_OPOCDE)
		i++;
	
	no_syscall_len = i + 1;
	
	return 0;
}


static void __exit rsysadd_exit(void){
	unregister_syscall();
}

module_init(rsysadd_init);
module_exit(rsysadd_exit);

MODULE_LICENSE("GPL");
