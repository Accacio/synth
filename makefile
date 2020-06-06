##
# Synth
#
# @file
# @version 0.1
#
OBJS=main.cc

OBJ_NAME=synth
LINKER_FLAGS=-lSDL2 -lSDL2main
compile:
	@echo "Compiling - Begin"
	@g++ -O $(OBJS) $(LINKER_FLAGS) -o $(OBJ_NAME)
	@echo "Compiling - Done"
run:
	@./$(OBJ_NAME)
# end
clean:
	@rm $(OBJ_NAME)
