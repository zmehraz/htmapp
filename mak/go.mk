
# Qt WebKit framework?
ifneq ($(findstring qtwk,$(PRJ_FWRK)),)
	include $(CFG_MAKROOT)/go/qtwk.mk
endif

GO_LIBS := -L$(CFG_OUTROOT) $(foreach inc,$(PRJ_LIBS),-l$(inc))

ifdef SYSLIBS
	GO_LIBS := $(GO_LIBS) $(foreach inc,$(SYSLIBS),-l$(inc))
endif

ifdef SYSLIBP
	GO_LIBS := $(GO_LIBS) $(foreach inc,$(SYSLIBP),-L$(inc))
endif

#-------------------------------------------------------------------
# Build target
#-------------------------------------------------------------------

ifeq ($(PRJ_TYPE),lib)

$(CFG_OUTFILE): $(BLDOUT) $(BLDTGT) $(BLD_DEPENDS)
	- rm -f $@
	$(CFG_AR) $(CFG_AR_FLAGS) $@ $(BLD_DEPENDS)
BLDTGT := $(CFG_OUTFILE)

# lib
else

$(CFG_OUTFILE): $(BLDOUT) $(BLDTGT) $(BLD_DEPENDS)
	- rm -f $@
	$(CFG_LD) $(CFG_LD_FLAGS) $(BLD_DEPENDS) $(GO_LIBS) -o "$@"
BLDTGT := $(CFG_OUTFILE)

# lib
endif

#-------------------------------------------------------------------
# Make it go ;)
#-------------------------------------------------------------------
.PHONY: all
all: $(BLDTGT)

