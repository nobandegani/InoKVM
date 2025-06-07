#include "websocket_utils.h"


void WebsocketUtils::setRef(
  KeyboardUtils& InkUtils,
  MouseUtils& InmUtils,
  CameraUtils& IncUtils
){
  kUtils = &InkUtils;
  mUtils = &InmUtils;
  cUtils = &IncUtils;
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
  update();

  if (cameraActive){
    if (cameraInterval == 0){
      SendCameraFeed();
    }else{
      unsigned long now = millis();
      if (now - lastCameraSendTime >= cameraInterval) {
        lastCameraSendTime = now;
        SendCameraFeed();
      }
    }
  }
}

void WebsocketUtils::SendCameraFeed(){
  camera_fb_t * fb = cUtils->capture();
  if (fb) {
    size_t sizeInBytes = fb->len;
    float sizeInKB = sizeInBytes / 1024.0;

    Serial.print("Frame size: ");
    Serial.print(sizeInKB, 2);
    Serial.println(" KB");

    wsClient.sendBinary((const char*)fb->buf, fb->len);
  }
  cUtils->releaseCamera(fb);
}

void WebsocketUtils::update(){
  //Serial.println("WS update");
  wsClient.poll();
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