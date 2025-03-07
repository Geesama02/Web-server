CPP = g++
FLAGS = -Wall -Wextra -Werror -std=c++98 -g -fsanitize=address 
NAME = webserv
SRC = main.cpp Parser/Parser.cpp Config/Config.cpp Config/Location.cpp Config/Server.cpp Request/Request.cpp Request/UploadFile.cpp\
	Response/Response.cpp Cgi/Cgi.cpp Response/DirectoryResolver.cpp Client/Client.cpp
OBJ = ${SRC:.cpp=.o}

all: ${NAME}

${NAME} : ${OBJ}
	${CPP} ${FLAGS} -o ${NAME} ${SRC}

%.o: %.cpp Parser/Parser.hpp Config/Config.hpp Config/Location.hpp Config/Server.hpp Request/Request.hpp\
		Request/UploadFile.hpp Response/Response.hpp Cgi/Cgi.hpp Client/Client.hpp
	${CPP} ${FLAGS} -c $< -o $@

clean:
	rm -rf ${OBJ}

fclean: clean
	rm -rf ${NAME}

re: fclean all

.PHONY: clean
