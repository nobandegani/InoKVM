#include <Arduino.h>
#include "USB.h"

#include "websocket_utils.h"
#include "keyboard_utils.h"
#include "mouse_utils.h"
#include "camera_utils.h"

const uint32_t interval = 1000;
const uint32_t delays = 1000;

WebsocketUtils wsUtils;
KeyboardUtils kUtils;
MouseUtils mUtils;
CameraUtils cUtils;

void setup() {
	// --------------- serial ---------------
  Serial.begin(115200);
	Serial.println("Serial start");
  delay(delays);

	// --------------- keyboard utils ---------------
	kUtils.setup();
	delay(delays);

	// --------------- mouse utils ---------------
	mUtils.setup();
	delay(delays);

	// --------------- camera utils ---------------
	cUtils.setup();
	delay(delays);

	// --------------- usb ---------------
	Serial.println("USB start");
	USB.begin();
  delay(delays);

	// --------------- websocket utils ---------------
	wsUtils.setConf(1000, 0);
	wsUtils.setRef(kUtils, mUtils, cUtils);
	wsUtils.setWifi("Inoland2", "wGfzTV053GM4V");
	delay(delays);
	//wsUtils.setWebsocket(true, "kvm.likenesslabs.com", 443);
	wsUtils.setWebsocket(false, "66.94.114.177", 80, "board_token");
	delay(delays);
}

void loop() { 
	wsUtils.loop();
	mUtils.loop();
}


