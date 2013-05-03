include ./makefile.config

OBJ_EXT= .o
CXXSRC_EXT= .cpp
CSRC_EXT= .c
LIB_EXT= .a
H_EXT= .h

OBJECTS = $(OBJ_DIR)config$(OBJ_EXT)\
	$(OBJ_DIR)const_def$(OBJ_EXT)

LIB_TARGET = $(LIB_DIR)libconfig$(LIB_EXT)

 CHECK:
	 $(PATH_QZONEBASECLASS_PRJ)/makeinclude/make_check

$(OBJ_DIR)%$(OBJ_EXT): $(SRC_DIR)%$(CXXSRC_EXT)
	@echo
	@echo "Compiling $< ==> $@..."
	$(CXX) $(INC) $(C_FLAGS) -c $< -o $@

$(OBJ_DIR)%$(OBJ_EXT): $(SRC_DIR)%$(CSRC_EXT)
	@echo
	@echo "Compiling $< ==> $@..."
	$(CC) $(INC) $(C_FLAGS) -DQZONE_LIB_STATIC -c $< -o $@


all:CHECK $(LIB_TARGET) $(OBJECTS)
	@echo
	$(AR) rc $(LIB_TARGET) $(OBJECTS) 
	cp -f ./*$(H_EXT) $(H_DIR)
	 @echo "ok"

clean:
	 rm -f $(LIB_TARGET) $(OBJECTS)
	 sh ../clean_h.sh

 $(LIB_TARGET): $(OBJECTS)



