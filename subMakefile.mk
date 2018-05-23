
objs_S    	:= start.o
objs_S    	+= lib1funcs.o 

objs_c    	:= nand_flash.o
objs_c    	+= relocate.o
objs_c    	+= uart.o
objs_c    	+= init.o
objs_c    	+= main.o
objs_c    	+= my_printf.o
objs_c    	+= string_utils.o
objs_c    	+= exception.o
objs_c    	+= interrupt.o
objs_c    	+= nor_flash.o
objs_c    	+= function.o


hardware_obj_c := 

SUB_DIR     := MAIN
SUB_DIR     += HARDWARE
