
.PHONY : default_target
default_target: all

#-------------------------------------------------------------------
# Project
#-------------------------------------------------------------------
PRJ_NAME := dox
PRJ_LIBROOT := ..

#-------------------------------------------------------------------
# Configure
#-------------------------------------------------------------------
include $(PRJ_LIBROOT)/mak/init.mk

DOX_ROOT := ../../dox
DOX_FILES := $(wildcard *.dox)
DOX_OUT := $(patsubst %.dox,$(DOX_ROOT)/%/status.txt,$(DOX_FILES))

#-------------------------------------------------------------------
# Build
#-------------------------------------------------------------------

.PHONY : all mk_root

mk_root :
	- mkdir $(DOX_ROOT)

$(DOX_ROOT)/%/status.txt : mk_root
	- rm -Rf "$(DOX_ROOT)/$*"
	- $(CFG_MD) "$(DOX_ROOT)/$*"
	( cat doxygen.cfg ; \
	  cat $*.dox ; \
	  echo "PROJECT_NAME		= $*" ; \
	  echo "PROJECT_NUMBER		= $*" ; \
	  echo "OUTPUT_DIRECTORY	= $(DOX_ROOT)/$*" ;\
	) | doxygen -

all: $(DOX_OUT)
	@echo Building $(DOX_OUT)

