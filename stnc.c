#include "server.h"
#include "client.h"
#define C "-c"
#define S "-s"

int main(int argc, char **argv) {
    if (strcmp(argv[1], C) == 0) clientStart(argc, argv);
    if (strcmp(argv[1], S) == 0) serverStart(argc, argv);
    return 0;
}