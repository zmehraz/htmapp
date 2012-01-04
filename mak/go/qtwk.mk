
PRJ_INCS := $(PRJ_INCS) $(CFG_LIBROOT)/qtwk
BLD_SRCDIR := $(CFG_LIBROOT)/qtwk
include $(CFG_MAKROOT)/build.mk

PRJ_INCS := $(PRJ_INCS) $(CFG_LIBROOT)/qtwk
BLD_TYPE := moc
BLD_EXTN := h
BLD_FLST := mainwindow network
BLD_SRCDIR := $(CFG_LIBROOT)/qtwk
include $(CFG_MAKROOT)/build.mk

