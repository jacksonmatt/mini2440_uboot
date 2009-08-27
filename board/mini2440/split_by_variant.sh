#!/bin/sh
#
# ---------------------------------------------------------
#  Set the core module defines according to Core Module
# ---------------------------------------------------------
# ---------------------------------------------------------
# Set up the mini2440 type define
# ---------------------------------------------------------

mkdir -p ${obj}include

if [ "$1" = "" ]
then
	echo "$0:: No parameters - using mini2440 REV B config"
	echo "#define CONFIG_ARCH_MINI2440_REVB" > ${obj}include/config.h
	variant="mini2440 Rev B ~ Toppoly 3.5\" Display and 128MB NAND Flash"
else
	case "$1" in

	mini2440_config)
	echo "#define CONFIG_ARCH_MINI2440_REVA" > ${obj}include/config.h
	variant="mini2440 Rev A ~ NEC 3.5\" Display and 64MB NAND Flash"
	;;

	mini2440v1_config)
	echo "#define CONFIG_ARCH_MINI2440_REVA" > ${obj}include/config.h
	variant="mini2440 Rev A ~ NEC 3.5\" Display and 64MB NAND Flash"
	;;

	mini2440v2_config)
	echo "#define CONFIG_ARCH_MINI2440_REVB" > ${obj}include/config.h
	variant="mini2440 Rev B ~ Toppoly 3.5\" Display and 128MB NAND Flash"
	;;

	esac

fi

# ---------------------------------------------------------
# Complete the configuration
# ---------------------------------------------------------
$MKCONFIG -a mini2440 arm arm920t mini2440 NULL s3c24x0
echo "Variant:: $variant"
