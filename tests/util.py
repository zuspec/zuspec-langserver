
import asyncio
import os
import re
import debug_mgr.core as dmgr
import libjson_rpc.core as jrpc
import lls.core as lls
from pygls.lsp.client import BaseLanguageClient

def create_server(port):
    tests_dir = os.path.dirname(os.path.abspath(__file__))

class SocketClient(BaseLanguageClient):

    def __init__(self):
        super().__init__("abc", "1.0")

        self.server = None
        self.port = -1
        self.connected = False
        self.connected_ev = asyncio.Event()
        self.writer = None
        self.reader = None


    def start_io(self):
        self.server = asyncio.get_event_loop().run_until_complete(
            asyncio.start_server(self.handle_connection, '127.0.0.1', 0))
        self.port = self.server.sockets[0].getsockname()[1]

    async def close(self):
        if self.writer is not None:
            self.writer.close()
#        if self.reader is not None:
#            self.reader.close()

    async def wait_connection(self):
        while not self.connected:
            await self.connected_ev.wait()
    
    def handle_connection(self, reader, writer):
        print("handle_connection")
        self.connected = True
        self.connected_ev.set()

        self.writer = writer
        self.reader = reader
        self.protocol.connection_made(writer)

        self._stop_event = asyncio.Event()
        asyncio.create_task(
            self._readline(
                self._stop_event, 
                reader, 
                self.protocol.data_received))
        
    def write(self, data):
        print("Write: \"%s\"" % str(data), flush=True)
        self.writer.write(data)

    async def _readline(self, stop_event, reader, msg_handler):
        CONTENT_LENGTH_PATTERN = re.compile(rb"^Content-Length: (\d+)\r\n$")

        # Initialize message buffer
        message = []
        content_length = 0

        while not stop_event.is_set():
            # Read a header line
            header = await reader.readline()
            if not header:
                break
            message.append(header)

            # Extract content length if possible
            if not content_length:
                match = CONTENT_LENGTH_PATTERN.fullmatch(header)
                if match:
                    content_length = int(match.group(1))
#                    logger.debug("Content length: %s", content_length)

            # Check if all headers have been read (as indicated by an empty line \r\n)
            if content_length and not header.strip():
                # Read body
                body = await reader.readexactly(content_length)
                if not body:
                    break
                message.append(body)

                # Pass message to protocol
                msg_handler(b"".join(message))

                # Reset the buffer
                message = []
                content_length = 0

