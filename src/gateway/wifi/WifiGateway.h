#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>

#include "shared/IGateway.h"
#include "shared/Types.h"

class WifiGateway : public IWifiGateway {
  public:
    WifiGateway();
    void begin();
    void startAP();
    void stopAP();
    void update();
    bool isActive() const { return _active; }
    void broadcastSignals(const TeslaSignals& signals) override;

  private:
    void setupRoutes();
    String buildJson(const TeslaSignals& s);

    AsyncWebServer _server;
    AsyncWebSocket _ws;
    bool _active = false;
    uint32_t _startMs = 0;

    static constexpr const char* SSID = "TeslaCAN";
    static constexpr const char* PASSWORD = "teslacan";
    static constexpr bool HIDDEN = false;
    static constexpr uint8_t ON_MINUTES = 5;
};
