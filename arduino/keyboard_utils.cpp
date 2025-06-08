#include "keyboard_utils.h"

void KeyboardUtils::setup(){
  Serial.println("Keyboard start");
	Keyboard.begin();
}

void KeyboardUtils::press(uint8_t key){
  Serial.print("Keyboard press key: ");
  Serial.println((char)key);
  Keyboard.press(key);
}

void KeyboardUtils::releaseAll(uint32_t InDelay){
  Serial.println("Keyboard release all");
  delay(InDelay);
  Keyboard.releaseAll();
}

uint8_t KeyboardUtils::parse_key(String input){ 
  String org_input = input;
	input.toUpperCase();

  if (input == "F1") return KEY_F1;
  if (input == "F2") return KEY_F2;
  if (input == "F3") return KEY_F3;
  if (input == "F4") return KEY_F4;
  if (input == "F5") return KEY_F5;
  if (input == "F6") return KEY_F6;
  if (input == "F7") return KEY_F7;
  if (input == "F8") return KEY_F8;
  if (input == "F9") return KEY_F9;
  if (input == "F10") return KEY_F10;
  if (input == "F11") return KEY_F11;
  if (input == "F12") return KEY_F12;
	if (input == "F14") return KEY_F13;
	if (input == "F14") return KEY_F14;
	if (input == "F15") return KEY_F15;
	if (input == "F16") return KEY_F16;
	if (input == "F17") return KEY_F17;
	if (input == "F18") return KEY_F18;
	if (input == "F19") return KEY_F19;
	if (input == "F20") return KEY_F20;
	if (input == "F21") return KEY_F21;
	if (input == "F22") return KEY_F22;
	if (input == "F23") return KEY_F23;
	if (input == "F24") return KEY_F24;

  if (input == "ARROWUP") return KEY_UP_ARROW;
  if (input == "ARROWDOWN") return KEY_DOWN_ARROW;
  if (input == "ARROWLEFT") return KEY_LEFT_ARROW;
  if (input == "ARROWRIGHT") return KEY_RIGHT_ARROW;

  if (input == "TAB") return KEY_TAB;
  if (input == "ESCAPE") return KEY_ESC;
  if (input == "ENTER") return KEY_RETURN;
  if (input == "BACKSPACE") return KEY_BACKSPACE;
  if (input == "DELETE") return KEY_DELETE;
  if (input == "INSERT") return KEY_INSERT;
  if (input == "HOME") return KEY_HOME;
  if (input == "END") return KEY_END;
  if (input == "PAGEUP") return KEY_PAGE_UP;
  if (input == "PAGEDOWN") return KEY_PAGE_DOWN;

  if (input == "SPACE" || input == " ") return KEY_SPACE;
  if (input == "CONTROL") return KEY_LEFT_CTRL;
  //if (input == "RIGHTCTRL") return KEY_RIGHT_CTRL;
  if (input == "ALT") return KEY_LEFT_ALT;
  //if (input == "RIGHTALT") return KEY_RIGHT_ALT;
  if (input == "SHIFT") return KEY_LEFT_SHIFT;
  //if (input == "RIGHTSHIFT") return KEY_RIGHT_SHIFT;

  if (input == "KP1") return KEY_KP_1;
  if (input == "KP2") return KEY_KP_2;
  if (input == "KP3") return KEY_KP_3;
  if (input == "KP4") return KEY_KP_4;
  if (input == "KP5") return KEY_KP_5;
  if (input == "KP6") return KEY_KP_6;
  if (input == "KP7") return KEY_KP_7;
  if (input == "KP8") return KEY_KP_8;
  if (input == "KP9") return KEY_KP_9;
  if (input == "KP0") return KEY_KP_0;
  if (input == "KPENTER") return KEY_KP_ENTER;

  if (org_input.length() == 1) return org_input.charAt(0);
	
	return 0;
}