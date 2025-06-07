let ws = null;

// --- Connect Button Logic ---
document.getElementById("connectBtn").addEventListener("click", () => {
    const token = document.getElementById("tokenInput").value.trim();
    if (!token) {
        alert("❗ Please enter a token");
        return;
    }

    const protocol = location.protocol === "https:" ? "wss://" : "ws://";
    const wsUrl = `${protocol}${location.host}?token=${encodeURIComponent(token)}`;
    ws = new WebSocket(wsUrl);
    ws.binaryType = "arraybuffer";

    ws.onopen = () => {
        console.log("✅ Connected to server");
        document.getElementById("connectBtn").disabled = true;
    };

    ws.onmessage = (msg) => {
        if (typeof msg.data === "string") {
            console.log("📥 JSON:", msg.data);
        } else if (msg.data instanceof ArrayBuffer) {
            const blob = new Blob([msg.data], { type: "image/jpeg" }); // or image/png
            const url = URL.createObjectURL(blob);
            const img = document.getElementById("cameraImage");
            img.src = url;
            img.onload = () => URL.revokeObjectURL(url);
        }
    };

    ws.onclose = () => {
        console.log("🔌 Disconnected from server");
        document.getElementById("connectBtn").disabled = false;
    };

    ws.onerror = (err) => {
        console.error("❌ WebSocket error:", err);
    };
});

// --- Controls ---
const input = document.getElementById("inputBox");
const CameraFeedCheckbox = document.getElementById("CameraFeedCheckbox");
const randomMouseCheckbox = document.getElementById("randomMouseCheckbox");
const pad = document.getElementById("touchpad");
const sensitivityInput = document.getElementById("sensitivity");

input.addEventListener("keyup", (e) => {
    e.preventDefault();
    if (ws && ws.readyState === WebSocket.OPEN) {
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

CameraFeedCheckbox.addEventListener("change", () => {
    if (ws && ws.readyState === WebSocket.OPEN) {
        ws.send(JSON.stringify({ camera_active: CameraFeedCheckbox.checked }));
    }
});

randomMouseCheckbox.addEventListener("change", () => {
    if (ws && ws.readyState === WebSocket.OPEN) {
        ws.send(JSON.stringify({ random_mouse: randomMouseCheckbox.checked }));
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
    if (!mouseDown || !ws || ws.readyState !== WebSocket.OPEN) return;

    const sensitivity = parseFloat(sensitivityInput.value) || 1;
    const dx = (e.clientX - lastX) * sensitivity;
    const dy = (e.clientY - lastY) * sensitivity;

    lastX = e.clientX;
    lastY = e.clientY;

    if (Math.abs(dx) > 0 || Math.abs(dy) > 0) {
        ws.send(JSON.stringify({
            mouse: {
                dx: Math.round(dx),
                dy: Math.round(dy)
            }
        }));
    }
});
