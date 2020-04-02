CC_SRC=$(wildcard *.cc)
CC_OBJS=$(patsubst %.cc, %.o, $(CC_SRC))
CPP_SRC=$(wildcard *.cpp)
CPP_OBJS=$(patsubst %.cpp, %.o, $(CPP_SRC))
CPP=g++ -std=c++11
TARGET=protobuf
LIBS=-lpthread -lprotobuf

$(TARGET):$(CC_OBJS) $(CPP_OBJS)
	$(CPP) $^ -o $@ $(LIBS)

%.o:%.cc
	$(CPP) -c $< -o $@

%.o:%.cpp
	$(CPP) -c $< -o $@

.PTHONY:clean all

clean:
	$(RM) $(CC_OBJS) $(CPP_OBJS)

all:
	echo $(CC_SRC)
	echo $(CPP_SRC)
	echo $(CC_OBJS)
	echo $(CPP_OBJS)
