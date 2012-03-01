
#-------------------------------------------------------------------
# What to build?
#-------------------------------------------------------------------

# htmapp
BUILDDIRS := $(BUILDDIRS) src

# cii precompiler tool
BUILDDIRS := $(BUILDDIRS) tools/cii

ifeq ($(TOOLSONLY),)

# Qt / WebKit
BUILDDIRS := $(BUILDDIRS) qtwk

# Example applications
BUILDDIRS := $(BUILDDIRS) \
							examples/dsa \
							examples/test \

# Installers	
BUILDDIRS := $(BUILDDIRS) install

endif

# doxygen
ifneq ($(DOX),)
	BUILDDIRS := $(BUILDDIRS) dox
endif

#-------------------------------------------------------------------
# multithreading
#-------------------------------------------------------------------
NOMULTI := 1
ifeq ($(NOMULTI),)
ifeq ($(NUMJOBS),)
#ifdef NUMBER_OF_PROCESSORS
NUMJOBS := -j$(NUMBER_OF_PROCESSORS)
else
NUMJOBS := -j2
endif
endif

.PHONY: all $(BUILDDIRS)
all: $(BUILDDIRS)

$(BUILDDIRS) :
	$(MAKE) $(NUMJOBS) -C $@ $(BUILDTYPE)

