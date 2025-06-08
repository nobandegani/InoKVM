import asyncio
import websockets
import time
import os

# === CONFIG ===
WS_URL = "ws://66.94.114.177:80/?token=board_token"
IMAGE_PATH = r"C:\Users\noban\Pictures\Screenshots\Screenshot 2025-06-08 193537.png"

DUMMY_SIZE = 35 * 1024  # 35 KB

async def send_data(dummy):
    if (dummy):
        data = os.urandom(DUMMY_SIZE)
    else:
        with open(IMAGE_PATH, "rb") as f:
            data = f.read()


    async with websockets.connect(WS_URL) as ws:
        print(f"Connected to {WS_URL}")
        count = 1

        while True:
            start = time.time()
            await ws.send(data)
            end = time.time()

            elapsed_ms = (end - start) * 1000
            print(f"[{count}] Sent {DUMMY_SIZE / 1024:.1f} KB in {elapsed_ms:.2f} ms")

            count += 1

asyncio.run(send_data(False))
