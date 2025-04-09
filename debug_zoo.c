#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/delay.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Debug Boss");
MODULE_DESCRIPTION("A debugging demonstration kernel module");
MODULE_VERSION("1.0");

// Simple leak example
// This module allocates memory without freeing it, causing a memory leak.
// This is just for demonstration purposes and should not be done in production code.

static void __leak(void)
{
    char *leak = kmalloc(100, GFP_KERNEL);
    if (!leak)
    {
        printk(KERN_ERR "Memory allocation failed\n");
        return;
    }
    // Memory is allocated but never freed, causing a memory leak
    // Uncomment the next line to free the memory and avoid the leak
    // kfree(leak);
}

// Use-after-free example
// This module demonstrates a use-after-free error.
// It allocates memory, frees it, and then tries to use the freed memory.
// This is just for demonstration purposes and should not be done in production code.

static void __use_after_free(void)
{
    char *leak = kzalloc(100, GFP_KERNEL);
    if (!leak)
    {
        printk(KERN_ERR "Memory allocation failed\n");
        return;
    }
    strcpy(leak, "This is a use-after-free example");
    kfree(leak);
    // Using the pointer after freeing it, which is undefined behavior
    printk(KERN_INFO "Using freed memory: %s\n", leak); // This is dangerous!
}

// Lockdep example
// This module demonstrates a lock dependency issue.
// It creates two threads that acquire locks in different orders,
// which can lead to deadlocks or other synchronization issues.
// This is just for demonstration purposes and should not be done in production code.

static struct task_struct *lock_thread1;
static struct task_struct *lock_thread2;

static DEFINE_MUTEX(lock_a);
static DEFINE_MUTEX(lock_b);

static int lock_thread_fn1(void *data)
{
    mutex_lock(&lock_a);
    msleep(100);
    mutex_lock(&lock_b); // A -> B
    msleep(100);

    mutex_unlock(&lock_b);
    mutex_unlock(&lock_a);

    return 0;
}

static int lock_thread_fn2(void *data)
{
    msleep(50); // Ensure interleaving
    mutex_lock(&lock_b);
    msleep(100);
    mutex_lock(&lock_a); // B -> A -> should trigger lockdep

    mutex_unlock(&lock_a);
    mutex_unlock(&lock_b);

    return 0;
}

static void __lockdep(void)
{
    printk(KERN_INFO "lockdep test loaded\n");

    lock_thread1 = kthread_run(lock_thread_fn1, NULL, "lock_thread1");
    lock_thread2 = kthread_run(lock_thread_fn2, NULL, "lock_thread2");
}

static void __lockdep_exit(void)
{
    printk(KERN_INFO "lockdep test unloaded\n");
}

// Race condition example
//

static struct task_struct *race_thread1;
static struct task_struct *race_thread2;

static unsigned long shared_counter = 0;
#define LOOP_COUNT 1000000

static int race_thread_fn(void *data)
{
    int i;
    for (i = 0; i < LOOP_COUNT && !kthread_should_stop(); i++)
    {
        // Simulated race: read-modify-write with no lock
        shared_counter++;
        // Optional small delay to increase race chance
        if (i % 1000 == 0)
            udelay(1);
    }

    printk(KERN_INFO "Thread %s finished. Final value of shared_counter = %lu\n",
           current->comm, shared_counter);

    return 0;
}

static int __race_init(void)
{
    printk(KERN_INFO "Race module loaded. Starting threads...\n");
    shared_counter = 0;

    race_thread1 = kthread_run(race_thread_fn, NULL, "race_thread1");
    race_thread2 = kthread_run(race_thread_fn, NULL, "race_thread2");

    if (IS_ERR(race_thread1) || IS_ERR(race_thread2))
    {
        printk(KERN_ERR "Failed to create threads\n");
        return -1;
    }

    return 0;
}

static void __race_exit(void)
{
    printk(KERN_INFO "Final value of shared_counter = %lu (expected: %d)\n", shared_counter, 2 * LOOP_COUNT);
}

// Slab poisoning example
// This module demonstrates slab poisoning.
// It allocates memory but uses more than it should, causing a write-out-of-bounds error.
// This is just for demonstration purposes and should not be done in production code.

static void __init __slab_poison(void)
{
    pr_info("__slab_poison: loading module\n");

    // Allocate 16 bytes
    char *buf = kmalloc(16, GFP_KERNEL);
    if (!buf)
    {
        pr_err("__slab_poison: allocation failed\n");
        return;
    }

    pr_info("__slab_poison: allocated buffer at %px\n", buf);

    // Intentionally write past the end (off-by-one)
    buf[16] = 'X'; // + 1 byte past the end

    pr_info("__slab_poison: wrote out-of-bounds\n");
}

// params to which sample to run

static int leak = 0;
module_param(leak, int, 0);
MODULE_PARM_DESC(leak, "Enable memory leak example");

static int use_after_free = 0;
module_param(use_after_free, int, 0);
MODULE_PARM_DESC(use_after_free, "Enable use-after-free example");

static int lockdep = 0;
module_param(lockdep, int, 0);
MODULE_PARM_DESC(lockdep, "Enable lockdep example");

static int race = 0;
module_param(race, int, 0);
MODULE_PARM_DESC(race, "Race condition example");

static int slab_poison = 0;
module_param(slab_poison, int, 0);
MODULE_PARM_DESC(slab_poison, "Enable slab poisoning example");

static int __init my_module_init(void)
{
    if (leak)
    {
        __leak();
    }

    if (use_after_free)
    {
        __use_after_free();
    }

    if (lockdep)
    {
        __lockdep();
    }

    if (race)
    {
        __race_init();
    }

    if (slab_poison)
    {
        __slab_poison();
    }

    printk(KERN_INFO "Hello, world! This is my first kernel module.\n");

    return 0;
}

static void __exit my_module_exit(void)
{
    if (lockdep)
    {
        __lockdep_exit();
    }

    if (race)
    {
        __race_exit();
    }

    printk(KERN_INFO "Goodbye, world! My kernel module is being removed.\n");
}

module_init(my_module_init);
module_exit(my_module_exit);
