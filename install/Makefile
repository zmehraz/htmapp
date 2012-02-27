
.PHONY : default_target
default_target: all

#-------------------------------------------------------------------
# Project
#-------------------------------------------------------------------
PRJ_NAME := dsa_installer
PRJ_DESC := Disk Space Analyzer Installer
PRJ_LIBROOT := ..

# Signing info
#PRJ_COMPANY :=
#PRJ_URL :=
#PRJ_EMAIL :=
#PRJ_COPYRIGHT :=
#PRJ_SIGN :=

#-------------------------------------------------------------------
# Configure
#-------------------------------------------------------------------
include $(PRJ_LIBROOT)/mak/init.mk

ifneq ($(TGT_PLATFORM),windows)
UNSUPPORTED := TGT_PLATFORM=$(TGT_PLATFORM) not supported
include $(PRJ_LIBROOT)/mak/unsupported.mk
else

ifneq ($(DBG),)
UNSUPPORTED := Not making installers for debug version
include $(PRJ_LIBROOT)/mak/unsupported.mk
else

ifeq ($(NSIS),)
UNSUPPORTED := NSIS root not specified
include $(PRJ_LIBROOT)/mak/unsupported.mk
else

#-------------------------------------------------------------------
# Build
#-------------------------------------------------------------------

ifneq ($(CFG_VER),)
	NSIS_VER_STR := /DDVER="$(CFG_VER)"
endif

ifneq ($(CFG_FVER),)
	NSIS_VER_STR := $(NSIS_VER_STR) /DFVER="$(CFG_FVER)"
endif

ifneq ($(TGT_PROC),)
	NSIS_PROC_STR := /DPROC="$(TGT_PROC)"
endif

ifneq ($(TGT_BUILD),)
	NSIS_BUILD_STR := /DBUILD="$(TGT_BUILD)"
endif

# Build list of tarets
NSIS_SCRIPTS := DiskSpaceAnalyzer.nsi
NSIS_INSTALLERS := $(foreach f,$(NSIS_SCRIPTS),$(CFG_OUTROOT)/Install$(subst .nsi,,$(f))$(CFG_DPOSTFIX)_$(CFG_FVER)_$(TGT_PROC).exe)
NSIS_DEL_INSTALLERS := $(foreach f,$(NSIS_INSTALLERS),$(shell $(CFG_DEL) $(f)))

# Make the installers
$(CFG_OUTROOT)/Install%$(CFG_DPOSTFIX)_$(CFG_FVER)_$(TGT_PROC).exe : %.nsi
	$(NSIS) /DOUTROOT="$(CFG_OUTROOT)" /DPOSTFIX="$(CFG_DPOSTFIX)" /DLIBROOT="$(CFG_LIBROOT)" $(NSIS_VER_STR) $(NSIS_PROC_STR) $(NSIS_BUILD_STR) $<

.PHONY : all
all : $(NSIS_INSTALLERS)

endif
endif
endif
