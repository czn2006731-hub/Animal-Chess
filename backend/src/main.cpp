#include <drogon/drogon.h>

#include "ServerApp.h"

int main(int argc, char *argv[]) {
    const std::string configPath = argc > 1 ? argv[1] : "config.json";
    drogon::app().loadConfigFile(configPath);
    drogon::app().setDocumentRoot("../frontend/dist");
    animalfight::ServerApp app;
    app.initialize();
    drogon::app().run();
}

