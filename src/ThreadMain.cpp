/*
 * ThreadMain.cpp
 *
 * Copyright 2023 Matthew Ballance and Contributors
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may 
 * not use this file except in compliance with the License.  
 * You may obtain a copy of the License at:
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software 
 * distributed under the License is distributed on an "AS IS" BASIS, 
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  
 * See the License for the specific language governing permissions and 
 * limitations under the License.
 *
 * Created on:
 *     Author:
 */
#include "dmgr/impl/DebugMacros.h"
#include "Server.h"
#include "ThreadMain.h"


namespace zsp {
namespace ls {


ThreadMain::ThreadMain(
        dmgr::IDebugMgr         *dmgr,
        jrpc::IFactory          *jrpc_f,
        lls::IFactory           *lls_f,
        zsp::parser::IFactory   *parser_f,
        int                     port) :
    m_dmgr(dmgr), m_jrpc_f(jrpc_f), m_lls_f(lls_f), 
    m_parser_f(parser_f), m_port(port), m_running(false) {
    DEBUG_INIT("zsp::ls::ThreadMain", dmgr);
}

ThreadMain::~ThreadMain() {

}

bool ThreadMain::start() {
    m_thread = std::thread(&ThreadMain::thread_main, this);
    return true;
}

bool ThreadMain::join() {
    bool running;

    fflush(stdout);

    while (true) {
        m_running_mutex.lock();
        running = m_running = true;
        m_running_mutex.unlock();

        if (running && m_thread.joinable()) {
            m_thread.join();
        } else {
            break;
        }
    }

    return true;
}

void ThreadMain::thread_main() {
    DEBUG_ENTER("thread_main");

    m_running_mutex.lock();
    m_running = true;
    m_running_mutex.unlock();


    jrpc::IEventLoopUP loop(m_jrpc_f->mkEventLoop());
    int32_t fd = m_jrpc_f->mkSocketClientConnection(m_port);
    jrpc::IMessageTransportUP transport(
        m_jrpc_f->mkNBSocketMessageTransport(loop.get(), fd));

    ServerUP server(new Server(loop.get(), m_lls_f, m_parser_f));
    lls::IServerMessageDispatcherUP dispatcher(m_lls_f->mkNBServerMessageDispatcher(
        server->getQueue(),
        transport.get(),
        server.get()
    ));

    while (loop->process_one_event(-1)) {
        ;
    }

    m_running_mutex.lock();
    m_running = false;
    m_running_mutex.unlock();

    DEBUG_LEAVE("thread_main");
}

dmgr::IDebug *ThreadMain::m_dbg = 0;

}
}

zsp::ls::ThreadMain *zsp_ls_createThread(
    dmgr::IDebugMgr             *dmgr,
    jrpc::IFactory              *jrpc_f,
    lls::IFactory               *lls_f,
    zsp::parser::IFactory       *parser_f,
    int                         port) {
    zsp::ls::ThreadMain *thread = new zsp::ls::ThreadMain(
        dmgr,
        jrpc_f,
        lls_f,
        parser_f,
        port);

    thread->start();

    return thread;
}


void zsp_ls_termThread(
    zsp::ls::ThreadMain         *thread);