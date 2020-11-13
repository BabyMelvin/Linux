#include <asm/uaccess.h>
#include <linux/module.h>
#include <linux/proc_fs.h>

static struct proc_dir_entry* my_dir;
static struct proc_dir_entry* pfile;
static char msg[255];

static int myproc_write(struct file* file, const char __user* buffer, unsigned long count, void* data)
{
    printk(KERN_ERR "melvin called ==%s==\n", __func__);
    unsigned long count2 = count;
    if (count2 >= sizeof(msg)) {
        count2 = sizeof(msg) - 1;
    }
    if (copy_from_user(msg, buffer, count2)) {
        return -EFAULT;
    }
    msg[count2] = '\0';
    return count;
}

static int myproc_read(char* page, char** start, off_t off, int count, int* eof, void* data)
{
    printk(KERN_ERR "melvin called ==%s==\n", __func__);
    int len = strlen(msg);
    if (off > len) {
        return 0;
    }

    if (count > len - off) {
        count = len - off;
    }
    memcpy(page + off, msg + off, count);
    return off + count;
}

static void __exit myproc_exit(void)
{
    printk(KERN_ERR "melvin called ==%s==\n", __func__);
    remove_proc_entry("poll", my_dir);
    remove_proc_entry("mydir", NULL);
}

struct file_operations proc_fops = {
    .read = myproc_read,
    .write = myproc_write,
    .owner = THIS_MODULE,
};

static int __init myproc_init(void)
{
    printk(KERN_ERR "melvin called ==%s==\n", __func__);
    my_dir = proc_mkdir("my_dir", NULL);
    if (!my_dir) {
        printk(KERN_ERR "can't create /proc/mydir\n");
        return -1;
    }

    pfile = proc_create("pool", 0666, my_dir, &proc_fops);
    if (!pfile) {
        printk(KERN_ERR "create file failed\n");
        return -1;
    }

    return 0;
}
module_init(myproc_init);
module_exit(myproc_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("melvin");
