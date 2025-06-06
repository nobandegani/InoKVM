#include "websocket_utils.h"

void WebsocketUtils::setup(
  KeyboardUtils& InkUtils,
  MouseUtils& InmUtils,
  uint32_t InDelays, 
  String InSsid, 
  String InPass, 
  bool InSsl, 
  String InServer, 
  uint16_t InPort
  )
  {
  kUtils = &InkUtils;
  mUtils = &InmUtils;
  delays = InDelays;
  ssid_Router = InSsid;
  password_Router = InPass;
  use_ssl = InSsl;
  server = InServer;
  port = InPort;
  
  
  // --------------- wifi ---------------
	Serial.println("Wifi start");
	WiFi.begin(ssid_Router.c_str(), password_Router.c_str());
	Serial.println(String("Connecting to ")+ssid_Router);
	while (WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
	Serial.println("\nWifi Connected, IP address: ");
  Serial.println(WiFi.localIP());
	delay(delays);

  // --------------- websocket ---------------
	Serial.print("Connecting to WebSockets Server @ ");
  Serial.println(server + ":" + port);

  wsClient.onMessage([this](websockets::WebsocketsMessage msg){
    this->onMessageCallback(msg);
  });
  wsClient.onEvent([this](websockets::WebsocketsEvent event, String data){
      this->onEventsCallback(event, data);
  });


	if (use_ssl){
    wsClient.connect(server + ":" + port);
  }else{
    wsClient.connect(server + ":" + port);
  }

	Serial.print("Connected to WebSockets Server @ ");
  Serial.println(server + ":" + port);
}

void WebsocketUtils::loop(){
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

	j_key = "random_mouse";
	if (payload_json.hasOwnProperty(j_key)) {
		bool j_return = (bool) payload_json[j_key];
		Serial.print("json is random mouse:");
		Serial.println(j_return);
		mUtils->random_move = j_return;
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