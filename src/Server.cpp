/*
 * Server.cpp
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
#include "dmgr/impl/DebugMacros.h"
#include "Server.h"


namespace zsp {
namespace ls {


Server::Server(
        jrpc::IEventLoop        *loop,
        lls::IFactory           *lls_factory,
        zsp::parser::IFactory   *parser_factory) : 
            ServerBase(loop, lls_factory), m_parser_factory(parser_factory) {
    DEBUG_INIT("Server", lls_factory->getDebugMgr());
}

Server::~Server() {

}

lls::IHoverUP Server::hover(lls::IHoverParamsUP &params) {
    DEBUG_ENTER("hover");
    lls::IContentUP content(m_factory->mkContentMarkedString("", "Hello World!"));
    lls::IRangeUP range;
    DEBUG_LEAVE("hover");

    return lls::IHoverUP(m_factory->mkHover(content, range));
}

lls::IInitializeResultUP Server::initialize(lls::IInitializeParamsUP &params) {
    lls::ITextDocumentSyncOptionsUP textDocSync(
            m_factory->mkTextDocumentSyncOptions(true, lls::TextDocumentSyncKind::Incremental));
    lls::IServerCapabilitiesUP capabilites(
        m_factory->mkServerCapabilities(
            textDocSync
        )
    );
    lls::IServerInfoUP serverInfo;

    capabilites->setHoverProvider(true);

    lls::IInitializeResultUP ret(m_factory->mkInitializeResult(
        capabilites,
        serverInfo));

    return ret;
}

dmgr::IDebug *Server::m_dbg = 0;

}
}
