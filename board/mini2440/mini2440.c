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

/* FCLK = 405 MHz, HCLK = 101 MHz, PCLK = 50 MHz, UCLK = 48 MHz */
#define CLKDIVN_VAL	5
#define M_MDIV		0x7f
#define M_PDIV		0x2
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

	gpio->GPACON = 0x007FFFFF;
	// port B outputs reconfigured
	gpio->GPBCON = 	
					(0x1 <<  0) | // GPB0	OUT	TOUT0	PWM Buzzer
					(0x1 <<  2) | // GPB1	OUT				LCD
					(0x1 <<  4) | // GPB2	OUT	L3MODE
					(0x1 <<  6) | // GBP3	OUT	L3DATA
					(0x1 <<  8) | // GBP4	OUT	L3CLOCK
					(0x1 << 10) | // GBP5	OUT	LED1
					(0x1 << 12) | // GBP6	OUT	LED2
					(0x1 << 14) | // GBP7	OUT	LED3
					(0x1 << 16) | // GBP8	OUT	LED4
					(0x2 << 18) | // GBP9	---	nXDACK0		CON5 EBI
					(0x2 << 20) | // GBP10	---	nXDREQ0		CON5 EBI
					0;
	gpio->GPBUP = (1 << 10) - 1; // disable pullup on all 10 pins
	gpio->GPBDAT = 0;

	// lcd signals on C and D
	gpio->GPCCON = 0xAAAAAAAA;
	gpio->GPCUP = 0xFFFFFFFF;
	gpio->GPDCON = 0xAAAAAAAA;
	gpio->GPDUP = 0xFFFFFFFF;
	// port E is set for all it's special functions (i2c, spi etc)
    gpio->GPECON = 0xAAAAAAAA;
	gpio->GPEUP = 0x0000FFFF;

	gpio->GPFCON = 
					(0x1 <<  0) | // GPG0	EINT0	OUT
					(0x1 <<  2) | // GPG1	EINT1	OUT
					(0x1 <<  4) | // GPG2	EINT2	OUT
					(0x1 <<  6) | // GPG3	EINT3	OUT
					(0x1 <<  8) | // GPG4	EINT4	OUT
					(0x1 << 10) | // GPG5	EINT5	OUT
					(0x1 << 12) | // GPG6	EINT6	OUT
					(0x0 << 14) | // GPG7	EINT7	IN	DM9000
					0;
	gpio->GPFDAT = 0;
	gpio->GPFUP = ((1 << 7) - 1) // all disabled
					& ~( 1 << 7 ) // but for the ethernet one, we need it.
					;

	gpio->GPGCON =
					(0x0 <<  0) | // GPG0	EINT8	IN	Key1
					(0x1 <<  2) | // GPG1	EINT9	OUT				Con5
					(0x1 <<  4) | // GPG2	EINT10	OUT
					(0x0 <<  6) | // GPG3	EINT11	IN	Key2
					(0x1 <<  8) | // GPG4	EINT12	OUT
					(0x0 << 10) | // GPG5	EINT13	IN	Key3
					(0x0 << 12) | // GPG6	EINT14	IN	Key4
					(0x0 << 14) | // GPG7	EINT15	IN	Key5
					(0x1 << 16) | // GPG8	EINT16	OUT	nCD_SD
					(0x1 << 18) | // GPG9	EINT17	OUT
					(0x1 << 20) | // GPG10	EINT18	OUT
					(0x0 << 22) | // GPG11	EINT19	IN	Key6
					(0x0 << 24) | // GPG12	EINT18	IN	// Datasheet says GPG[12..15] need to be inputs
					(0x0 << 26) | // GPG13	EINT18	IN	// hard pullups
					(0x0 << 28) | // GPG14	EINT18	IN
					(0x0 << 30) | // GPG15	EINT18	IN
					0;
	gpio->GPGUP = (1 << 15) -1;	// disable pullups for all pins
	
	gpio->GPHCON = 
					(0x2 <<  0) | // GPH0	nCTS0			---
					(0x2 <<  2) | // GPH1	nRTS0			---
					(0x2 <<  4) | // GPH2	TXD0			---
					(0x2 <<  6) | // GPH3	RXD0			---
					(0x2 <<  8) | // GPH4	TXD1			---
					(0x2 << 10) | // GPH5	RXD1			---
					(0x2 << 12) | // GPH6	[TXD2]	nRTS1
					(0x2 << 14) | // GPH7	[RXD2]	nCTS1
					(0x1 << 16) | // GPH8	UEXTCLK			OUT
					(0x1 << 18) | // GPH9	CLKOUT0			OUT
					(0x1 << 20) | // GPH10	CLKOUT1			OUT
					0;
	gpio->GPHUP = (1 << 10) - 1; // disable pullups for all pins

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


