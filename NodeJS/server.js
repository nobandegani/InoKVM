const WebSocket = require('ws');
const url       = require('url');

const STREAMER_TOKEN = 'STREAMER_TOKEN_HERE';
const CLIENT_TOKEN   = 'CLIENT_TOKEN_HERE';

let streamer = null;
const clients = new Set();

const wss = new WebSocket.Server({ port: 8765 }, () => {
    console.log('WebSocket server listening on ws://0.0.0.0:8765');
});

wss.on('connection', (ws, req) => {
    const { query } = url.parse(req.url, true);
    const token     = query.token;

    if (token === STREAMER_TOKEN) {
        if (streamer) {
            ws.close(1008, 'Streamer already connected');
            return;
        }
        ws.role   = 'streamer';
        streamer  = ws;
        console.log('A streamer connected');
    }
    else if (token === CLIENT_TOKEN) {
        ws.role = 'client';
        clients.add(ws);
        console.log('A client connected (total:', clients.size, ')');
    }
    else {
        ws.close(1008, 'Invalid token');
        return;
    }

    ws.on('close', () => {
        if (ws.role === 'streamer') {
            console.log('Streamer disconnected');
            streamer = null;
        } else {
            clients.delete(ws);
            console.log('A client disconnected (remaining:', clients.size, ')');
        }
    });

    ws.on('message', (message) => {
        if (ws.role === 'streamer') {
            // forward to all clients
            for (const client of clients) {
                if (client.readyState === WebSocket.OPEN) {
                    client.send(message);
                }
            }
        } else {
            console.log('Received message from client:', message);
        }
    });

    ws.on('error', (err) => {
        console.error('WebSocket error', err);
    });
});
