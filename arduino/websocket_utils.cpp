#include "websocket_utils.h"


void WebsocketUtils::setRef(
  KeyboardUtils& InkUtils,
  MouseUtils& InmUtils,
  CameraUtils& IncUtils
){
  kUtils = &InkUtils;
  mUtils = &InmUtils;
  cUtils = &IncUtils;

  cameraQueue = xQueueCreate(cameraQueueSize, sizeof(camera_fb_t*));

  xTaskCreatePinnedToCore(
    cameraCaptureTask,   // task function
    "CameraCapture",         // name
    8192,                 // stack size
    this,                 // param
    1,                    // priority
    &cameraCaptureHandle,    // handle
    1                     // core 1 (ESP32 has 2 cores)
  );

  xTaskCreatePinnedToCore(
    cameraSendTask,   // task function
    "CameraSend",         // name
    8192,                 // stack size
    this,                 // param
    1,                    // priority
    &cameraSendHandle,    // handle
    1                     // core 1 (ESP32 has 2 cores)
  );

  dummyData.resize(5 * 1024);
  for (size_t i = 0; i < dummyData.size(); ++i) {
      dummyData[i] = i % 256;
  }
}

void WebsocketUtils::setWifi(
  String InSsid, 
  String InPass
){
  ssid_Router = InSsid;
  password_Router = InPass;

	Serial.println("Wifi start");
	WiFi.begin(ssid_Router.c_str(), password_Router.c_str());
	Serial.println(String("Connecting to ")+ssid_Router);
	while (WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
	Serial.println("\nWifi Connected, IP address: ");
  Serial.println(WiFi.localIP());
}
void WebsocketUtils::setWebsocket(
  bool InSsl, 
  String InServer, 
  uint16_t InPort,
  String InToken
){
  use_ssl = InSsl;
  server = InServer;
  port = InPort;
  token = InToken;

  String wsAdd = server + ":" + port + "/?token=" + token;
	if (use_ssl){
    setCert();
    wsAdd = "wss://" + wsAdd;
  }else{
    wsAdd = "ws://" + wsAdd;
  }

  Serial.print("Connecting to WebSockets Server @ ");
  Serial.println(wsAdd);

  wsClient.onMessage([this](websockets::WebsocketsMessage msg){
    this->onMessageCallback(msg);
  });
  wsClient.onEvent([this](websockets::WebsocketsEvent event, String data){
      this->onEventsCallback(event, data);
  });

  wsClient.connect(wsAdd);

	Serial.print("Connected to WebSockets Server @ ");
  Serial.println(wsAdd);
}

void  WebsocketUtils::setConf(
  uint32_t InDelay,
  unsigned int InCameraInterval
){
  delays = InDelay;
  cameraInterval = InCameraInterval;
}

void WebsocketUtils::loop(){
  wsClient.poll();
}

void WebsocketUtils::cameraCaptureTask(void *param) {
  WebsocketUtils* self = static_cast<WebsocketUtils*>(param);
  
  while (true) {
    if (self->cameraActive){
      self->cameraCapture();
    }
    if (self->cameraInterval != 0){
      vTaskDelay(self->cameraInterval / portTICK_PERIOD_MS);
    }
  }
}

void WebsocketUtils::cameraCapture(){
  camera_fb_t * fb = cUtils->capture();
  if (fb) {
    if (xQueueSend(cameraQueue, &fb, 0)) {
      unsigned long now = millis();
      unsigned long delta = now - cameraCaptureTime;
      cameraCaptureTime = now;

      Serial.print("Camera capture success:");
      Serial.print(delta);
      Serial.println(" ms");
      //printMemoryUsage();
    }else{
      cUtils->releaseCamera(fb);
      Serial.println("❌ Camera capture fail, queue full");
    }
  }
}

void WebsocketUtils::cameraSendTask(void *param) {
  WebsocketUtils* self = static_cast<WebsocketUtils*>(param);
  
  while (true) {
    if (self->cameraActive){
      self->cameraSend();
    }
    if (self->cameraInterval != 0){
      vTaskDelay(self->cameraInterval / portTICK_PERIOD_MS);
    }
  }
}

void WebsocketUtils::cameraSend(){
  camera_fb_t *fb = nullptr;
  if (xQueueReceive(cameraQueue, &fb, portMAX_DELAY)) {
    unsigned long now = millis();
    unsigned long delta = now - cameraSendTime;
    cameraSendTime = now;

    size_t sizeInBytes = fb->len;
    float sizeInKB = sizeInBytes / 1024.0;

    Serial.print("Camera send success:");
    Serial.print(delta);
    Serial.print("ms, size:");
    Serial.print(sizeInKB, 2);
    Serial.println(" KB");
    //wsClient.sendBinary((const char*)fb->buf, fb->len);
    cUtils->releaseCamera(fb);
  }else{
    Serial.println("❌ Camera send failed, queue recieve failed");
  }
}


void WebsocketUtils::onMessageCallback(websockets::WebsocketsMessage message) {
    if (message.isEmpty()){
      Serial.println("WS msg is empty");
    }
    if (!message.isComplete()){
      Serial.println("WS msg is not complete");
    }
    if (message.isText()){
      Serial.println("WS type is TEXT");
			solve_json_command(message.data());
    }
    if (message.isBinary()){
      Serial.println("WS type is Bin");
    }

}

void WebsocketUtils::onEventsCallback(websockets::WebsocketsEvent event, String data) {
    if(event == websockets::WebsocketsEvent::ConnectionOpened) {
        Serial.println("WS Connnection Opened");
    } else if(event == websockets::WebsocketsEvent::ConnectionClosed) {
        Serial.println("WS Connnection Closed");
    } else if(event == websockets::WebsocketsEvent::GotPing) {
        Serial.println("WS Got a Ping!");
    } else if(event == websockets::WebsocketsEvent::GotPong) {
        Serial.println("WS Got a Pong!");
    }
}

void WebsocketUtils::solve_json_command(String payload){
	String j_key = "";
	
	JSONVar payload_json = JSON.parse(payload);
	if (JSON.typeof(payload_json) == "undefined") {
    Serial.println("WS payload json is invalid!");
    return;
  }

  j_key = "camera_active";
	if (payload_json.hasOwnProperty(j_key)) {
		bool j_return = (bool) payload_json[j_key];
		Serial.print("Json is camera active:");
		Serial.println(j_return);
		cameraActive = j_return;
  }

	j_key = "random_mouse";
	if (payload_json.hasOwnProperty(j_key)) {
		bool j_return = (bool) payload_json[j_key];
		Serial.print("Json is random mouse:");
		Serial.println(j_return);
		mUtils->random_move = j_return;
  }

  j_key = "mouse";
	if (payload_json.hasOwnProperty(j_key)) {
		int dx = (int) payload_json[j_key]["dx"];
    int dy = (int) payload_json[j_key]["dy"];
		
		Serial.print("Json is mouse move:");
    Serial.print(dx);
    Serial.print(",");
    Serial.println(dy);
    mUtils->move(dx, dy);
  }

  j_key = "mouse_click";
	if (payload_json.hasOwnProperty(j_key)) {
		String j_return = (String) payload_json[j_key];
		Serial.print("Json is mouse_click:");
		Serial.println(j_return);
    mUtils->mouse_click(j_return);
  }

	j_key = "keys";
	if (payload_json.hasOwnProperty(j_key)) {
		JSONVar keys = payload_json[j_key]; 
		
		Serial.println("Received keys:");
		
		// Press all keys in the array
		for (int i = 0; i < keys.length(); i++) {
			String keyStr = (String)keys[i];

			Serial.print("- ");
			Serial.println(keyStr);

			uint8_t keyCode = kUtils->parse_key(keyStr);
			if (keyCode != 0) {
				kUtils->press(keyCode);
			} else {
				Serial.println("❌ Unknown key");
			}
		}

		kUtils->releaseAll(50);
	}
}

void WebsocketUtils::setCert(){
  const char ssl_ca_cert[] PROGMEM = \
    "-----BEGIN CERTIFICATE-----\n" \
    "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\n" \
    "-----END CERTIFICATE-----\n";

  wsClient.setCACert(ssl_ca_cert);
}

void WebsocketUtils::printMemoryUsage() {
    // SRAM (internal)
    size_t free_heap = heap_caps_get_free_size(MALLOC_CAP_INTERNAL);
    size_t total_heap = heap_caps_get_total_size(MALLOC_CAP_INTERNAL);
    size_t used_heap = total_heap - free_heap;

    // PSRAM (external)
    size_t free_psram = heap_caps_get_free_size(MALLOC_CAP_SPIRAM);
    size_t total_psram = heap_caps_get_total_size(MALLOC_CAP_SPIRAM);
    size_t used_psram = total_psram - free_psram;

    Serial.println("====== Memory Stats ======");
    Serial.printf("SRAM  Total: %d KB | Used: %d KB | Free: %d KB\n", total_heap / 1024, used_heap / 1024, free_heap / 1024);
    Serial.printf("PSRAM Total: %d KB | Used: %d KB | Free: %d KB\n", total_psram / 1024, used_psram / 1024, free_psram / 1024);
    Serial.println("===========================");
}
