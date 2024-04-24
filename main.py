from signal_generator import SignalGenerator
import asyncio
from socket_maker import Socket_Maker, WebSocketClient
import websockets
import numpy as np
import argparse
import json
import matplotlib.pyplot as plt


async def main(socket_server_1, socket_client_1):
    # Запуск сервера
    server_task = asyncio.create_task(socket_server_1.start())

    # Даём серверу пару секунд на запуск перед соединением клиента
    await asyncio.sleep(2)

    # Запуск клиента
    client_task = asyncio.create_task(socket_client_1.listen())

    # Ждём выполнения обеих задач (они могут заниматься своим делом и не завершаться)
    await asyncio.gather(server_task, client_task)


if __name__ == '__main__':
    signals = []
    signal_generator = SignalGenerator()
    signal = signal_generator.generate()
    signals.append(signal)
    signal_generator.signal_type = "square"
    signal = signal_generator.generate()
    signals.append(signal)

    host = 'localhost'
    port = 8080
    socket_server = Socket_Maker(host, port)
    socket_server.update_signals(signals)
    socket_client = WebSocketClient(f"ws://{host}:{port}")
    asyncio.run(main(socket_server, socket_client))



# See PyCharm help at https://www.jetbrains.com/help/pycharm/
