const express = require("express");
const http = require("http");
const WebSocket = require("ws");
const path = require("path");
const url = require("url");

const WEB_TOKEN = "web_token";
const BOARD_TOKEN = "board_token";

const app = express();
const server = http.createServer(app);
const wss = new WebSocket.Server({ noServer: true });

// Serve static files
app.use(express.static(path.join(__dirname, "public")));

// Upgrade HTTP to WebSocket with token check
server.on("upgrade", (req, socket, head) => {
    const query = url.parse(req.url, true).query;
    const token = query?.token;

    if (token !== WEB_TOKEN && token !== BOARD_TOKEN) {
        socket.write("HTTP/1.1 401 Unauthorized\r\n\r\n");
        socket.destroy();
        return;
    }

    wss.handleUpgrade(req, socket, head, (ws) => {
        ws.clientType = token === WEB_TOKEN ? "web" : "board";
        wss.emit("connection", ws, req);
    });
});

// WebSocket connection handler
wss.on("connection", (ws) => {
    console.log(`Client connected as ${ws.clientType}`);

    ws.on("message", (raw, isBinary) => {
        if (isBinary) {
            console.log("📸 Binary received, size:", raw.length);

            // Send binary only to clients with type "web"
            wss.clients.forEach((client) => {
                if (
                    client.readyState === WebSocket.OPEN &&
                    client.clientType === "web"
                ) {
                    client.send(raw, { binary: true });
                }
            });
        } else {
            const text = raw.toString();

            let parsed;
            try {
                parsed = JSON.parse(text);
                console.log("📥 JSON:", parsed);

                // Send JSON only to clients with type "board"
                wss.clients.forEach((client) => {
                    if (
                        client.readyState === WebSocket.OPEN &&
                        client.clientType === "board"
                    ) {
                        client.send(text);
                    }
                });
            } catch {
                console.log("❌ Invalid JSON");
            }
        }
    });

    ws.on("close", () => {
        console.log(`Client (${ws.clientType}) disconnected`);
    });
});

server.listen(3658, () => {
    console.log("Server listening at http://localhost:3658");
});