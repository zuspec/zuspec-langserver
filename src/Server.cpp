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
#include "jrpc/impl/TaskLambda.h"
#include "lls/IDocumentSymbolResponse.h"
#include "Server.h"
#include "TaskDocumentSymbols.h"
#include "TaskUpdateSourceFileData.h"
#include "TaskFindSourceFiles.h"
#include "TaskWorkspaceStartup.h"
#include "TaskDidChange.h"
#include "TaskDidClose.h"
#include "TaskDidOpen.h"


namespace zsp {
namespace ls {


Server::Server(
        jrpc::IEventLoop        *loop,
        lls::IFactory           *lls_factory,
        zsp::parser::IFactory   *parser_factory) : 
            ServerBase(loop, lls_factory), 
            m_queue(lls_factory->getFactory()->mkTaskQueue(loop)),
            m_ctxt(new Context(
                lls_factory->getDebugMgr(),
                m_queue.get(),
                lls_factory, 
                m_client, 
                parser_factory)) {
    DEBUG_INIT("zsp::ls::Server", m_ctxt->getDebugMgr());
}

Server::~Server() {

}

void Server::init(lls::IClient *client) {
    lls::ServerBase::init(client);
    m_ctxt->setClient(client);
}

lls::IHoverUP Server::hover(
    const std::string       &id,
    lls::IHoverParamsUP     &params) {
    DEBUG_ENTER("hover");
    // File supports multiple simultaneous readers
    // Multiple can read-lock, while only one can write-lock
    // - Lock file, ensuring any activity involving it is complete
    //   - This keeps the 
    // - 
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
    capabilites->setDocumentSymbolProvider(true);

    // Setup a job to find .pss files
    DEBUG("Root path: %s", params->getRootPath().c_str());

    m_roots.push_back(params->getRootPath());
    for (std::vector<std::string>::const_iterator
        it=params->getWorkspaceFolders().begin();
        it!=params->getWorkspaceFolders().end(); it++) {
        DEBUG("Workspace Folder: %s", it->c_str());
        m_roots.push_back(*it);
    }

    TaskWorkspaceStartup(m_ctxt.get(), m_roots).run(0, true);

    lls::IInitializeResultUP ret(m_factory->mkInitializeResult(
        capabilites,
        serverInfo));

    return ret;
}

void Server::didOpen(lls::IDidOpenTextDocumentParamsUP &params) {
    DEBUG_ENTER("didOpen");

    // Now, queue this file for parsing
    // Note: TaskDidOpen is known to yield on start
    TaskDidOpen(
        m_ctxt.get(), 
        params->getTextDocument()->getUri(),
        params->getTextDocument()->getText()).run(0, true);

    DEBUG_LEAVE("didOpen");
}

void Server::didChange(lls::IDidChangeTextDocumentParamsUP &params) {
    DEBUG_ENTER("didChange");

    // TODO: handle incremental change updates

    TaskDidChange(
        m_ctxt.get(),
        params->getTextDocument()->getUri(),
        params->getChanges().at(0)->getText());

    DEBUG_LEAVE("didChange");
}

void Server::didClose(lls::IDidCloseTextDocumentParamsUP &params) {
    DEBUG_ENTER("didClose");

    TaskDidClose(
        m_ctxt.get(),
        params->getTextDocument()->getUri()).run(0, true);

    DEBUG_LEAVE("didClose");
}

// Hover is:
//
// - Semaphore -- holds up-to-date status
// - LockRW    -- tracks who can read/write the resources
//
// 
// - Need the linked state of this file to be up-to-date, and have read-lock that state
//   - Need the state of the filelist to be up-to-date
//     - Will hold a read-lock on the list once any update is complete
//   - Need the parse state of all files to be up-to-date
//     - Must acquire a write lock to the 
//   - Need the (otf) parse state of this file to be up-to-date
//   - Need the link state of this file to be up-to-date
//   -> Result is the link state of this file and a read-lock on it

lls::IDocumentSymbolResponseUP Server::documentSymbols(
    const std::string               &id,
    lls::IDocumentSymbolParamsUP    &params) {
    DEBUG_ENTER("documentSymbols %s", params->getTextDocument()->getUri().c_str());

    TaskDocumentSymbols(
        m_ctxt.get(), 
        params->getTextDocument()->getUri(),
        id).run(0, true);

    DEBUG_LEAVE("documentSymbols");

    return 0;
}

dmgr::IDebug *Server::m_dbg = 0;

}
}
