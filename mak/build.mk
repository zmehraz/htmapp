
#-------------------------------------------------------------------
# Build dependencies
#-------------------------------------------------------------------

ifeq ($(BLD_TYPE),moc)
	BLD_PRJROOT := $(CFG_PRJROOT)/_moc
else
	BLD_PRJROOT := $(CFG_PRJROOT)/_obj
endif

$(BLD_PRJROOT): $(BLDTGT)
	-mkdir $@
BLDTGT := $(BLD_PRJROOT)

ifeq ($(BLD_SRCDIR),)
	BLD_SRCDIR := .
endif

ifeq ($(BLD_EXTN),)
	BLD_EXTN := cpp
endif

# Build a list of source files
ifeq ($(BLD_FLST),)
	BLD_SOURCES := $(wildcard $(BLD_SRCDIR)/*.$(BLD_EXTN))
else
	BLD_SOURCES := $(foreach file,$(BLD_FLST),$(BLD_SRCDIR)/$(file).$(BLD_EXTN))
endif

# Build a list of object files
BLD_SRCDEPS := $(subst $(BLD_SRCDIR)/,$(BLD_PRJROOT)/,$(BLD_SOURCES))
BLD_DEPENDS := $(BLD_DEPENDS) $(subst .$(BLD_EXTN),.obj,$(BLD_SRCDEPS))

# Include directories
ifneq ($(PRJ_INCS),)
	BLD_INCS := $(BLD_INCS) $(foreach inc,$(PRJ_INCS),-I$(inc))
	PRJ_INCS :=
endif
ifneq ($(CFG_INCS),)
	BLD_INCS := $(BLD_INCS) $(foreach inc,$(CFG_INCS),-I$(inc))
	CFG_INCS :=
endif

# Defines
ifneq ($(PRJ_DEFS),)
	BLD_DEFS := $(BLD_INCS) $(foreach def,$(PRJ_DEFS),-D$(def))
	PRJ_DEFS :=
endif
ifneq ($(CFG_DEFS),)
	BLD_DEFS := $(BLD_INCS) $(foreach def,$(CFG_DEFS),-D$(def))
	CFG_DEFS :=
endif

# System includes
ifdef SYSINCS
	BLD_INCS := $(BLD_INCS) $(foreach inc,$(SYSINCS),-I$(inc))
endif

ifeq ($(BLD_TYPE),moc)

# moc
.PRECIOUS: $(BLD_PRJROOT)/moc_%.cpp
$(BLD_PRJROOT)/moc_%.cpp : $(BLD_SRCDIR)/%.h $(BLDTGT)
	moc "$<" -o "$@" $(CFG_EXTR) $(BLD_DEFS) $(BLD_INCS) 

$(BLD_PRJROOT)/%.obj : $(BLD_PRJROOT)/moc_%.cpp
	$(CFG_PP) $< $(CFG_PP_FLAGS) $(CFG_EXTR) $(BLD_DEFS) $(BLD_INCS) -o $@

else

# cpp build
$(BLD_PRJROOT)/%.obj : $(BLD_SRCDIR)/%.cpp $(BLDTGT)
	$(CFG_PP) $< $(CFG_PP_FLAGS) $(CFG_EXTR) $(BLD_DEFS) $(BLD_INCS) -o $@
	
endif

