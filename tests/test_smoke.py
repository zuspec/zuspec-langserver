import pytest
import socket
import sys
import traceback
import zsp_ls.core as zsp_ls
import debug_mgr.core as dm

@pytest.fixture
def langserver():
    val = 5
    print("pre-test")

    zsp_ls_f = zsp_ls.Factory.inst()

    serversocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    serversocket.bind(('127.0.0.1', 0))
    port = serversocket.getsockname()[1]
    serversocket.listen(1)

    thread_main = zsp_ls_f.mkThreadMain(port)

    dm.Factory.inst().getDebugMgr().enable(True)

    thread_main.start()

    sock, addr = serversocket.accept()
    sock.setsockopt(socket.IPPROTO_TCP, socket.TCP_NODELAY, 1)
    print("sock: %s" % str(sock))

    yield (sock, thread_main)

    print("post-test %s" % val)

def test_smoke(langserver):
    print("langserver: %s" % str(langserver))

    sock = langserver[0]
    for _ in range(1000):
        sock.send("Hello World\n\n".encode())
    
    raise Exception("Marker")

#    data = sock.recv(1024)