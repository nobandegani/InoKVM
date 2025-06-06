#pragma once

#include <Arduino.h>

#include "esp_camera.h"

#define CAMERA_MODEL_ESP32S3_EYE
#include "camera_pins.h"


class CameraUtils {
  public:
    void setup();

  private:
    int cameraSetup(void);
};