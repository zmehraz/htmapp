
# bin
#$(CFG_BINROOT): $(BLDOUT)
#	-mkdir -p $(CFG_BINROOT)
#BLDOUT := $(CFG_BINROOT)

# bin/<type>
#$(CFG_OUTROOT): $(BLDOUT)
#	-mkdir -p $(CFG_OUTROOT)
#BLDOUT := $(CFG_OUTROOT)

# bin/<type>/obj
#$(CFG_OBJROOT): $(BLDOUT)
#	-mkdir -p $(CFG_OBJROOT)
#BLDOUT := $(CFG_OBJROOT)

# bin/<type>/obj/<project>
$(CFG_PRJROOT): $(BLDOUT)
	-mkdir -p $(CFG_PRJROOT)
BLDOUT := $(CFG_PRJROOT)

