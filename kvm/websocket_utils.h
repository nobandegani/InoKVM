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

    String token;

    websockets::WebsocketsClient wsClient;

    KeyboardUtils* kUtils = nullptr;
    MouseUtils* mUtils = nullptr;
    
    QueueHandle_t cameraQueue = NULL;
    TaskHandle_t cameraCaptureHandle = NULL;
    TaskHandle_t cameraSendHandle = NULL;
    
    CameraUtils* cUtils = nullptr;

    bool cameraActive = false;
    unsigned int cameraInterval = 1000;
    unsigned long cameraCaptureTime = 0;
    unsigned long cameraSendTime = 0;

    std::vector<uint8_t> dummyData;
  
  public:
    void setConf(
      uint32_t InDelay = 1000,
      unsigned int InCameraInterval = 50
    );

    void setRef(
      KeyboardUtils& InkUtils,
      MouseUtils& InmUtils,
      CameraUtils& IncUtils
    );

    void setWifi(
      String InSsid, 
      String InPass
    );

    void setWebsocket(
      bool InSsl, 
      String InServer, 
      uint16_t InPort,
      String InToken
    );

    void loop();

    void onMessageCallback(websockets::WebsocketsMessage message);
    void onEventsCallback(websockets::WebsocketsEvent event, String data);

    void printMemoryUsage();

  private:
    void solve_json_command(String payload);

    static void cameraCaptureTask(void *param);
    void cameraCapture();

    static void cameraSendTask(void *param);
    void cameraSend();
    

    void setCert();
};






