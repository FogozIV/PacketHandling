# Finally import the compiled extension
from ._packet_handler import *
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
    
    def is_connected(self):
        """Check if the client is connected to the server."""
        try:
            # Try to get socket info - if it fails, we're not connected
            self.socket.getpeername()
            return True
        except:
            return False

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
        try:
            self.socket.send(bytes(self.handler.create_packet(packet)))
        except Exception as e:
            print(f"Send error: {e}")
            raise

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


class ClientConnection:
    def __init__(self, socket, client_id, address, recv_buf_size, shared_handler=None, shared_dispatcher=None, server=None):
        self.socket = socket
        self.client_id = client_id
        self.address = address
        self.recv_buf_size = recv_buf_size
        self.server = server  # Reference to server for cleanup
        import threading
        
        # Use shared handlers if provided, otherwise create new ones
        if shared_handler is not None:
            self.handler = shared_handler
        else:
            self.handler = PacketHandler()
            
        if shared_dispatcher is not None:
            self.dispatcher = shared_dispatcher
        else:
            self.dispatcher = PacketDispatcher()
        
        self.running = True
        self.recv_thread = threading.Thread(target=self._recv_loop, daemon=True)
        self.recv_thread.start()
    
    def _recv_loop(self):
        while self.running:
            try:
                data = self.socket.recv(self.recv_buf_size)
                if not data:
                    print(f"Client {self.client_id} ({self.address}) disconnected.")
                    break
                self.handler.receive_data(list(data))
                status, packet = self.handler.check_packet()
                while status == CheckStatus.EXECUTED_PACKET:
                    self.dispatcher.dispatch_packet(packet)
                    status, packet = self.handler.check_packet()
            except Exception as e:
                # Handle specific connection errors more gracefully
                if "10053" in str(e) or "connection" in str(e).lower():
                    print(f"Client {self.client_id} ({self.address}) connection lost")
                else:
                    print(f"Recv error for client {self.client_id}: {e}")
                break
        
        self.running = False
        # Notify server to remove this client (but don't call close from within the thread)
        if self.server:
            self.server._remove_client(self.client_id, skip_thread_join=True)
    
    def send_packet(self, packet):
        """Send a packet to this client."""
        self.socket.send(bytes(self.handler.create_packet(packet)))
    
    def close(self):
        """Close the client connection."""
        self.running = False
        try:
            self.socket.close()
        except:
            pass  # Socket might already be closed
        
        # Only join if we're not in the recv thread
        if threading.current_thread() != self.recv_thread:
            self.recv_thread.join()


class SocketPacketServer:
    def __init__(self, host: str, port: int = 80, recv_buf_size: int = 1024, 
                 shared_handlers: bool = False, on_client_disconnect=None):
        import socket
        import threading
        
        self.host = host
        self.port = port
        self.recv_buf_size = recv_buf_size
        self.shared_handlers = shared_handlers
        self.on_client_disconnect = on_client_disconnect
        
        self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        
        # Shared handlers for all clients if enabled
        if shared_handlers:
            self.shared_handler = PacketHandler()
            self.shared_dispatcher = PacketDispatcher()
        else:
            self.shared_handler = None
            self.shared_dispatcher = None
        
        self.clients = {}  # {client_id: ClientConnection}
        self.next_client_id = 1
        self.running = False
        self.accept_thread = threading.Thread(target=self._accept_loop, daemon=True)
    
    def start(self):
        """Start the server and begin accepting connections."""
        self.socket.bind((self.host, self.port))
        self.socket.listen()
        self.running = True
        self.accept_thread.start()
        print(f"Server started on {self.host}:{self.port}")
    
    def stop(self):
        """Stop the server and close all client connections."""
        self.running = False
        self.socket.close()
        
        # Close all client connections (normal cleanup with thread join)
        for client in list(self.clients.values()):
            client.close()
        
        self.accept_thread.join()
        print("Server stopped")
    
    def send_packet(self, packet, client_id=None):
        """Send a packet to a specific client."""
        if client_id is None:
            raise ValueError("client_id must be provided for send_packet")
        
        if client_id not in self.clients:
            raise ValueError(f"Client {client_id} not found")
        
        self.clients[client_id].send_packet(packet)
    
    def broadcast_packet(self, packet):
        """Send a packet to all connected clients."""
        for client in self.clients.values():
            try:
                client.send_packet(packet)
            except Exception as e:
                print(f"Failed to send packet to client {client.client_id}: {e}")
    
    def _accept_loop(self):
        """Accept new client connections."""
        while self.running:
            try:
                client_socket, address = self.socket.accept()
                client_id = self.next_client_id
                self.next_client_id += 1
                
                print(f"New client connected: {client_id} from {address}")
                
                client_conn = ClientConnection(
                    client_socket, client_id, address, self.recv_buf_size,
                    self.shared_handler, self.shared_dispatcher, self
                )
                
                self.clients[client_id] = client_conn
                
            except Exception as e:
                if self.running:  # Only print error if we're still supposed to be running
                    print(f"Accept error: {e}")
                break
    
    def _remove_client(self, client_id, skip_thread_join=False):
        """Remove a client from tracking and cleanup."""
        if client_id in self.clients:
            client = self.clients[client_id]
            print(f"Removing client {client_id} ({client.address})")
            
            # Call disconnect callback if provided
            if self.on_client_disconnect:
                try:
                    self.on_client_disconnect(client_id, client.address)
                except Exception as e:
                    print(f"Error in disconnect callback: {e}")
            
            if skip_thread_join:
                # Just close the socket, don't join the thread (it's calling us)
                client.running = False
                client.socket.close()
            else:
                # Normal cleanup - join the thread
                client.close()
            
            del self.clients[client_id]
    
    def get_client_count(self):
        """Get the number of connected clients."""
        return len(self.clients)
    
    def get_client_ids(self):
        """Get a list of all connected client IDs."""
        return list(self.clients.keys())
    
    def is_client_connected(self, client_id):
        """Check if a specific client is connected."""
        return client_id in self.clients