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
#include "lls/IDocumentSymbolResponse.h"
#include "Server.h"
#include "TaskBuildDocumentSymbols.h"
#include "TaskUpdateSourceFileData.h"
#include "TaskFindSourceFiles.h"


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
                parser_factory)),
            m_source_files(new SourceFileCollection(
                lls_factory->getDebugMgr(),
                m_queue.get())) {
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
    std::vector<std::string> roots;
    DEBUG("Root path: %s", params->getRootPath().c_str());

    roots.push_back(params->getRootPath());
    for (std::vector<std::string>::const_iterator
        it=params->getWorkspaceFolders().begin();
        it!=params->getWorkspaceFolders().end(); it++) {
        DEBUG("Workspace Folder: %s", it->c_str());
        roots.push_back(*it);
    }

    m_queue->addTask(new TaskFindSourceFiles(
        m_queue->mkTaskGroup(),
        m_factory, 
        m_source_files.get(),
        roots), true);

    lls::IInitializeResultUP ret(m_factory->mkInitializeResult(
        capabilites,
        serverInfo));

    return ret;
}

void Server::didOpen(lls::IDidOpenTextDocumentParamsUP &params) {
    DEBUG_ENTER("didOpen");
    SourceFileData *src;

    // TaskGroup
    // - Ensure file discovery is up-to-date
    // - Acquire lock on URI
    //   - Obtain or create 'src'
    //   - Parse live content (no need to lock)
    //   - Create "everything but me" linked image
    //   - Create initial "everything + live" linked image
    //   - Send response
    //   - Queue any marker-propagation tasks

    if (m_source_files->hasFile(params->getTextDocument()->getUri())) {
        DEBUG("File already found");
        src = m_source_files->getFile(params->getTextDocument()->getUri());
    } else {
        // This file wasn't found during discovery, so we add it to the
        // collection that we're managing now.
        // TODO: Should we mark it as somehow 'unmanaged'?
        DEBUG("File not found already");
        src = new SourceFileData(
            params->getTextDocument()->getUri(),
            -1);
        SourceFileDataUP src_up(src);
        m_source_files->addFile(src_up);
    }
    DEBUG("Set Live Content: %d", params->getTextDocument()->getText().size());
    src->setLiveContent(params->getTextDocument()->getText());
    DEBUG("  Set Live Content: %d", src->getLiveContent().size());

    // Now, queue this file for parsing
    m_queue->addTaskPreempt(new TaskUpdateSourceFileData(
        m_queue->mkTaskGroup(),
        m_ctxt.get(),
        m_source_files.get(),
        src), true);
    DEBUG_LEAVE("didOpen");
}

void Server::didChange(lls::IDidChangeTextDocumentParamsUP &params) {
    SourceFileData *src;

    if (!m_source_files->hasFile(params->getTextDocument()->getUri())) {
        // ERROR
        DEBUG("Error: attempting to change an unopened file");
        return;
    }

    src = m_source_files->getFile(params->getTextDocument()->getUri());

    for (std::vector<lls::ITextDocumentContentChangeEventUP>::const_iterator
        it=params->getChanges().begin();
        it!=params->getChanges().end(); it++) {
        DEBUG("Change: hasRange=%p text=%s",
            (*it)->getRange(),
            (*it)->getText().c_str());
    }
    src->setLiveContent(params->getChanges().at(0)->getText());

    // TODO: file-content changing should prompt an
    // update of the linked view for this file
    // Link updates depend on both the 'live' content being
    // parsed *and* any updates of non-live content completing

    // Now, queue this file for parsing
    m_queue->addTask(new TaskUpdateSourceFileData(
        m_queue->mkTaskGroup(),
        m_ctxt.get(),
        m_source_files.get(),
        src), true);
}

void Server::didClose(lls::IDidCloseTextDocumentParamsUP &params) {
    DEBUG_ENTER("didClose");
    SourceFileData *src;

    if (!m_source_files->hasFile(params->getTextDocument()->getUri())) {
        // ERROR
        return;
    }

    src = m_source_files->getFile(params->getTextDocument()->getUri());
    // Revert to being a closed file
    src->setLiveContent("");

    // Now, queue this file for parsing
    jrpc::TaskStatus ret = TaskUpdateSourceFileData(
        m_queue->mkTaskGroup(),
        m_ctxt.get(),
        m_source_files.get(), src).run();

    DEBUG_LEAVE("didClose");
}

lls::IDocumentSymbolResponseUP Server::documentSymbols(
    const std::string               &id,
    lls::IDocumentSymbolParamsUP    &params) {
    DEBUG_ENTER("documentSymbols %s", params->getTextDocument()->getUri().c_str());
    SourceFileData *src = 0;
    zsp::ast::IGlobalScope *global = 0;

    // Because we're just looking at symbols in a single file,
    // we only need to predicate symbols on live updates to 
    // this file.
    // - Pending 'didOpen' handling
    // - Pending 'fileChange' operations
    // 
    // First, check if 'global' is locked
    // - If so, wait
    // Next, check if <file> is locked
    // - If so, wait
    // Finally, lock file m,.

    lls::IDocumentSymbolResponseUP response;
    if (m_source_files->hasFile(params->getTextDocument()->getUri())) {
        src = m_source_files->getFile(params->getTextDocument()->getUri());
        global = (src->getLiveAst())?src->getLiveAst():src->getStaticAst();
    }

    if (!global) {
        // ERROR -- Create a null response to keep everything moving
        std::vector<lls::IDocumentSymbolUP> symbols;
        response = m_factory->mkDocumentSymbolResponse(symbols);
        DEBUG("Global is null");
    } else {
        TaskBuildDocumentSymbols builder(m_factory);
        response = builder.build(global);
    }

    DEBUG_LEAVE("documentSymbols");

    return response;
}

dmgr::IDebug *Server::m_dbg = 0;

}
}
