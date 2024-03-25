include util.mak

TOP_DIR=.
OBJ_DIR = $(TOP_DIR)/obj
ELF_DIR = $(TOP_DIR)/bin


#-----------------------------------------------------------------------------
.SILENT:
  TEA = 2>&1 | tee _$@-comp.err

all: app_os_simple
	$(ECHO) "    All done.\n"

app_os_simple:
	$(ECHO) "    FreeRTOS simple application image building started:"
	$(MKDIR) ./$(OBJ_DIR)
	$(MKDIR) ./$(ELF_DIR)
	make -f make_app_os_simple TOP_DIR=$(TOP_DIR) OBJ_DIR=$(OBJ_DIR) ELF_DIR=$(ELF_DIR) $@ $(TEA)
