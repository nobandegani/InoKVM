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

unsigned long lastPrint = 0;
unsigned int memStatInterval = 5000;

void loop() { 
	wsUtils.loop();
	mUtils.loop();

	unsigned long now = millis();
    if (now - lastPrint >= memStatInterval) {
        printMemoryUsage();
        lastPrint = now;
    }
}

void printMemoryUsage() {
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

