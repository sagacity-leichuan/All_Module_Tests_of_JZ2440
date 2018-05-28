all : $(hardware_i2c_obj_c) 
	
$(hardware_i2c_obj_c):%.o:%.c
	${CC} $(CPPFLAGS) $(CFLAGS) -march=armv4 -c -o $(OBJ_DIR)/$@ $<

