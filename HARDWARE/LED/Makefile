all : $(hardware_led_obj_c) 
	
$(hardware_led_obj_c):%.o:%.c
	${CC} $(CPPFLAGS) $(CFLAGS) -march=armv4 -c -o $(OBJ_DIR)/$@ $<

