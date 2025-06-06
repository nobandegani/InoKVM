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

    ws.on("message", (raw) => {
        console.log("Raw:", raw);
        const data = raw.toString();
        console.log("Data:", data);

        // 🔍 Check if it's valid JSON
        let isJSON = true;
        let parsed;
        try {
            parsed = JSON.parse(data);
        } catch (err) {
            isJSON = false;
        }

        if (isJSON) {
            console.log("✅ Valid JSON");
            // You can access parsed.name, parsed.key, etc.
        } else {
            console.log("❌ Not a valid JSON");
        }

        // 🔁 Broadcast to other clients
        wss.clients.forEach((client) => {
            if (client !== ws && client.readyState === WebSocket.OPEN) {
                if (isJSON) {
                    client.send(data);
                }else{
                    client.send(raw);
                }

            }
        });
    });

    ws.on("close", () => {
        console.log("Client disconnected");
    });
});

server.listen(3658, () => {
    console.log("Server listening at http://localhost:3658");
});
