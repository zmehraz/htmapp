
# Qt WebKit framework?
ifneq ($(findstring qtwk,$(PRJ_FWRK)),)
	include $(CFG_MAKROOT)/go/qtwk.mk
endif

GO_LIBS := $(foreach inc,$(PRJ_LIBS),-l$(inc))

ifdef SYSLIBS
	GO_LIBS := $(GO_LIBS) $(foreach inc,$(SYSLIBS),-l$(inc))
endif

ifdef SYSLIBP
	GO_LIBS := $(GO_LIBS) $(foreach inc,$(SYSLIBP),-L$(inc))
endif

#-------------------------------------------------------------------
# Build target
#-------------------------------------------------------------------
$(CFG_OUTFILE): $(BLDTGT) $(BLD_DEPENDS)
	$(CFG_LD) $(CFG_LD_FLAGS) $(BLD_DEPENDS) $(GO_LIBS) -o "$@"
BLDTGT := $(CFG_OUTFILE)

#-------------------------------------------------------------------
# Make it go ;)
#-------------------------------------------------------------------
.PHONY: all
all: $(BLDTGT)

