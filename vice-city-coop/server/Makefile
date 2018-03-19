CC = g++
CC_FLAGS = -g -std=gnu++17 -w -Wall -Wextra -pedantic-errors 

EXEC = vccoop_server_linux
SOURCES = $(wildcard *.cpp)
OBJECTS = $(SOURCES:.cpp=.o)
LDFLAGS = -lpthread -lm -ldl -llua -lstdc++fs -L./vendor/lua/src/ -I./vendor/lua/include -I../plugin-sdk/plugin_vc/

$(EXEC): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $(EXEC)
 
%.o: %.cpp
	$(CC) $(LDFLAGS) -c $(CC_FLAGS) $< -o $@

clean:
	cd ./vendor/lua/src/ && make clean && cd ../../..
	rm -f $(EXEC) $(OBJECTS)
