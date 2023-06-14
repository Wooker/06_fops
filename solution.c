#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/uaccess.h>
#include <linux/fs.h>

#define DEVICE_NAME "solution_node"
#define SOLUTION_MAJOR 240

static int a = 0;
module_param(a, int, 0);
MODULE_PARM_DESC(a, "First integer");

static int read_count = 0;
static int write_count = 0;

static bool read = false;

static int solution_open(struct inode *inode, struct file *file) {
  printk(KERN_INFO "kernel_mooc In open");
  read_count++;
  return 0;
} 

static ssize_t solution_read(struct file *filp, char __user *buf, size_t count, loff_t *offset)
{
  int result, len; 
  char sum_string[15]; // Allocate a buffer to hold the sum as a string
  printk(KERN_INFO "kernel_mooc In read");

  if (read) {
    printk(KERN_INFO "kernel_mooc Already read");

    read = false;
    return 0;
  }
  printk(KERN_INFO "kernel_mooc First read, reads: %d", read_count);
  
  len = snprintf(sum_string, sizeof(sum_string), "%d %d\n", read_count, write_count); // Convert sum to string

  if (*offset >= len)
      return 0; // End of file

  result = copy_to_user(buf, sum_string, len); 
  if ( result != 0)
    return -EFAULT; // Failed to copy sum to user space

  *offset += len;
  read = true;
  return len;
}

static ssize_t solution_write(struct file *filp, const char __user *buf, size_t count, loff_t *offset)
{
  int result;
  char input_buffer[20]; // Allocate a buffer to store the user input
  printk(KERN_INFO "kernel_mooc In write");

  result = copy_from_user(input_buffer, buf, count); 

  if (result != 0) {
    printk(KERN_ERR "kernel_mooc Write copy_from_user failed");
    return -EFAULT; // Failed to copy from user space
  }

  if (sscanf(input_buffer, "%d", &a) != 1) {
    printk(KERN_ERR "kernel_mooc Write parsing failed, input_buffer: %s", input_buffer);
    return -EINVAL; // Invalid input format
  }

  printk(KERN_INFO "kernel_mooc write_count: %d", write_count);
  write_count += count;

  return count;
}

// Register operations
static struct file_operations solution_fops = {
  .owner = THIS_MODULE,
  .open= solution_open,
  .read = solution_read,
  .write = solution_write,
};

static int __init solution_init(void)
{
  int result;

  // Register the character device driver
  result = register_chrdev(SOLUTION_MAJOR, DEVICE_NAME, &solution_fops);
  if (result < 0) {
      printk(KERN_ALERT "kernel_mooc Failed to register solution module %d\n", result);
      return result;
  }

  printk(KERN_INFO "kernel_mooc Solution module is loaded\n");
  return 0;
}

static void __exit solution_exit(void)
{
    // Unregister the character device driver
    unregister_chrdev(SOLUTION_MAJOR, DEVICE_NAME);

    printk(KERN_INFO "kernel_mooc Solution module is unloaded\n");
}

module_init(solution_init);
module_exit(solution_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Zakhar Semenov");
MODULE_DESCRIPTION("Solution");