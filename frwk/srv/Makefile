
PRJ_LIBROOT := ../..

#-------------------------------------------------------------------
# Project information
#-------------------------------------------------------------------
PRJ_NAME := frwk-srv
PRJ_DESC := Service framework
PRJ_TYPE := lib
PRJ_INCS := inc
PRJ_LIBS :=
PRJ_FRWK := html5

#-------------------------------------------------------------------
#Configure
#-------------------------------------------------------------------
include $(PRJ_LIBROOT)/mak/init.mk

#-------------------------------------------------------------------
# Build dependencies
#-------------------------------------------------------------------
PRJ_INCS := $(PRJ_INCS) $(CFG_LIBROOT)/frwk/srv
BLD_SRCDIR := $(CFG_LIBROOT)/frwk/srv
include $(PRJ_LIBROOT)/mak/build.mk

#-------------------------------------------------------------------
# Build the target
#-------------------------------------------------------------------
include $(PRJ_LIBROOT)/mak/go.mk
