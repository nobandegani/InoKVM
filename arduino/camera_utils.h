#pragma once

#include <Arduino.h>

#include "esp_camera.h"

#define CAMERA_MODEL_ESP32S3_EYE
#include "camera_pins.h"


class CameraUtils {
  public:
    uint32_t delays;

    void setup(uint32_t InDelay = 2000);

    void loop();

    camera_fb_t * capture();
    void releaseCamera(camera_fb_t * fb);
    
  private:
    int cameraSetup(void);
    
};