
#-------------------------------------------------------------------
# What to build?
#-------------------------------------------------------------------
BUILDDIRS := $(BUILDDIRS) src

BUILDDIRS := $(BUILDDIRS) tools/cii

ifeq ($(TOOLSONLY),)

BUILDDIRS := $(BUILDDIRS) qtwk

BUILDDIRS := $(BUILDDIRS) \
							app/dsa \
							app/test \
							app/lev \
							

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

