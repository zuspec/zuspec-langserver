
import asyncio
import json
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
        try:
            self.connected = True
            self.connected_ev.set()

            self.writer = writer
            self.reader = reader
            self.protocol.set_writer(writer)
        except Exception as e:
            print("Exception1: %s" % str(e), flush=True)

        try:
            self._stop_event = asyncio.Event()
            tasks = set()
            readline_t = asyncio.create_task(
                self._readline(
                    self._stop_event, 
                    reader, 
                    self.protocol.handle_message))
            tasks.add(readline_t)
            readline_t.add_done_callback(tasks.discard)
        except Exception as e:
            print("Exception2: %s" % str(e), flush=True)
        
    def write(self, data):
        print("Write: \"%s\"" % str(data), flush=True)
        self.writer.write(data)

    async def _readline(self, stop_event, reader, msg_handler):
        CONTENT_LENGTH_PATTERN = re.compile(rb"^Content-Length: (\d+)\r\n$")

        # Initialize message buffer
        message = []
        content_length = 0

        print("_readline is_set=%s" % stop_event.is_set(), flush=True)

        while not stop_event.is_set():
            # Read a header line
            header = await reader.readline()
            if not header:
                break
            message.append(header)

            # Extract content length if possible
            if not content_length:
                print("check header %s" % header, flush=True)
                match = CONTENT_LENGTH_PATTERN.fullmatch(header)
                if match:
                    content_length = int(match.group(1))
#                    logger.debug("Content length: %s", content_length)

            # Check if all headers have been read (as indicated by an empty line \r\n)
            if content_length and not header.strip():
                try:
                    # Read body
                    body = await reader.readexactly(content_length)
                    if not body:
                        break
                    message.append(body)

                    message_s = b"".join(message).decode()

                    # Pass message to protocol
                    print("--> msg_handler %s" % message_s, flush=True)

                    # Need to trim the header
                    lcb_idx = message_s.find('{')
                    if lcb_idx != -1:
                        message_s = message_s[lcb_idx:]
                    print("trimmed message: %s" % message_s, flush=True)
                    msg_j = json.loads(message_s)

                    msg = self.protocol.structure_message(msg_j)

                    self.protocol.handle_message(msg)

                    print("<--> msg_handler %s" % message, flush=True)
                except Exception as e:
                    print("Exception: %s" % str(e), flush=True)

                # Reset the buffer
                message = []
                content_length = 0

