
objs_S    	:= start.o
objs_S    	+= lib1funcs.o 

objs_relocate_c    	+= relocate.o

hardware_nand_obj_c		:= nand_flash.o

objs_c    	+= uart.o
objs_c    	+= init.o
objs_c    	+= main.o
objs_c    	+= my_printf.o
objs_c    	+= string_utils.o
objs_c    	+= exception.o
objs_c    	+= interrupt.o
objs_c    	+= timer.o
objs_c    	+= function.o

hardware_led_obj_c		:= led.o

hardware_nor_obj_c		:= nor_flash.o

hardware_lcd_obj_c    	:= lcd_controller.o
hardware_lcd_obj_c    	+= s3c2440_lcd_controller.o
hardware_lcd_obj_c    	+= lcd.o
hardware_lcd_obj_c    	+= lcd_4.3.o
hardware_lcd_obj_c    	+= framebuffer.o
hardware_lcd_obj_c    	+= geometry.o
hardware_lcd_obj_c    	+= font_8x16.o
hardware_lcd_obj_c    	+= font.o
hardware_lcd_obj_c    	+= font_16x32.o

hardware_ts_obj_c		:=touchscreen.o
hardware_ts_obj_c		+=tslib.o

SUB_DIR     := MAIN
SUB_DIR     += HARDWARE/LED
SUB_DIR     += HARDWARE/NAND
SUB_DIR     += HARDWARE/NOR
SUB_DIR     += HARDWARE/LCD
SUB_DIR     += HARDWARE/TS
