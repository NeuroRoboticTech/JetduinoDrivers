# GNU Make solution makefile autogenerated by Premake
# Type "make help" for usage help

ifndef config
  config=debug
endif
export config

PROJECTS := dxl

.PHONY: all clean help $(PROJECTS)

all: $(PROJECTS)

dxl: 
	@echo "==== Building dxl ($(config)) ===="
	@${MAKE} --no-print-directory -C . -f dxl.make

clean:
	@${MAKE} --no-print-directory -C . -f dxl.make clean

help:
	@echo "Usage: make [config=name] [target]"
	@echo ""
	@echo "CONFIGURATIONS:"
	@echo "   debug"
	@echo "   release"
	@echo ""
	@echo "TARGETS:"
	@echo "   all (default)"
	@echo "   clean"
	@echo "   dxl"
	@echo ""
	@echo "For more information, see http://industriousone.com/premake/quick-start"
