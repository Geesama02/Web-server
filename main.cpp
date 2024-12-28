// #include <iostream>
#include "Parser/Parser.hpp"

// void handle_signal(int sig) {
//     (void)sig;
//     std::cout << "Server Terminated!" << std::endl;
//     // exit(1);
// }

int main(int argc, char **argv)
{
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
    if (parser.start_parsing(config, argv[1]))
        return (1);
    if (config.init_sockets())
        return (1);
    return (0);
}