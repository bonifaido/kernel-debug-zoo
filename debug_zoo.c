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
    strcpy(leak, "This is a memory leak example");

    printk(KERN_INFO "Memory leak example: allocated %p and now leaking\n", leak);
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

// Out-of-bounds example
// This module demonstrates an out-of-bounds write error.
// It allocates a buffer of a certain size and then writes beyond its bounds.
// This is just for demonstration purposes and should not be done in production code.

static void __out_of_bounds(int num_bytes)
{
    char *buffer = kmalloc(num_bytes, GFP_KERNEL);
    if (!buffer)
    {
        printk(KERN_ERR "Memory allocation failed\n");
        return;
    }

    // This writes past the end of our 16-byte allocation!
    sprintf(buffer, "This string is definitely longer than %d bytes", num_bytes);

    kfree(buffer);
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

static int out_of_bounds = 0;
module_param(out_of_bounds, int, 0);
MODULE_PARM_DESC(out_of_bounds, "Enable out-of-bounds example");

static int __init debug_zoo_init(void)
{
    printk(KERN_INFO "Here comes the Debug Zoo!");
    printk(KERN_INFO "Use 'dmesg' to see the debug messages.");

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

    if (out_of_bounds)
    {
        __out_of_bounds(out_of_bounds);
    }

    return 0;
}

static void __exit debug_zoo_exit(void)
{
    if (lockdep)
    {
        __lockdep_exit();
    }

    if (race)
    {
        __race_exit();
    }

    printk(KERN_INFO "The animals are leaving the stage...");
    printk(KERN_INFO "Debug Zoo module unloaded");
}

module_init(debug_zoo_init);
module_exit(debug_zoo_exit);
