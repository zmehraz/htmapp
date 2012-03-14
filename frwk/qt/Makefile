
PRJ_LIBROOT := ../..

#-------------------------------------------------------------------
# Project information
#-------------------------------------------------------------------
PRJ_NAME := frwk-qt
PRJ_DESC := Qt framework for HTML5 application development
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
PRJ_INCS := $(PRJ_INCS) $(CFG_LIBROOT)/frwk/qt
BLD_SRCDIR := $(CFG_LIBROOT)/frwk/qt
include $(PRJ_LIBROOT)/mak/build.mk

PRJ_INCS := $(PRJ_INCS) $(CFG_LIBROOT)/frwk/qt
BLD_NAME := moc
BLD_TYPE := moc
BLD_EXTN := h
BLD_FLST := mainwindow network web_page
BLD_SRCDIR := $(CFG_LIBROOT)/frwk/qt
include $(PRJ_LIBROOT)/mak/build.mk

#-------------------------------------------------------------------
# Build the target
#-------------------------------------------------------------------
include $(PRJ_LIBROOT)/mak/go.mk
