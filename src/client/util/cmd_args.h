#ifndef CLIENT_CMD_ARGS_H
#define CLIENT_CMD_ARGS_H

#include <string>
#include <iostream>
#include <cstring>

struct ClientArgs {
    std::string serverUrl = "http://localhost:8080";
    int port = 0; // Default to 0 which means unset

    void parse(int argc, char *argv[]) {
        for (int i = 1; i < argc; i++) {
            if ((strcmp(argv[i], "--server") == 0 || strcmp(argv[i], "-s") == 0) && i + 1 < argc) {
                serverUrl = argv[i + 1];
                i++;
            } else if ((strcmp(argv[i], "--port") == 0 || strcmp(argv[i], "-p") == 0) && i + 1 < argc) {
                port = std::stoi(argv[i + 1]);
                i++;
            } else if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
                std::cout << "DeskPP Client\n"
                        << "Usage:\n"
                        << "  --server, -s URL      Set server URL (default: http://localhost:8080)\n"
                        << "  --port, -p PORT       Set server port (overrides port in server URL)\n"
                        << "  --help, -h            Show this help message\n";
                exit(0);
            }
        }
    }
};

#endif // CLIENT_CMD_ARGS_H