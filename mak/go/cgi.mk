
PRJ_INCS := $(PRJ_INCS) $(CFG_LIBROOT)/frwk/$(FRWK)
PRJ_LIBS := frwk-$(FRWK) $(PRJ_LIBS)
BLD_FLST := main
BLD_SRCDIR := $(CFG_LIBROOT)/frwk/$(FRWK)/exe
include $(CFG_MAKROOT)/build.mk

