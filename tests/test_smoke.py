
import asyncio
import logging
import os
import lsprotocol.types
import pytest
import socket
import pygls
import shutil
import sys
import traceback
import zsp_ls.core as zsp_ls
import debug_mgr.core as dm
import lsprotocol
from .util import SocketClient

import debug_mgr.core as dmgr
dmgr.Factory.inst().getDebugMgr().enable(True)


@pytest.fixture
def langserver():
    val = 5
    print("pre-test", flush=True)
    dmgr.Factory.inst().getDebugMgr().enable(True)

    zsp_ls_f = zsp_ls.Factory.inst()

    loop = asyncio.new_event_loop()
    asyncio.set_event_loop(loop)

    client = SocketClient()
    client.start_io()

    thread_main = zsp_ls_f.mkThreadMain(client.port)

    dm.Factory.inst().getDebugMgr().enable(True)
    thread_main.start()

    loop.run_until_complete(client.wait_connection())

    yield (client, thread_main)

    print("client: %s ; writer: %s" % (str(client), str(client.writer)), flush=True)
    loop.run_until_complete(client.close())
    
    thread_main.join()

    print("post-test %s" % val)

@pytest.fixture
def rundir(request):
    root_rundir = os.path.join(os.getcwd(), "rundir")

    test_rundir = os.path.join(root_rundir, request.node.name)

    if os.path.isdir(test_rundir):
        shutil.rmtree(test_rundir)
    os.makedirs(test_rundir)

    yield test_rundir

    print("clean-up %s" % test_rundir)

def copy_files(test_rundir, files):
    for name,content in files.items():
        fullpath = os.path.join(test_rundir, name)
        fulldir = os.path.dirname(fullpath)
        
        if not os.path.isdir(fulldir):
            os.makedirs(fulldir)
        
        with open(fullpath, "w") as fp:
            fp.write(content)

def test_smoke(langserver, rundir):
    print("langserver: %s" % str(langserver))

    logging.basicConfig(level=logging.DEBUG)
    logger = logging.getLogger("my_logger")
    logger.debug("Hello there")

    copy_files(rundir, {
        "comp1.pss" : """
component comp1 {
    int i;
}
        """,
        "comp2.pss" : """
component comp2 {
    int x;
}
"""
    })

    client : pygls.client.BaseLanguageClient = langserver[0]

    loop = asyncio.get_event_loop()

    client_capabilities = lsprotocol.types.ClientCapabilities()
    params = lsprotocol.types.InitializeParams(
        capabilities=client_capabilities,
        root_uri=rundir,
        root_path=rundir,
        )
    result = loop.run_until_complete(client.initialize_async(params))
    print("result: %s" % str(result))

    print("--> SendInitialized", flush=True)
    params = lsprotocol.types.InitializedParams()
    client.initialized(params)
    print("<-- SendInitialized", flush=True)

    print("--> SendDidOpenTextDocument", flush=True)
    with open(os.path.join(rundir, "comp1.pss"), "r") as fp:
        content = fp.read()
    params = lsprotocol.types.DidOpenTextDocumentParams(
        lsprotocol.types.TextDocumentItem(
            "file://" + os.path.join(rundir, "comp1.pss"),
            "pss",
            0,
            content
        )
    )
    client.text_document_did_open(params)
    print("<-- SendDidOpenTextDocument", flush=True)

    print("--> DocumentSymbolAsync", flush=True)
    params = lsprotocol.types.DocumentSymbolParams(
        lsprotocol.types.TextDocumentIdentifier(
            "file://" + os.path.join(rundir, "comp1.pss")))

    result = loop.run_until_complete(client.text_document_document_symbol_async(params))
    assert len(result) != 0

    print("<-- DocumentSymbolAsync", flush=True)

    print("result: %s" % str(result))

