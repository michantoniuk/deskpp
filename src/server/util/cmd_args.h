#ifndef CMD_ARGS_H
#define CMD_ARGS_H

#include <string>
#include <iostream>
#include <cstring>

struct Args {
    int port = 8080;
    std::string dbPath = "deskpp.sqlite";

    void parse(int argc, char *argv[]) {
        for (int i = 1; i < argc; i++) {
            if ((strcmp(argv[i], "--port") == 0 || strcmp(argv[i], "-p") == 0) && i + 1 < argc) {
                port = std::stoi(argv[i + 1]);
                i++;
            } else if ((strcmp(argv[i], "--database") == 0 || strcmp(argv[i], "-db") == 0) && i + 1 < argc) {
                dbPath = argv[i + 1];
                i++;
            } else if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
                std::cout << "DeskPP Server\n"
                        << "Usage:\n"
                        << "  --port, -p PORT       Set server port (default: 8080)\n"
                        << "  --database, -db PATH  Set database path (default: deskpp.sqlite)\n"
                        << "  --help, -h            Show this help message\n";
                exit(0);
            }
        }
    }
};

#endif // CMD_ARGS_H