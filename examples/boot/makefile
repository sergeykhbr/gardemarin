include util.mak

TOP_DIR=.
OBJ_DIR = $(TOP_DIR)/obj
ELF_DIR = $(TOP_DIR)/bin


#-----------------------------------------------------------------------------
.SILENT:
  TEA = 2>&1 | tee _$@-comp.err

all: test
	$(ECHO) "    All done.\n"

boot:
	$(ECHO) "    Boot image building started:"
	$(MKDIR) ./$(OBJ_DIR)
	$(MKDIR) ./$(ELF_DIR)
	make -f make_boot TOP_DIR=$(TOP_DIR) OBJ_DIR=$(OBJ_DIR) ELF_DIR=$(ELF_DIR) $@ $(TEA)
