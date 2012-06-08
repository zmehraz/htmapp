
PRJ_LIBROOT := ../..

#-------------------------------------------------------------------
# Project information
#-------------------------------------------------------------------

# Project Name
PRJ_NAME := test

# Project Description
PRJ_DESC := HtmApp Test Application

# Project output file type
PRJ_TYPE := exe

# Resource directories (dir1 dir2 ...)
PRJ_RESD := res

# Files to compile
PRJ_CIIX := *.htm

# Include folders
PRJ_INCS :=

# Libraries to link
PRJ_LIBS := htmapp

# Framework
PRJ_FRWK := html5

# Parameters
PRJ_CIID := { home:"http://embedded/index.htm", \
			  width:"800", \
			  height:"600" \
			}

#-------------------------------------------------------------------
#Configure
#-------------------------------------------------------------------
include $(PRJ_LIBROOT)/mak/init.mk

ifeq ($(TGT_PLATFORM),windows)
	PRJ_LIBS := $(PRJ_LIBS) ws2_32
endif

#-------------------------------------------------------------------
# Build dependencies
#-------------------------------------------------------------------
#include $(PRJ_LIBROOT)/mak/build.mk

#-------------------------------------------------------------------
# Build the target
#-------------------------------------------------------------------
include $(PRJ_LIBROOT)/mak/go.mk

