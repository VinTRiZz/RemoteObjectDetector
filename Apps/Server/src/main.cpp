#include <Components/Logger/Logger.h>

#include "endpoint/serverendpoint.hpp"

int main(int argc, char* argv[]) {
    COMPLOG_INFO("Starting RemoteObjectDetector server");
    ServerEndpoint server;
    server.start(9001, 9002);
    COMPLOG_INFO("RemoteObjectDetector server exited");
    return 0;
}
