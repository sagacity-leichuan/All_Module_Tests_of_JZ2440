
include subMakefile.mk

CC      = arm-linux-gcc
LD      = arm-linux-ld
AR      = arm-linux-ar
OBJCOPY = arm-linux-objcopy
OBJDUMP = arm-linux-objdump

TOP_DIR     := $(shell pwd)
BIN_DIR     := $(TOP_DIR)/BIN
OBJ_DIR     := $(TOP_DIR)/OBJ
INCLUDEDIR 	:= $(TOP_DIR)/INCLUDE
TEMP_DIR 	:= $(TOP_DIR)/TEMP
BIN         := AllModuleTestsofJZ2440.bin
CFLAGS 		:= -Wall -O2
CPPFLAGS   	:= -nostdinc -I$(INCLUDEDIR)

LDS         := AllModuleTestsofJZ2440.lds
ELF         := AllModuleTestsofJZ2440.elf
DIS         := AllModuleTestsofJZ2440.dis

LDFLASG := -L $(shell dirname `$(CC) $(CFLAGS) -print-libgcc-file-name`) -lgcc

export CC LD OBJCOPY OBJDUMP INCLUDEDIR CFLAGS CPPFLAGS AR LDFLASG hardware_obj_c
export TOP_DIR BIN_DIR OBJ_DIR BIN TEMP_DIR LDS ELF DIS objs_S objs_c  SUB_DIR 
export hardware_lcd_obj_c hardware_led_obj_c hardware_nand_obj_c hardware_nor_obj_c
export objs_relocate_c hardware_ts_obj_c hardware_key_obj_c hardware_i2c_obj_c 
export hardware_spi_obj_c hardware_sensor_obj_c

all: CHECKDIR $(SUB_DIR) BIN_PROCESS

CHECKDIR:
	mkdir -p  $(BIN_DIR)  $(TEMP_DIR)

$(SUB_DIR):ECHO
	make -C $@
	
ECHO:
	@echo $(OBJ_DIR)
	@echo begin compile ...
	
BIN_PROCESS:
	make -C $(OBJ_DIR)
	@echo program successfully!
	
.PHONY :clean
	
clean:
	rm -rf $(OBJ_DIR)/*.o $(BIN_DIR) $(TEMP_DIR)
	@echo remove successfully!
	