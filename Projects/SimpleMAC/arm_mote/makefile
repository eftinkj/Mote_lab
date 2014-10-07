#*********************************************************************
#
#   HEADER NAME:
#       makefile - build file for motes
#				Author   - Mohammed Rana Basheer
#
# Copyright 2007 by ESNL.
#---------------------------------------------------------------------

#PRG defines the output file name
PRG						= esnl

#add your project files in a new line
OBJ						= STARTUP.obj								\
								task.obj									\
								hwm_wireless_cc2420.obj		\
								hwm_spi.obj								\
								hwm_pwrp.obj							\
								demo.obj


#OUTDIR points to the folder under your main project folder containing the
#hex and OM51 file, OBJDIR contains all generated obj files. INCDIR points
#to all folders containing header files. If there are multiple folders 
#seperate them with semi-colon. KEIL_PATH points to the folder containing
#keil executables
#KEIL_PATH		= D:/progra~1/8051/keil/C51/
#KEIL_PATH		= C:/keil/C51/

OUTDIR					= out
OBJDIR					= obj
LSTDIR					= lst
BIN_PATH				= bin/
INC_PATH				= inc
INCDIR					= INCDIR\(.\; $(KEIL_PATH)$(INC_PATH)\)
AS							= $(KEIL_PATH)$(BIN_PATH)a51.exe
CC							= $(KEIL_PATH)$(BIN_PATH)c51.exe
LD							= $(KEIL_PATH)$(BIN_PATH)BL51.EXE

#BUILD_TYPE can be either debug or release mode. 
ifeq ($(BUILD_TYPE),RELEASE)
	#Release mode
	CFLAGS				= OT\(8, SPEED\) LARGE  $(INCDIR) WL\(1\) PRINT\($(LSTDIR)/$(subst .,.lst_,$<)\)
	ASFLAGS				= SET\(LARGE\) PRINT\($(LSTDIR)/$(subst .,.lst_,$<)\)
	LDFLAGS				= XDATA \(0X0000-0X1FFF\) DATA \(30H\) RS \(256\) NODEBUGLINES  NODEBUGSYMBOLS NOOVERLAY
else
	#Debug Mode
	CFLAGS				= DB OE LARGE $(INCDIR) CODE WL\(1\) PRINT\($(LSTDIR)/$(subst .,.lst_,$<)\)
	ASFLAGS				= SET\(LARGE\) DEBUG EP PRINT\($(LSTDIR)/$(subst .,.lst_,$<)\)
	LDFLAGS				= XDATA \(0X0000-0X1FFF\) DATA \(30H\) RS \(256\) NOOVERLAY
endif

comma:= ,
empty:=
space:= $(empty) $(empty)

all: 
#	$(OBJDIR) \
#			$(OUTDIR)	\
#			$(LSTDIR)	\
#			$(OUTDIR)/$(PRG)

###including the dependency files
%.dep:
#	@echo generating dependency;																													\
#	gdeps32 . > $(@:.dep=.d);																															\
#	sed -e 's/\[//' -e 's/\]//' -e 's/^\(.*\):/.\/$(OBJDIR)\/\1\:/' < $(@:.dep=.d) > $@;	\
#	rm -rf $(@:.dep=.d)

#-include $(PRG).dep


$(OBJDIR):
	@echo creating directory $(OBJDIR)
	mkdir	$(OBJDIR)

$(OUTDIR):
	@echo creating directory $(OUTDIR)
	mkdir	$(OUTDIR)

$(LSTDIR):
	@echo creating directory $(LSTDIR)
	mkdir	$(LSTDIR)


#$(OBJ:%.obj=$(OBJDIR)/%.obj)
$(OUTDIR)/$(PRG): $(OBJ:%.obj=$(OBJDIR)/%.obj)
	@echo linking files to make $@;
	$(LD) $(subst $(space),$(comma),$^) TO $@ $(LDFLAGS) 
	
###Generating the object files
$(OBJDIR)/%.obj : %.c
				@echo compiling $<;
				$(CC) $< $(CFLAGS) OJ\($@\)

$(OBJDIR)/%.obj : %.a51
				@echo compiling $<;
				$(AS) $< $(ASFLAGS) OJ\($@\)


clean:	clean_subdirs
	@echo Cleaning up
	@del /Q /S /F *.#1 2>NUL
	@del /Q /S /F *.#2 2>NUL
	@del /Q /S /F *.#3 2>NUL
	@del /Q /S /F *.obj 2>NUL
	@del /Q /S /F *.lst 2>NUL
	@del /Q /S /F *.sym 2>NUL
	@del /Q /S /F *.rel 2>NUL
	@del /Q /S /F *.adb 2>NUL
	@del /Q /S /F *.asm 2>NUL
	@del /Q /S /F *.hex 2>NUL
	@del /Q /S /F *.HEX 2>NUL
	@del /Q /S /F *.rst 2>NUL
	@del /Q /S /F UMR_STACK.* 2>NUL

	
	@del /Q /S /F tmp.out 2>NUL
	@del /Q /S /F cyglink.txt 2>NUL


mydir = defs PRJ_CAMT_Headset_01 PRJ_NEM_Utilities_01 \
	replicator robotics_01 SECURITY

#mydir = defs doc 8051_routing_scheduling_F120 \
#	PRJ_CAMT_Headset_01 PRJ_NEM_Utilities_01 \
#	replicator robotics_01 SECURITY
	
clean_subdirs:
#	@for %%i in ($(mydir)); do \
#		$(MAKE) clean -C %%i




#	rm -rf $(OUTDIR)/*
#	rm -rf $(OBJDIR)/*
#	rm -rf $(LSTDIR)/*
#	rm -rf $(PRG).dep


