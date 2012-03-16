
# Apache modules are shared objects
#  apr-1 aprutil-1
#  /usr/lib/libapr-1.so /usr/lib/libaprutil-1.so


# +++ There's probably a bit more elegant way...
ifeq ($(PRJ_TYPE),exe)
	PRJ_TYPE := dll
	PRJ_LIBS := frwk-$(FRWK) $(PRJ_LIBS)
endif