#
# Makefile for nfc devices
#

obj-$(CONFIG_PN544_NFC)		+= pn544.o
obj-$(CONFIG_NFC_PN533)		+= pn533.o
obj-$(CONFIG_NFC_WILINK)	+= nfcwilink.o
obj-$(CONFIG_NFC_QNCI)		+= nfc-nci.o
obj-$(CONFIG_SEC_NFC)		+= sec_nfc.o
obj-$(CONFIG_ESE_OT)		+= ese_p3.o

ccflags-$(CONFIG_NFC_DEBUG) := -DDEBUG
