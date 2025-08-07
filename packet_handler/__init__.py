# Finally import the compiled extension
from .packet_handler import *
class SocketPacketInterface:
    def __init__(self, host: str, port: int = 80, recv_buf_size: int = 1024):
        import socket
        import threading
        import queue
        self.host = host
        self.port = port
        self.recv_buf_size = recv_buf_size

        self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.handler = PacketHandler()
        self.dispatcher = PacketDispatcher()
        self.send_queue = queue.Queue()
        self.running = False

        self.recv_thread = threading.Thread(target=self._recv_loop, daemon=True)

    def connect(self):
        self.socket.connect((self.host, self.port))

    def start(self):
        self.running = True
        self.recv_thread.start()

    def stop(self):
        self.running = False
        self.socket.close()
        self.recv_thread.join()

    def send_packet(self, packet):
        """
        Push a packet to be sent. `packet` must be bytes or a type that supports `.serialize()`.
        """
        self.socket.send(bytes(self.handler.create_packet(packet)))

    def _recv_loop(self):
        while self.running:
            try:
                data = self.socket.recv(self.recv_buf_size)
                if not data:
                    print("Remote socket closed.")
                    break
                self.handler.receive_data(list(data))
                status, packet = self.handler.check_packet()
                while status == CheckStatus.EXECUTED_PACKET:
                    self.dispatcher.dispatch_packet(packet)
                    status, packet = self.handler.check_packet()
            except Exception as e:
                print(f"Recv error: {e}")
                break
