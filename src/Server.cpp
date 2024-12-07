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
#include "TaskDeclaration.h"
#include "TaskDidChange.h"
#include "TaskDidClose.h"
#include "TaskDidOpen.h"
#include "TaskDidSave.h"
#include "TaskHover.h"


namespace zsp {
namespace ls {


Server::Server(
        jrpc::IEventLoop        *loop,
        lls::IFactory           *lls_factory,
        zsp::parser::IFactory   *parser_factory) : 
            ServerBase(loop, lls_factory), m_exited(false),
            m_queue(lls_factory->getFactory()->mkTaskQueue(loop)),
            m_ctxt(new Context(
                lls_factory->getDebugMgr(),
                m_queue.get(),
                lls_factory, 
                m_client, 
                parser_factory)) {
    DEBUG_INIT("zsp::ls::Server", m_ctxt->getDebugMgr());
    loop->setTaskQueue(m_queue.get());
}

Server::~Server() {

}

void Server::init(lls::IClient *client) {
    lls::ServerBase::init(client);
    m_ctxt->setClient(client);
}

void Server::hover(
    const std::string       &id,
    lls::IHoverParamsUP     &params) {
    DEBUG_ENTER("hover");
    // File supports multiple simultaneous readers
    // Multiple can read-lock, while only one can write-lock
    // - Lock file, ensuring any activity involving it is complete
    //   - This keeps the 
    // - 
    TaskHover(
        id,
        m_ctxt.get(),
        params->getTextDocument()->getUri(),
        params->getPosition()->getLine(),
        params->getPosition()->getCharacter()).run(0, true);



    DEBUG_LEAVE("hover");
}

void Server::initialize(
    const std::string           &id,
    lls::IInitializeParamsUP    &params) {
    lls::ITextDocumentSyncOptionsUP textDocSync(
            m_factory->mkTextDocumentSyncOptions(
                true, 
                lls::TextDocumentSyncKind::Incremental,
                false, // will_save
                false, // will_save_wait_until
                true,  // save
                false  // include_text
                ));
    lls::IServerCapabilitiesUP capabilites(
        m_factory->mkServerCapabilities(
            textDocSync
        )
    );
    lls::IServerInfoUP serverInfo;

    capabilites->setHoverProvider(true);
    capabilites->setDocumentSymbolProvider(true);
    capabilites->setDeclarationProvider(true);

    // Setup a job to find .pss files
    DEBUG("Root path: %s", params->getRootPath().c_str());

    m_roots.push_back(params->getRootPath());
    for (std::vector<std::string>::const_iterator
        it=params->getWorkspaceFolders().begin();
        it!=params->getWorkspaceFolders().end(); it++) {
        DEBUG("Workspace Folder: %s", it->c_str());
        m_roots.push_back(*it);
    }

    m_ctxt->getClient()->sendRspSuccess(id, 
            m_ctxt->getLspFactory()->mkInitializeResult(
                capabilites,
                serverInfo).release());
}

void Server::initialized() {
    DEBUG_ENTER("initialized");

    TaskWorkspaceStartup(m_ctxt.get(), m_roots).run(0, true);

    DEBUG_LEAVE("initialized");
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
        params->getChanges().at(0)->getText()).run(0, true);

    DEBUG_LEAVE("didChange");
}

void Server::didSave(lls::IDidSaveTextDocumentParamsUP &params) {
    DEBUG_ENTER("didSave");
    TaskDidSave(
        m_ctxt.get(), 
        params->getTextDocument()->getUri()).run(0, true);
    DEBUG_LEAVE("didSave");
}

void Server::didClose(lls::IDidCloseTextDocumentParamsUP &params) {
    DEBUG_ENTER("didClose");

    TaskDidClose(
        m_ctxt.get(),
        params->getTextDocument()->getUri()).run(0, true);

    DEBUG_LEAVE("didClose");
}

void Server::declaration(
        const std::string               &id,
        lls::IDeclarationParamsUP       &params) {
    DEBUG_ENTER("declaration");
    TaskDeclaration(
        m_ctxt.get(),
        id,
        params->getTextDocument()->getUri(),
        params->getPosition()->getLine(),
        params->getPosition()->getCharacter()
    ).run(0, true);

    DEBUG_LEAVE("declaration");
}

void Server::definition(
        const std::string                       &id,
        lls::ITextDocumentPositionParamsUP      &params) {
    DEBUG_ENTER("definition");
    TaskDeclaration(
        m_ctxt.get(),
        id,
        params->getTextDocument()->getUri(),
        params->getPosition()->getLine(),
        params->getPosition()->getCharacter()
    ).run(0, true);

    DEBUG_LEAVE("definition");
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

void Server::documentSymbols(
    const std::string               &id,
    lls::IDocumentSymbolParamsUP    &params) {
    DEBUG_ENTER("documentSymbols %s", params->getTextDocument()->getUri().c_str());

    TaskDocumentSymbols(
        m_ctxt.get(), 
        params->getTextDocument()->getUri(),
        id).run(0, true);

    DEBUG_LEAVE("documentSymbols");
}

void Server::shutdown(const std::string &id) {
    DEBUG_ENTER("shutdown");
    m_client->sendRspSuccess(id, nullptr);
    m_exited = true;
    DEBUG_LEAVE("shutdown");
}

dmgr::IDebug *Server::m_dbg = 0;

}
}
