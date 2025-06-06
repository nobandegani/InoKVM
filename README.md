# InoKVM

A simple yet powerful hardware-software combo that turns an ESP32-S3 board into a KVM-like (Keyboard, Video, Mouse) device.

- 🧠 **Board**: ESP32-S3-WROOM
- ⌨️ Acts as a USB HID Keyboard & Mouse
- 📷 Streams onboard camera feed to browser
- 🌐 Connects to Node.js WebSocket server for commands
- 🖱️ Web UI for sending key/mouse inputs & viewing stream

---
## 🚀 Getting Started

### 1. Flash ESP32-S3

Make sure you have the following:

- **Board**: ESP32-S3 WROOM (or similar ESP32 with native USB)
- **PlatformIO / Arduino IDE** with correct board selected
- In `kvm.ino`, edit the `wsUtils.setup(...)` line:

```cpp
wsUtils.setup(
  kUtils, mUtils,
  1000,               // Delay in ms
  "YOUR_WIFI_SSID",
  "YOUR_WIFI_PASSWORD",
  false,              // Use SSL (true/false)
  "ws://YOUR_PC_IP",  // WebSocket server address
  80                  // WebSocket port
);
```

###  2. Run Node.js Server
Install dependencies and start the server:
```Java
npm install
node server.js
```
Access the Web UI at:
👉 http://localhost:3658

You’ll see:

- An input box for sending keys

- A checkbox for enabling random mouse movement

- A touchpad area (if added) for dragging the mouse

- Camera stream (if supported by your ESP32)


##  📌 Notes

- The keyboard_utils and mouse_utils classes are plug-and-play and require no changes.

- The kvm.ino file is tailored for ESP32-S3 with native USB support but should work with other ESP32 variants with modifications.

- If you want to change Wi-Fi or server behavior, just edit the wsUtils.setup(...) call.

## 📄 License

- This project is licensed under the Mozilla Public License 2.0.
- You can freely use, modify, and distribute this code under the terms of the MPL-2.0 license.