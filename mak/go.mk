
# Framework stuff
ifeq ($(PRJ_TYPE),exe)
	ifneq ($(findstring html5,$(PRJ_FRWK)),)
		-include $(CFG_MAKROOT)/go/$(FRWK).mk
	endif
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
	$(CFG_AR) $(CFG_AR_FLAGS) "$@" $(foreach f,$(BLD_DEPENDS),"$(f)")
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

