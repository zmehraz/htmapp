
#-------------------------------------------------------------------
# Build dependencies
#-------------------------------------------------------------------

# Build a list of source files
CMP_SOURCES := $(wildcard ./*.cpp)

# Build a list of object files
CMP_DEPENDS := $(subst ./,$(CFG_PRJROOT)/,$(CMP_SOURCES))
CMP_DEPENDS := $(subst .cpp,.obj,$(CMP_DEPENDS))

# cpp build
$(CFG_PRJROOT)/%.obj : ./%.cpp $(BLDTGT)
	$(CFG_PP) $< -o $@

