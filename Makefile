MODULE:=$(shell basename $(CURDIR))
export OPENWSN_ROOT=$(CURDIR)

DIRS += $(OPENWSN_ROOT)/openstack \
		$(OPENWSN_ROOT)/drivers/common \
		$(OPENWSN_ROOT)/kernel/openos \
		$(OPENWSN_ROOT)/bsp/boards/riot-adaption \

include $(RIOTBASE)/Makefile.base
