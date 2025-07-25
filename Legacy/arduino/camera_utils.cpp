#include "camera_utils.h"

/*
typedef struct {
    uint8_t * buf;              < Pointer to the pixel data 
    size_t len;                 < Length of the buffer in bytes 
    size_t width;               < Width of the buffer in pixels 
    size_t height;              < Height of the buffer in pixels 
    pixformat_t format;         < Format of the pixel data
    struct timeval timestamp;   < Timestamp since boot of the first DMA buffer of the frame
} camera_fb_t;
*/

void CameraUtils::setup(uint32_t InDelay){
  delays = InDelay;

	if (cameraSetup() != 1){
    Serial.println("Camera configuration failed!");
  }
}

void CameraUtils::loop(){
  //delay(delays);
}

camera_fb_t * CameraUtils::capture(){
  camera_fb_t * fb = NULL;
  fb = esp_camera_fb_get();
  if (fb != NULL) {
    return fb;
  }else {
    Serial.println("Camera capture failed.");
    return NULL;
  }
}

void CameraUtils::releaseCamera(camera_fb_t * fb){
  esp_camera_fb_return(fb);
}

int CameraUtils::cameraSetup(void) {
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 10000000;
  config.pixel_format = PIXFORMAT_JPEG; // for streaming
  
  if(psramFound()){
    Serial.println("Camera psram available, increasing the quality");
    config.frame_size = FRAMESIZE_VGA; //FRAMESIZE_HD FRAMESIZE_SXGA FRAMESIZE_UXGA
    config.grab_mode = CAMERA_GRAB_LATEST;
    config.fb_location = CAMERA_FB_IN_PSRAM;
    config.jpeg_quality = 20;
    config.fb_count = 5;
  }else{
    Serial.println("Camera psram not available, decreasing the quality");
    config.frame_size = FRAMESIZE_VGA; //FRAMESIZE_VGA FRAMESIZE_SVGA FRAMESIZE_HD
    config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
    config.fb_location = CAMERA_FB_IN_DRAM;
    config.jpeg_quality = 20;
    config.fb_count = 1;
  }

  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return 0;
  }

  sensor_t * s = esp_camera_sensor_get();
  // initial sensors are flipped vertically and colors are a bit saturated
  s->set_vflip(s, 1); // flip it back
  s->set_brightness(s, 1); // up the brightness just a bit
  s->set_saturation(s, 0); // lower the saturation

  Serial.println("Camera configuration complete!");
  return 1;
}