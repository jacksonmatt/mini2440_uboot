/*
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Marius Groeger <mgroeger@sysgo.de>
 *
 * (C) Copyright 2002
 * David Mueller, ELSOFT AG, <d.mueller@elsoft.ch>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <s3c2440.h>

DECLARE_GLOBAL_DATA_PTR;

#define CLKDIVN_VAL	5
#define M_MDIV		0x7f	// 0x6e
#define M_PDIV		0x2		// 0x3
#define M_SDIV		0x1

#define U_M_MDIV	0x38
#define U_M_PDIV	0x2
#define U_M_SDIV	0x2

static inline void delay (unsigned long loops)
{
	__asm__ volatile ("1:\n"
	  "subs %0, %1, #1\n"
	  "bne 1b":"=r" (loops):"0" (loops));
}

/*
 * Miscellaneous platform dependent initialisations
 */

int board_init (void)
{
	S3C24X0_CLOCK_POWER * const clk_power = S3C24X0_GetBase_CLOCK_POWER();
	S3C24X0_GPIO * const gpio = S3C24X0_GetBase_GPIO();

	/* to reduce PLL lock time, adjust the LOCKTIME register */
	clk_power->LOCKTIME = 0xFFFFFF;
	clk_power->CLKDIVN = CLKDIVN_VAL;

	/* configure UPLL */
	clk_power->UPLLCON = ((U_M_MDIV << 12) + (U_M_PDIV << 4) + U_M_SDIV);
	/* some delay between MPLL and UPLL */
	delay (10);
	/* configure MPLL */
	clk_power->MPLLCON = ((M_MDIV << 12) + (M_PDIV << 4) + M_SDIV);

	/* some delay between MPLL and UPLL */
	delay (8000);

	gpio->GPBCON = 0x00295551;
	gpio->GPBUP = 0x000007FF;

	gpio->GPCCON = 0xAAAAAAAA;
	gpio->GPCUP = 0xFFFFFFFF;
	gpio->GPDCON = 0xAAAAAAAA;
	gpio->GPDUP = 0xFFFFFFFF;

    gpio->GPECON = 0xAAAAAAAA;
	gpio->GPEUP = 0x0000FFFF;
	gpio->GPFCON = 0x000055AA;
	gpio->GPFUP = 0x000000FF;
	gpio->GPGCON = 0xFF95FF3A;
	gpio->GPGUP = 0x0000FFFF;
	gpio->GPHCON = 0x0016FAAA;
	gpio->GPHUP = 0x000007FF;

	gpio->EXTINT0=0x22222222;
	gpio->EXTINT1=0x22222222;
	gpio->EXTINT2=0x22222222;

#if 0
	/* USB Device Part */
	/*GPGCON is reset for USB Device */
	gpio->GPGCON = (gpio->GPGCON & ~(3 << 24)) | (1 << 24); /* Output Mode */
	gpio->GPGUP = gpio->GPGUP | ( 1 << 12);			/* Pull up disable */

	gpio->GPGDAT |= ( 1 << 12) ; 
	gpio->GPGDAT &= ~( 1 << 12) ; 
	udelay(20000);
	gpio->GPGDAT |= ( 1 << 12) ; 
#endif

	/* arch number of SMDK2440-Board */
//	gd->bd->bi_arch_number = MACH_TYPE_S3C2440;
#ifndef MACH_TYPE_MINI2440
#define MACH_TYPE_MINI2440 1999 /* from kernel post 2.6.28 */
#endif
	gd->bd->bi_arch_number = MACH_TYPE_MINI2440;

	/* adress of boot parameters */
	gd->bd->bi_boot_params = 0x30000100;

	icache_enable();
	dcache_enable();

#if	defined(CONFIG_MINI2440_LED)
	gpio->GPBDAT = 0x00000181;
#endif

	return 0;
}

int dram_init (void)
{
	gd->bd->bi_dram[0].start = PHYS_SDRAM_1;
	gd->bd->bi_dram[0].size = PHYS_SDRAM_1_SIZE;

	return 0;
}

/* The sum of all part_size[]s must equal to the NAND size, i.e., 0x4000000.
   "initrd" is sized such that it can hold two uncompressed 16 bit 640*480
   images: 640*480*2*2 = 1228800 < 1245184. */

unsigned int dynpart_size[] = {
    CFG_UBOOT_SIZE, 0x20000, 0x200000, 0xa0000, 0x3d5c000-CFG_UBOOT_SIZE, 0 };
char *dynpart_names[] = {
    "u-boot", "u-boot_env", "kernel", "splash", "rootfs", NULL };


