
# platform
ifneq ($(findstring windows,$(TGT)),)
	TGT_PLATFORM := windows
else
	ifneq ($(findstring mingw,$(PRE)),)
		TGT_PLATFORM := windows
	else
		TGT_PLATFORM := posix
	endif
endif

# Processor type
ifneq ($(findstring x64,$(TGT)),)
	TGT_PROC := x64
else
	ifneq ($(findstring arm,$(TGT)),)
		TGT_PROC := arm
	else
		TGT_PROC := x86
	endif
endif

# Link type - static / shared
ifneq ($(findstring static,$(TGT)),)
	TGT_LINK := static
else
	ifneq ($(findstring shared,$(TGT)),)
		TGT_LINK := shared
	endif
endif

# Set a default link type if not specified
ifndef TGT_LINK
	ifeq ($(TGT_PLATFORM),windows)
		TGT_LINK := static
	else
		TGT_LINK := shared
	endif
endif

# Create a type string
TTYPE := $(PRE)$(TGT_PLATFORM)-$(TGT_PROC)-$(TGT_LINK)

