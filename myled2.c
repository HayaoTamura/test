#include<linux/module.h>
#include<linux/fs.h>
#include<linux/cdev.h>
#include<linux/device.h>
#include<linux/uaccess.h>
#include<linux/io.h>
#include<linux/timer.h>
#include<asm/uaccess.h>
#include<linux/kernel.h>
#include<linux/errno.h>
#include<linux/spinlock.h>
#include<asm/uaccess.h>
#include<linux/random.h>


MODULE_AUTHOR("Hayao Tamura");
MODULE_DESCRIPTION("driver for LED control");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");

#define MYTIMER_TIMEOUT_SECS 1
#define LED_GPIO 25
static dev_t dev;
static int mode=0;
static struct timer_list timer;
static int expire_time = 500;
static struct cdev cdv;
static struct class *cls = NULL;
static volatile u32 *gpio_base = NULL;


static void refresh_timer(void);
//struct timer_list mytimer;
//int sw = 0;
//static int access_num;




/*
static void mytimer_func(unsigned long arg){
        printk(KERN_ALERT "1 seconds.\n");
        if(sw == 0){
                gpio_base[10] = 1 << LED_GPIO;
                sw = 1;
        }
        else if(sw == 1){
                gpio_base[7] = 1 << LED_GPIO;
                sw = 0;
        }
        mod_timer(&mytimer, jiffies + MYTIMER_TIMEOUT_SECS*HZ);
        }


*/
static void timer_handler(unsigned long data)
{
        if(mode == 0)
                gpio_base[10] =1 << 25;
        else if(mode == 1)
                gpio_base[7] = 1 << 25;
        printk(KERN_INFO "mode %d\n",mode);
        refresh_timer();
}

static void refresh_timer(void)
{
        int expire_tick;
        if(mode == 1) mode=0;
        else mode =1;

        init_timer(&timer);
        expire_tick = expire_time * HZ / 1000;

        if(expire_tick ==0)expire_tick =1;

        timer.expires = jiffies + expire_tick;
        timer.data=jiffies;
        timer.function = timer_handler;
        add_timer(&timer);
}


static ssize_t led_write(struct file* filp, const char* buf,size_t count,loff_t*pos){
        char c;
        if(copy_from_user(&c,buf,sizeof(char)))
                return -EFAULT;
        if(c == '0')
                gpio_base[10] = 1 << LED_GPIO;
        else if(c == '1')
                gpio_base[7] = 1 << LED_GPIO;
/*      else if(c== '2'){
        gpio_base[7] = 1 <<LED_GPIO;
        init_timer(&mytimer);
        mytimer.expires =jiffies + MYTIMER_TIMEOUT_SECS * HZ;
        mytimer.data =0;
        mytimer.function = mytimer_func;
        add_timer(&mytimer);
        }
*/

        printk(KERN_INFO "led_write %c\n",c);
        return 1;

}


static struct file_operations led_fops ={
        .owner = THIS_MODULE,
        .write = led_write
};

static int __init init_mod(void){

        int retval;
        int expire_tick;
        gpio_base = ioremap_nocache(0x3f200000,0xA0);
        const u32 led = LED_GPIO;
        const u32 index = led/10;
        const u32 shift = (led%10)*3;
        const u32 mask = ~(0x7 << shift);
        gpio_base[index] = (gpio_base[index] & mask) | (0x01 << shift);
        retval = alloc_chrdev_region(&dev, 0, 1,"myled");
        if(retval<0){
                printk(KERN_ERR "alloc_chrdev_region failed.\n");
                return retval;
        }
        printk(KERN_INFO "%s is loaded. major:%d\n",__FILE__,MAJOR(dev));
        cdev_init(&cdv, &led_fops);
        retval = cdev_add(&cdv, dev, 1);


        if(retval<0){
                printk(KERN_ERR"cdev_add failed. major:%d, minor:%d",MAJOR(dev));
                return retval;}

        cls = class_create(THIS_MODULE,"myled");
        if(IS_ERR(cls)){
         printk(KERN_ERR "class_create failed.");
         return PTR_ERR(cls);
        }
        device_create(cls,NULL,dev,NULL,"myled%d",MINOR(dev));
        refresh_timer();

        return 0;
}
static void  __exit cleanup_mod(void)
{

        gpio_base[10]=1 <<25;
        del_timer(&timer);
        cdev_del(&cdv);
        device_destroy(cls, dev);
        class_destroy(cls);
        unregister_chrdev_region(dev,1);
        printk(KERN_INFO "%s is unloaded. major:%d\n",__FILE__,MAJOR(dev));
        del_timer(&timer);
     
}

module_init(init_mod);
module_exit(cleanup_mod);





