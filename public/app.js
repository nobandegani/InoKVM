const protocol = location.protocol === "https:" ? "wss://" : "ws://";
const ws = new WebSocket(protocol + location.host);

ws.onopen = () => console.log("✅ Connected to server");
ws.onmessage = (msg) => console.log("📥 Received:", msg.data);

const input = document.getElementById("inputBox");
const checkbox = document.getElementById("randomMouseCheckbox");
const pad = document.getElementById("touchpad");
const sensitivityInput = document.getElementById("sensitivity");

input.addEventListener("keyup", (e) => {
    e.preventDefault();

    if (ws.readyState === WebSocket.OPEN) {
        let keys = [];
        if (e.ctrlKey) keys.push("CONTROL");
        if (e.altKey) keys.push("ALT");
        if (e.shiftKey) keys.push("SHIFT");
        if (e.metaKey) keys.push("META");

        const key = e.key;
        if (!["CONTROL", "ALT", "SHIFT", "META"].includes(key)) {
            keys.push(key);
        }

        if (keys.length > 0) {
            ws.send(JSON.stringify({ keys }));
        }
    }
});

checkbox.addEventListener("change", () => {
    if (ws.readyState === WebSocket.OPEN) {
        ws.send(JSON.stringify({ random_mouse: checkbox.checked }));
    }
});

let lastX = null, lastY = null;
let mouseDown = false;

pad.addEventListener("pointerdown", (e) => {
    lastX = e.clientX;
    lastY = e.clientY;
    mouseDown = true;
});

pad.addEventListener("pointerup", () => {
    mouseDown = false;
    lastX = null;
    lastY = null;
});

pad.addEventListener("pointermove", (e) => {
    if (!mouseDown) return;

    const sensitivity = parseFloat(sensitivityInput.value) || 1;
    const dx = (e.clientX - lastX) * sensitivity;
    const dy = (e.clientY - lastY) * sensitivity;

    lastX = e.clientX;
    lastY = e.clientY;

    if (Math.abs(dx) > 0 || Math.abs(dy) > 0) {
        if (ws.readyState === WebSocket.OPEN) {
            ws.send(JSON.stringify({
                mouse: {
                    dx: Math.round(dx),
                    dy: Math.round(dy)
                }
            }));
        }
    }
});
