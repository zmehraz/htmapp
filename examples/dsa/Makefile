
PRJ_LIBROOT := ../..

#-------------------------------------------------------------------
# Project information
#-------------------------------------------------------------------

# Project Name
PRJ_NAME := DiskSpaceAnalyzer

# Project Description
PRJ_DESC := Disk Space Analyzer

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
PRJ_FWRK := qtwk

# Parameters
PRJ_CIID := { home:"http://embedded/index.htm", \
			  width:"800", \
			  height:"600", \
			  noscroll:"1", \
			  nocontext:"1" \
			}

#-------------------------------------------------------------------
#Configure
#-------------------------------------------------------------------
include $(PRJ_LIBROOT)/mak/init.mk

ifeq ($(TGT_PLATFORM),windows)
	PRJ_LIBS := $(PRJ_LIBS) QtWebKit4 QtNetwork4 QtGui4 QtCore4 phonon4
else
	PRJ_LIBS := $(PRJ_LIBS) QtWebKit QtNetwork QtGui QtCore
endif

#-------------------------------------------------------------------
# Build dependencies
#-------------------------------------------------------------------
BLD_SRCDIR := cpp
include $(PRJ_LIBROOT)/mak/build.mk


#-------------------------------------------------------------------
# Build the target
#-------------------------------------------------------------------
include $(PRJ_LIBROOT)/mak/go.mk

