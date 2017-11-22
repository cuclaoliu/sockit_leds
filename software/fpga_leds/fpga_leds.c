//	fpga_leds.c
//	In qsys: leds as PIO assigned address 0x0010 connected to h2f_lw_axi_master.


#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/uaccess.h>
#include <linux/ioport.h>
#include <linux/io.h>

#define HW_LWFPGA_BASE ( 0xff200000 )
#define HW_LEDS_OFST ( 0x10 )
#define HW_REGS_SPAN ( 0x01000 )
#define HW_REGS_MASK ( HW_REGS_SPAN - 1 )
#define HW_FPGA_LEDS_BASE ( ( HW_LWFPGA_BASE ) + ( HW_LEDS_OFST ) )

void *fpga_leds_mem;

static struct device_driver fpga_leds_driver = {
	.name = "fpga_leds",
	.bus = &platform_bus_type,
};

ssize_t fpga_leds_show(struct device_driver *drv, char *buf)
{
	return 0;
}

ssize_t fpga_leds_store(struct device_driver *drv, const char *buf, size_t count)
{
	u8 value = 0;
	
	if (buf == NULL) {
		pr_err("Error, string must not be NULL\n");
		return -EINVAL;
	}

	if (kstrtou8(buf, 10, &value) < 0) {
		pr_err("Could not convert string to integer\n");
		return -EINVAL;
	}

	if (value < 0 || value > 15) {
		pr_err("Invalid value %d\n", value);
		return -EINVAL;
	}

	iowrite8(value, fpga_leds_mem);

	return count;	
}

static DRIVER_ATTR(fpga_leds, S_IWUSR, fpga_leds_show, fpga_leds_store);

MODULE_AUTHOR("Stephen");
MODULE_DESCRIPTION("FPGA LEDs");
MODULE_LICENSE("Dual BSD/GPL");

static int __init fpga_leds_init(void)
{
	int ret;
	struct resource *res;

	ret = driver_register(&fpga_leds_driver);
    if (ret < 0){
		printk("driver_register error!\n");
		return ret;
	}

	ret = driver_create_file(&fpga_leds_driver, &driver_attr_fpga_leds);
	if (ret < 0) {
		driver_unregister(&fpga_leds_driver);
		printk("driver_create_file error!\n");
		return ret;
	}

	res = request_mem_region(HW_FPGA_LEDS_BASE, HW_REGS_SPAN, "fpga_leds");
	if (res == NULL) {
		driver_remove_file(&fpga_leds_driver, &driver_attr_fpga_leds);
		driver_unregister(&fpga_leds_driver);
		printk("request_mem_region error!\n");
		return -EBUSY;
	}

	fpga_leds_mem = ioremap(HW_FPGA_LEDS_BASE, HW_REGS_SPAN);
	if (fpga_leds_mem == NULL) {
		driver_remove_file(&fpga_leds_driver, &driver_attr_fpga_leds);
		driver_unregister(&fpga_leds_driver);
		release_mem_region(HW_FPGA_LEDS_BASE, HW_REGS_SPAN);
		printk("ioremap error!\n");
		return -EFAULT;
	}

	printk("fpga_leds initial successful.\n");
	return 0;
}

static void __exit fpga_leds_exit(void)
{
	printk("fpga_leds exit.\n");
	driver_remove_file(&fpga_leds_driver, &driver_attr_fpga_leds);
	driver_unregister(&fpga_leds_driver);
	release_mem_region(HW_FPGA_LEDS_BASE, HW_REGS_SPAN);
	iounmap(fpga_leds_mem);
}

module_init(fpga_leds_init);
module_exit(fpga_leds_exit);


