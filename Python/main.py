import cv2
import asyncio
import websockets

is_dev = True

STREAMER_TOKEN = "STREAMER_TOKEN_HERE"

if is_dev:
    SERVER_URL = "127.0.0.1"
    SERVER_PORT = 8765
    WS_PREFIX = "ws"
else:
    SERVER_URL = "https://kvm.likenesslabs.dev"
    IS_SSL = False
    WS_PREFIX = "wss" if IS_SSL else "ws"
    SERVER_PORT = 443 if IS_SSL else 80

WS_URI = f"{WS_PREFIX}://{SERVER_URL}:{SERVER_PORT}/?token={STREAMER_TOKEN}"

Capture_Device_Name = "/dev/video0"
FPS    = 15
Res_Width = 1920
Res_Height = 1080
QUALITY = 80

async def stream_video():
    # connect (allow unlimited message size)
    async with websockets.connect(WS_URI, max_size=None) as ws:
        print(f'Connected to {WS_URI}')
        # open capture (device 0)
        cap = cv2.VideoCapture(Capture_Device_Name)
        cap.set(cv2.CAP_PROP_FRAME_WIDTH,  Res_Width)
        cap.set(cv2.CAP_PROP_FRAME_HEIGHT, Res_Height)
        cap.set(cv2.CAP_PROP_FPS,           FPS)

        try:
            while True:
                ret, frame = cap.read()
                if not ret:
                    print('Frame grab failed, exiting')
                    break

                # JPEG encode
                success, jpg = cv2.imencode('.jpg', frame, [
                    int(cv2.IMWRITE_JPEG_QUALITY), QUALITY
                ])
                if not success:
                    continue

                # send binary blob
                await ws.send(jpg.tobytes())

                # throttle to target FPS
                await asyncio.sleep(1 / FPS)

        finally:
            cap.release()

if __name__ == '__main__':
    asyncio.run(stream_video())
