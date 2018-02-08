MODULE := openwsn

DIRS += $(CURDIR)/openstack \
	$(CURDIR)/drivers/common \
	$(CURDIR)/kernel/openos \

include $(RIOTBASE)/Makefile.base
