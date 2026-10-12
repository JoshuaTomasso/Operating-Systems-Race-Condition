.POSIX:

C++ := c++
CFLAGS := -std=c++11 -c -g2 -O0 -Wall -Wextra -Werror -pedantic-errors
DOS2UNIX := dos2unix

SRC = main.cpp
OBJ = ${SRC:.cpp=.o}
NAME = RaceCondition

all: ${NAME}

${NAME}: ${OBJ}
	${C++} -o $@ ${OBJ}

.cpp.o:
	${DOS2UNIX} $<
	${C++} ${CFLAGS} -o $@ $<

clean:
	rm -f ${OBJ} ${NAME}
