CPP = c++
FLAGS = -Wall -Wextra -Werror -std=c++98 -fsanitize=address 
NAME = webserv
SRC = main.cpp Parser/Parser.cpp Config/Config.cpp Config/Location.cpp Config/Server.cpp Request/Request.cpp
OBJ = ${SRC:.cpp=.o}

all: ${NAME}

${NAME} : ${OBJ}
	${CPP} ${FLAGS} -o ${NAME} ${SRC}

%.o: %.cpp Parser/Parser.hpp Config/Config.hpp Config/Location.hpp Config/Server.hpp Request/Request.hpp
	${CPP} ${FLAGS} -c $< -o $@

clean:
	rm -rf ${OBJ}

fclean: clean
	rm -rf ${NAME}

re: fclean all

.PHONY: clean