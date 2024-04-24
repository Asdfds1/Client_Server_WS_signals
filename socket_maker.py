import asyncio
import websockets
import json


class Socket_Maker:
    def __init__(self, host, port):
        self.host = host
        self.port = port
        self.signal = None

    async def send_signals(self, websocket, signals):
        while True:
            # Предполагается, что self.signals уже сгенерирован и готов к отправке
            await websocket.send(json.dumps(self.signal))
            await asyncio.sleep(1)

    def update_signals(self, new_signal):
        # Метод для обновления списка сигналов
        self.signal = new_signal

    async def start(self):
        async with websockets.serve(self.send_signals, self.host, self.port):
             await asyncio.Future()


class WebSocketClient:
    def __init__(self, uri):
        self.uri = uri

    # Метод для подключения к серверу и прослушивания входящих сообщений
    async def listen(self):
        async with websockets.connect(self.uri) as websocket:
            while True:
                message = await websocket.recv()
                print(f"Received message: {message}")