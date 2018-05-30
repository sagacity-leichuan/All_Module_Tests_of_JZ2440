
$(BIN_DIR)/$(BIN):$(objs_S) $(hardware_nand_obj_c) $(objs_relocate_c) $(objs_c) $(hardware_nor_obj_c)  $(hardware_lcd_obj_c) $(hardware_led_obj_c) $(hardware_ts_obj_c) $(hardware_key_obj_c) 
	${LD} -T $(TOP_DIR)/$(LDS) $^ libgcc.a -o $(TEMP_DIR)/$(ELF) 
	#${LD} -Ttext 0 -Tdata 0xe80 $^ -o $(TEMP_DIR)/$(ELF)
	${OBJCOPY} -O binary -S $(TEMP_DIR)/$(ELF) $@
	${OBJDUMP} -D $(TEMP_DIR)/$(ELF) > $(TEMP_DIR)/$(DIS)