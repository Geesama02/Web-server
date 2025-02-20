// #include <iostream>
#include "Parser/Parser.hpp"
#include "Request/Request.hpp"

// void handle_signal(int sig) {
//     (void)sig;
//     std::cout << "Server Terminated!" << std::endl;
//     exit(1);
// }

// void f() {system("valgrind --leak-check=yes --log-file=valgrind.txt ./webserv");}
int main(int argc, char **argv)
{
    // atexit(f);
    signal(SIGPIPE, SIG_IGN);
    if (argc > 2)
    {
        std::cerr << "Invalid Number of parameters\n";
        return (1);
    }
    Config config;
    Parser parser;
    // if (signal(SIGINT, handle_signal) == SIG_ERR) {
    //     std::cerr << "Error setting signal handler\n";
    //     return (1);
    // }
    if (parser.startParsing(config, argv[1]))
        return (1);
    if (config.startServers())
        return (1);
    // if (re)
    return (0);
}