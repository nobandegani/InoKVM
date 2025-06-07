const express = require("express");
const http = require("http");
const WebSocket = require("ws");
const path = require("path");

const app = express();
const server = http.createServer(app);
const wss = new WebSocket.Server({ server });

// Serve static files from the "public" folder
app.use(express.static(path.join(__dirname, "public")));

wss.on("connection", (ws) => {
    console.log("Client connected");

    ws.on("message", (raw, isBinary) => {
        if (isBinary) {
            console.log("📸 Received binary data, size:", raw.length);
            // Forward binary frame to all other clients
            wss.clients.forEach((client) => {
                if (client !== ws && client.readyState === WebSocket.OPEN) {
                    client.send(raw, { binary: true });
                }
            });
            return;
        }

        const data = raw.toString();
        console.log("📥 Data:", data);

        let parsed;
        try {
            parsed = JSON.parse(data);
            console.log("✅ Valid JSON:", parsed);

            // Forward JSON to others
            wss.clients.forEach((client) => {
                if (client !== ws && client.readyState === WebSocket.OPEN) {
                    client.send(data);
                }
            });
        } catch (err) {
            console.log("❌ Invalid JSON");
        }
    });

    ws.on("close", () => {
        console.log("Client disconnected");
    });
});

server.listen(3658, () => {
    console.log("Server listening at http://localhost:3658");
});
