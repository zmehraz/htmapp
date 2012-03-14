
PRJ_INCS := $(PRJ_INCS) $(CFG_LIBROOT)/frwk/$(FRWK)
PRJ_LIBS := frwk-$(FRWK) $(PRJ_LIBS)
BLD_FLST := main
BLD_SRCDIR := $(CFG_LIBROOT)/frwk/$(FRWK)/exe
include $(CFG_MAKROOT)/build.mk

ifeq ($(TGT_PLATFORM),windows)
	PRJ_LIBS := $(PRJ_LIBS) QtWebKit4 QtNetwork4 QtGui4 QtCore4 phonon4
else
	PRJ_LIBS := $(PRJ_LIBS) QtWebKit QtNetwork QtGui QtCore
endif

