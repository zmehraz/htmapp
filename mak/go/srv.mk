
PRJ_INCS := $(PRJ_INCS) $(CFG_LIBROOT)/frwk/$(FRWK)
PRJ_LIBS := frwk-$(FRWK) $(PRJ_LIBS)
BLD_FLST := main
BLD_SRCDIR := $(CFG_LIBROOT)/frwk/$(FRWK)/exe
include $(CFG_MAKROOT)/build.mk

ifeq ($(TGT_PLATFORM),windows)
	PRJ_LIBS := $(PRJ_LIBS) ws2_32
endif
