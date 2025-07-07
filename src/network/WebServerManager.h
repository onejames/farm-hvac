#ifndef WEBSERVER_MANAGER_H
#define WEBSERVER_MANAGER_H

#ifdef ARDUINO
#include <ESPAsyncWebServer.h>
#endif

// Forward declare dependencies to reduce header includes
class SystemState;
class ConfigManager;
class LogManager;

class WebServerManager {
public:
    explicit WebServerManager(SystemState& systemState,
                              ConfigManager& configManager,
                              LogManager& logManager);

    void setup();

private:
    void setupApiRoutes();
    void setupSettingsRoutes();
    void setupSystemRoutes();
    void setupStaticFileServer();

    SystemState& _systemState;
    ConfigManager& _configManager;
    LogManager& _logManager;
#ifdef ARDUINO
    AsyncWebServer _server;
#endif
};

#endif // WEBSERVER_MANAGER_H