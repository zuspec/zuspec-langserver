/*
 * TestBase.cpp
 *
 * Copyright 2022 Matthew Ballance and Contributors
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
#include "dmgr/FactoryExt.h"
#include "jrpc/FactoryExt.h"
#include "lls/FactoryExt.h"
#include "zsp/ast/IFactory.h"
#include "zsp/parser/FactoryExt.h"
#include "Server.h"
#include "TestBase.h"
#include <netinet/in.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>


extern "C" zsp::ast::IFactory *ast_getFactory();

namespace zsp {
namespace ls {


TestBase::TestBase() {

}

TestBase::~TestBase() {

}

void TestBase::SetUp() {
    m_dmgr = dmgr_getFactory()->getDebugMgr();
    m_jrpc_factory = jrpc_getFactory();
    m_jrpc_factory->init(m_dmgr);
    m_lls_factory = lls_getFactory();
    m_lls_factory->init(m_jrpc_factory);
}

void TestBase::enableDebug(bool en) {
    m_dmgr->enable(en);
}

TestBase::ClientServerData TestBase::mkClientServer() {
    ClientServerData ret;

    std::pair<int32_t,int32_t> server_port_fd = m_jrpc_factory->mkSocketServer();
    int32_t client_fd = m_jrpc_factory->mkSocketClientConnection(server_port_fd.first);

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    int addrlen = sizeof(addr);

    int32_t server_fd = ::accept(
        server_port_fd.second,
        (struct sockaddr *)&addr,
        (socklen_t *)&addrlen);

    ret.loop = jrpc::IEventLoopUP(m_jrpc_factory->mkEventLoop());

    zsp::parser::IFactory *zsp_parser_f = zsp_parser_getFactory();
    zsp::ast::IFactory *zsp_ast_f = ast_getFactory();
    zsp_parser_f->init(m_dmgr, zsp_ast_f);

    ret.client = lls::IClientMessageDispatcherUP(
        m_lls_factory->mkClientMessageDispatcher(
            m_jrpc_factory->mkNBSocketMessageTransport(
                ret.loop.get(),
                client_fd),
        0 // TODO: client
    ));

    ret.server = ServerUP(new Server(
        ret.loop.get(),
        m_lls_factory,
        zsp_parser_f
    ));

    ret.server_dispatch = lls::IServerMessageDispatcherUP(
        m_lls_factory->mkNBServerMessageDispatcher(
            m_jrpc_factory->mkNBSocketMessageTransport(
                ret.loop.get(),
                server_fd
            ),
            ret.server.get()
        )
    );

    return ret;
}

}
}
