CPP_C=g++
CPP_LD=g++
CPP_FLAGS=-std=c++0x -Wall -Werror
CPP_LD_FLAGS=-std=c++0x -o

WIN_CPP_C=i686-w64-mingw32-g++
WIN_CPP_LD=i686-w64-mingw32-g++
WIN_CPP_FLAGS=-std=c++0x -Wall -Werror
WIN_CPP_LD_FLAGS=-std=c++0x -static -o


SRCS=main.cpp\
     functions.cpp\
     classes.cpp

OBJS=${SRCS:.cpp=.o}

WIN_OBJS=${SRCS:.cpp=_win.o}

%.o: %.cpp
	${CPP_C} ${CPP_FLAGS} -c $<

%_win.o: %.cpp
	${WIN_CPP_C} ${WIN_CPP_FLAGS} -c $< -o $@

debug: 
	${CPP_C} ${CPP_FLAGS} -DDEBUG -c main.cpp
	${CPP_C} ${CPP_FLAGS} -DDEBUG -c functions.cpp
	make build

build: ${OBJS} 
	${CPP_LD} ${CPP_LD_FLAGS} Simulador ${OBJS} 

rebuild:
	make clean
	make build

windows: ${WIN_OBJS}
	${WIN_CPP_LD} ${WIN_CPP_LD_FLAGS} Simulador.exe ${WIN_OBJS}

clean:
	rm -rf Simulador ${OBJS} WinSimulador.exe ${WIN_OBJS}
