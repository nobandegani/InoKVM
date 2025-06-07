#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include <ArduinoWebsockets.h>
#include <Arduino_JSON.h>

#include "keyboard_utils.h"
#include "mouse_utils.h"
#include "camera_utils.h"

class WebsocketUtils {
  private:
    uint32_t delays;

    String ssid_Router;
    String password_Router;

    bool use_ssl;

    String server;
    uint16_t port;

    websockets::WebsocketsClient wsClient;

    KeyboardUtils* kUtils = nullptr;
    MouseUtils* mUtils = nullptr;
    CameraUtils* cUtils = nullptr;

    unsigned long lastCameraSendTime = 0;
    unsigned int cameraInterval = 1000;
  
  public:
    void setup(
      KeyboardUtils& InkUtils,
      MouseUtils& InmUtils,
      CameraUtils& IncUtils,
      uint32_t InDelays, 
      String InSsid, 
      String InPass, 
      bool InSsl, 
      String InServer, 
      uint16_t InPort,
      unsigned int InCameraInterval
    );

    void loop();
    void update();

    void onMessageCallback(websockets::WebsocketsMessage message);
    void onEventsCallback(websockets::WebsocketsEvent event, String data);

  private:
    void solve_json_command(String payload);

    void SendCameraFeed();
};






