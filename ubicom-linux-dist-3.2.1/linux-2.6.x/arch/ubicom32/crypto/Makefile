#
# arch/ubicom32/crypto/Makefile
#	Makefile for building crypto library.
#
# (C) Copyright 2009, Ubicom, Inc.
#
# This file is part of the Ubicom32 Linux Kernel Port.
#
# The Ubicom32 Linux Kernel Port is free software: you can redistribute
# it and/or modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation, either version 2 of the
# License, or (at your option) any later version.
#
# The Ubicom32 Linux Kernel Port is distributed in the hope that it
# will be useful, but WITHOUT ANY WARRANTY; without even the implied
# warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
# the GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with the Ubicom32 Linux Kernel Port.  If not,
# see <http://www.gnu.org/licenses/>.
#
# Ubicom32 implementation derived from (with many thanks):
#   arch/m68knommu
#   arch/blackfin
#   arch/parisc
#
obj-$(CONFIG_CRYPTO_UBICOM32) += crypto_ubicom32.o
obj-$(CONFIG_CRYPTO_AES_UBICOM32) += aes_ubicom32.o
obj-$(CONFIG_CRYPTO_DES_UBICOM32) += des.o
obj-$(CONFIG_CRYPTO_MD5_UBICOM32) += md5.o
obj-$(CONFIG_CRYPTO_SHA1_UBICOM32) += sha1.o
obj-$(CONFIG_CRYPTO_SHA2_UBICOM32) += sha2.o

des-y := des_ubicom32.o des_check_key.o
md5-y := md5_ubicom32.o
sha1-y := sha1_ubicom32.o 
sha2-y := sha2_ubicom32.o 
