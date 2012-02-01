
PRJ_INCS := $(PRJ_INCS) $(CFG_LIBROOT)/qtwk
PRJ_LIBS := qtwk $(PRJ_LIBS)
BLD_FLST := main
BLD_SRCDIR := $(CFG_LIBROOT)/qtwk/exe
include $(CFG_MAKROOT)/build.mk

