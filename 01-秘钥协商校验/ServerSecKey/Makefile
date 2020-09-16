OBJ_CPP=$(patsubst src/%.cpp,obj/%.o,$(wildcard src/*.cpp))
OBJ_CC=$(patsubst src/%.cc,obj/%.o,$(wildcard src/*.cc))

CPP=g++ -g -rdynamic -std=c++11 -Wall
LIBS=-pthread -lprotobuf -ljson -lssl -lcrypto

INC=./inc
TARGET=./bin/server


$(TARGET):$(OBJ_CPP) $(OBJ_CC)
	$(CPP) $^ -o $@ $(LIBS)
./obj/%.o:./src/%.cpp
	$(CPP) -I $(INC) -c $< -o $@

./obj/%.o:./src/%.cc
	$(CPP) -I $(INC) -c $< -o $@

.PHONY:clean all
clean:
	$(RM) $(OBJ_CPP) $(OBJ_CC)

all:
	echo $(OBJ_CPP)
	echo $(OBJ_CC)
