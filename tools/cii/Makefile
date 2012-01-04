
PRJ_LIBROOT := ../..

#-------------------------------------------------------------------
# Project information
#-------------------------------------------------------------------
PRJ_NAME := cii
PRJ_DESC := Embedded c/c++ compiler
PRJ_TYPE := exe
PRJ_INCS := 
PRJ_LIBS :=


#-------------------------------------------------------------------
#Configure
#-------------------------------------------------------------------
include $(PRJ_LIBROOT)/mak/init.mk


#-------------------------------------------------------------------
# Build dependencies
#-------------------------------------------------------------------
include $(PRJ_LIBROOT)/mak/build.mk

PRJ_INCS := $(PRJ_INCS) $(CFG_LIBROOT)/inc
BLD_TYPE := cpp
BLD_EXTN := cpp
BLD_FLST :=
BLD_SRCDIR := $(CFG_LIBROOT)/src
include $(CFG_MAKROOT)/build.mk

#-------------------------------------------------------------------
# Build the target
#-------------------------------------------------------------------
include $(PRJ_LIBROOT)/mak/go.mk
