TARGET = bin/dbview
SRC = $(wildcard src/*.c) 
OBJ = $(patsubst src/%.c, obj/%.o, $(SRC))

run: clean default
	./$(TARGET) -n -f ./mynewdb.db
	./$(TARGET) -f ./mynewdb.db -a "Nikolas K,221B Baker str,140"
	./$(TARGET) -f ./mynewdb.db -a "Maria Ta,The whitehouse,120"
	./$(TARGET) -f ./mynewdb.db -a "Kostas t,Down Jones 32,160"
	./$(TARGET) -f ./mynewdb.db -d "Kostas t"
	./$(TARGET) -f ./mynewdb.db -d "Kostas t"
	./$(TARGET) -f ./mynewdb.db -l

default: $(TARGET)

clean: 
	rm -f obj/*.o 
	rm -f bin/*
	rm -f *.db 

$(TARGET): $(OBJ)
	gcc -o $@ $?
obj/%.o : src/%.c 
	gcc -c $< -o $@ -Iinclude
