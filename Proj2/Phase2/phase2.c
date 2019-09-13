//Mike Rossetti
//Harrison Burack

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/syscalls.h>
#include <linux/sched.h>
#include <linux/list.h>
#include <linux/time.h>

unsigned long **sys_call_table;

//struct to hold all of the information we are getting
struct processinfo{
  long state;
  pid_t pid;
  pid_t parent_pid;
  pid_t youngest_child;
  pid_t younger_sibling;
  pid_t older_sibling;
  uid_t uid;
  long long start_time;
  long long user_time;
  long long sys_time;
  long long cutime;
  long long cstime;
};

asmlinkage long (*ref_sys_cs3013_syscall2)(void);

asmlinkage long new_sys_cs3013_syscall2(struct processinfo *info){
  struct processinfo display;
  struct task_struct *ts = current;
  struct task_struct *tempTask;
  struct list_head *pos;
  long long youngest_childTime;
  long long younger_childTime;
  long long older_childTime;
  
  printk(KERN_INFO "Intercepted sys_call_cs3013_syscall2\n");
  
  //setting initial values
  display.pid = ts->pid;
  display.parent_pid = ts->parent->pid;
  display.start_time = ts->start_time;

  if(list_empty(&(ts->children))){//if there are no children
    display.youngest_child = -1;
    display.cutime = -1;
    display.cstime = -1;
  }else{//if there are children
    display.cutime = 0;
    display.cstime = 0;
    youngest_childTime = -1;
    list_for_each(pos, &(ts->children)){//iterate through each child
      tempTask = list_entry(pos, struct task_struct, sibling);
      //increments system and user time
      display.cutime += cputime_to_usecs((tempTask->utime));
      display.cstime += cputime_to_usecs((tempTask->stime));
      if(tempTask->start_time < youngest_childTime || youngest_childTime == -1){//if all children have been iterated through
        youngest_childTime = tempTask->start_time;
        display.youngest_child = tempTask->pid;
      }
    }
  }

  if(list_is_singular(&(ts->children))){//if there is only one entry in the list
    display.younger_sibling = -1;
    display.older_sibling = -1;
    younger_childTime = -1;
  }else{//if there are multiple enteries
    older_childTime = -1;
    younger_childTime = -1;
    list_for_each(pos, &(ts->children)){//list each child
      tempTask = list_entry(pos, struct task_struct, sibling);
      if(tempTask->start_time > display.start_time && (tempTask->start_time < younger_childTime || younger_childTime == -1)){
        younger_childTime = tempTask->start_time;
        display.younger_sibling = tempTask->pid;
      }
      if(tempTask->start_time < display.start_time && (tempTask->start_time > older_childTime || older_childTime == -1)){
        older_childTime = tempTask->start_time;
        display.older_sibling = tempTask->pid;
      }
    }      
  }
  
  display.uid = current_uid().val;//show the current uit
  display.state = ts->state;//print state
  display.sys_time = cputime_to_usecs(ts->stime);//print system time
  display.user_time = cputime_to_usecs(ts->utime);//print user time


  if(copy_to_user(info, &display, sizeof(display))){
    return EFAULT;
  }
  
  return 0;
}

static unsigned long **find_sys_call_table(void) {
  unsigned long int offset = PAGE_OFFSET;
  unsigned long **sct;
  
  while (offset < ULLONG_MAX) {
    sct = (unsigned long **)offset;

    if (sct[__NR_close] == (unsigned long *) sys_close) {
      printk(KERN_INFO "Interceptor: Found syscall table at address: 0x%02lX",
                (unsigned long) sct);
      return sct;
    }
    
    offset += sizeof(void *);
  }
  
  return NULL;
}

static void disable_page_protection(void) {
  /*
    Control Register 0 (cr0) governs how the CPU operates.

    Bit #16, if set, prevents the CPU from writing to memory marked as
    read only. Well, our system call table meets that description.
    But, we can simply turn off this bit in cr0 to allow us to make
    changes. We read in the current value of the register (32 or 64
    bits wide), and AND that with a value where all bits are 0 except
    the 16th bit (using a negation operation), causing the write_cr0
    value to have the 16th bit cleared (with all other bits staying
    the same. We will thus be able to write to the protected memory.

    It's good to be the kernel!
   */
  write_cr0 (read_cr0 () & (~ 0x10000));
}

static void enable_page_protection(void) {
  /*
   See the above description for cr0. Here, we use an OR to set the 
   16th bit to re-enable write protection on the CPU.
  */
  write_cr0 (read_cr0 () | 0x10000);
}

static int __init interceptor_start(void) {
  /* Find the system call table */
  if(!(sys_call_table = find_sys_call_table())) {
    /* Well, that didn't work. 
       Cancel the module loading step. */
    return -1;
  }
  
  /* Store a copy of all the existing functions */
  ref_sys_cs3013_syscall2 = (void *)sys_call_table[__NR_cs3013_syscall2];

  /* Replace the existing system calls */
  disable_page_protection();

  sys_call_table[__NR_cs3013_syscall2] = (unsigned long *)new_sys_cs3013_syscall2;
  
  enable_page_protection();
  
  /* And indicate the load was successful */
  printk(KERN_INFO "Loaded interceptor!");

  return 0;
}

static void __exit interceptor_end(void) {
  /* If we don't know what the syscall table is, don't bother. */
  if(!sys_call_table)
    return;
  
  /* Revert all system calls to what they were before we began. */
  disable_page_protection();
  sys_call_table[__NR_cs3013_syscall2] = (unsigned long *)ref_sys_cs3013_syscall2;
  enable_page_protection();

  printk(KERN_INFO "Unloaded interceptor!");
}

MODULE_LICENSE("GPL");
module_init(interceptor_start);
module_exit(interceptor_end);
