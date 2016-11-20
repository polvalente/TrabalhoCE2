CPP_C=g++
CPP_LD=g++
CPP_FLAGS=-std=c++0x -Wall -Werror
CPP_LD_FLAGS=-std=c++0x -o

SRCS=main.cpp\
     functions.cpp\
     classes.cpp

OBJS=${SRCS:.cpp=.o}

%.o: %.cpp
	${CPP_C} ${CPP_FLAGS} -c $<
debug: 
	${CPP_C} ${CPP_FLAGS} -DDEBUG -c main.cpp
	${CPP_C} ${CPP_FLAGS} -DDEBUG -c functions.cpp
	make build

build: ${OBJS} 
	${CPP_LD} ${CPP_LD_FLAGS} Simulador ${OBJS} 

clean:
	rm -rf Simulador ${OBJS}
