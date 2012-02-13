
PRJ_LIBROOT := ..

#-------------------------------------------------------------------
# Project information
#-------------------------------------------------------------------
PRJ_NAME := qtwk
PRJ_DESC := Qt component for HTML application development
PRJ_TYPE := lib
PRJ_INCS := inc
PRJ_LIBS :=


#-------------------------------------------------------------------
#Configure
#-------------------------------------------------------------------
include $(PRJ_LIBROOT)/mak/init.mk

#-------------------------------------------------------------------
# Build dependencies
#-------------------------------------------------------------------
PRJ_INCS := $(PRJ_INCS) $(CFG_LIBROOT)/qtwk
BLD_SRCDIR := $(CFG_LIBROOT)/qtwk
include $(PRJ_LIBROOT)/mak/build.mk

PRJ_INCS := $(PRJ_INCS) $(CFG_LIBROOT)/qtwk
BLD_NAME := moc
BLD_TYPE := moc
BLD_EXTN := h
BLD_FLST := mainwindow network web_page
BLD_SRCDIR := $(CFG_LIBROOT)/qtwk
include $(PRJ_LIBROOT)/mak/build.mk

#-------------------------------------------------------------------
# Build the target
#-------------------------------------------------------------------
include $(PRJ_LIBROOT)/mak/go.mk