def test_syntax_error(langserver, rundir):
    print("langserver: %s" % str(langserver))

    logging.basicConfig(level=logging.DEBUG)
    logger = logging.getLogger("my_logger")
    logger.debug("Hello there")

    copy_files(rundir, {
        "comp1.pss" : """
component comp1 {
    int i;
    action 
}
        """,
        "comp2.pss" : """
component comp2 {
    int x;
}
"""
    })

    client : pygls.client.BaseLanguageClient = langserver[0]

    loop = asyncio.get_event_loop()

    client_capabilities = lsprotocol.types.ClientCapabilities()
    params = lsprotocol.types.InitializeParams(
        capabilities=client_capabilities,
        root_uri=rundir,
        root_path=rundir,
        )
    result = loop.run_until_complete(client.initialize_async(params))
    print("result: %s" % str(result))

    print("--> SendInitialized", flush=True)
    params = lsprotocol.types.InitializedParams()
    client.initialized(params)
    print("<-- SendInitialized", flush=True)

    print("--> SendDidOpenTextDocument", flush=True)
    with open(os.path.join(rundir, "comp1.pss"), "r") as fp:
        content = fp.read()
    params = lsprotocol.types.DidOpenTextDocumentParams(
        lsprotocol.types.TextDocumentItem(
            "file://" + os.path.join(rundir, "comp1.pss"),
            "pss",
            0,
            content
        )
    )
    client.text_document_did_open(params)
    print("<-- SendDidOpenTextDocument", flush=True)

    print("--> DocumentSymbolAsync", flush=True)
    params = lsprotocol.types.DocumentSymbolParams(
        lsprotocol.types.TextDocumentIdentifier(
            "file://" + os.path.join(rundir, "comp1.pss")))

    result = loop.run_until_complete(client.text_document_document_symbol_async(params))
#    assert len(result) != 0


    print("<-- DocumentSymbolAsync", flush=True)

    print("result: %s" % str(result))
#    raise Exception("Marker")

#    data = sock.recv(1024)

def test_change(langserver, rundir):
    print("langserver: %s" % str(langserver))

    logging.basicConfig(level=logging.DEBUG)
    logger = logging.getLogger("my_logger")
    logger.debug("Hello there")

    copy_files(rundir, {
        "comp1.pss" : """
component comp1 {
    int i;
}
        """,
        "comp2.pss" : """
component comp2 {
    int x;
}
"""
    })

    client : pygls.client.BaseLanguageClient = langserver[0]

    loop = asyncio.get_event_loop()

    client_capabilities = lsprotocol.types.ClientCapabilities()
    params = lsprotocol.types.InitializeParams(
        capabilities=client_capabilities,
        root_uri=rundir,
        root_path=rundir,
        )
    result = loop.run_until_complete(client.initialize_async(params))
    print("result: %s" % str(result))

    print("--> SendInitialized", flush=True)
    params = lsprotocol.types.InitializedParams()
    client.initialized(params)
    print("<-- SendInitialized", flush=True)

    print("--> SendDidOpenTextDocument", flush=True)
    with open(os.path.join(rundir, "comp1.pss"), "r") as fp:
        content = fp.read()
    params = lsprotocol.types.DidOpenTextDocumentParams(
        lsprotocol.types.TextDocumentItem(
            "file://" + os.path.join(rundir, "comp1.pss"),
            "pss",
            0,
            content
        )
    )
    client.text_document_did_open(params)
    print("<-- SendDidOpenTextDocument", flush=True)

    print("--> DocumentSymbolAsync", flush=True)
    params = lsprotocol.types.DocumentSymbolParams(
        lsprotocol.types.TextDocumentIdentifier(
            "file://" + os.path.join(rundir, "comp1.pss")))

    result = loop.run_until_complete(client.text_document_document_symbol_async(params))
    assert len(result) != 0
    print("<-- DocumentSymbolAsync", flush=True)

    content += "\nstruct S { }\n"
    params = lsprotocol.types.DidChangeTextDocumentParams(
        lsprotocol.types.VersionedTextDocumentIdentifier(
            1,
            "file://" + os.path.join(rundir, "comp1.pss")),
        content_changes=[lsprotocol.types.TextDocumentContentChangeEvent_Type2(
            text=content
        )]
    )
    result = client.text_document_did_change(params)
    print("didChange: %s" % str(result))

    print("--> DocumentSymbolAsync", flush=True)
    params = lsprotocol.types.DocumentSymbolParams(
        lsprotocol.types.TextDocumentIdentifier(
            "file://" + os.path.join(rundir, "comp1.pss")))

    result = loop.run_until_complete(client.text_document_document_symbol_async(params))
    assert len(result) != 0
    print("<-- DocumentSymbolAsync", flush=True)

    print("result: %s" % str(result))
#    raise Exception("Marker")

#    data = sock.recv(1024)