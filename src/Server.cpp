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
#include "TaskUpdateSourceFileData.h"
#include "TaskFindSourceFiles.h"


namespace zsp {
namespace ls {


Server::Server(
        jrpc::IEventLoop        *loop,
        lls::IFactory           *lls_factory,
        zsp::parser::IFactory   *parser_factory) : 
            ServerBase(loop, lls_factory), m_parser_factory(parser_factory),
            m_source_files(new SourceFileCollection()),
            m_queue(lls_factory->getFactory()->mkTaskQueue(loop)) {
    DEBUG_INIT("Server", lls_factory->getDebugMgr());

    m_ast_builder = zsp::parser::IAstBuilderUP(m_parser_factory->mkAstBuilder(0));
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

    jrpc::ITaskUP task(new TaskFindSourceFiles(m_factory, m_source_files.get(), roots));
    m_queue->addTask(task);

    lls::IInitializeResultUP ret(m_factory->mkInitializeResult(
        capabilites,
        serverInfo));

    return ret;
}

void Server::didOpen(lls::IDidOpenTextDocumentParamsUP &params) {
    DEBUG_ENTER("didOpen");
    SourceFileData *src;

    if (m_source_files->hasFile(params->getTextDocument()->getUri())) {
        src = m_source_files->getFile(params->getTextDocument()->getUri());
    } else {
        // This file wasn't found during discovery, so we add it to the
        // collection that we're managing now.
        // TODO: Should we mark it as somehow 'unmanaged'?
        src = new SourceFileData(
            params->getTextDocument()->getUri(),
            -1);
    }
    src->setLiveContent(params->getTextDocument()->getText());

    // Now, queue this file for parsing
    jrpc::ITaskUP task(new TaskUpdateSourceFileData(
        m_factory,
        m_ast_builder.get(), 
        src));
    m_queue->addTask(task);
    DEBUG_LEAVE("didOpen");
}

void Server::didChange(lls::IDidChangeTextDocumentParamsUP &params) {
    SourceFileData *src;

    if (!m_source_files->hasFile(params->getTextDocument()->getUri())) {
        // ERROR
        return;
    }

    src = m_source_files->getFile(params->getTextDocument()->getUri());
    // TODO: apply changes
//    src->setLiveContent(params->getChanges()->

    // Now, queue this file for parsing
    jrpc::ITaskUP task(new TaskUpdateSourceFileData(
        m_factory,
        m_ast_builder.get(), 
        src));
    m_queue->addTask(task);
}

void Server::didClose(lls::IDidCloseTextDocumentParamsUP &params) {
    SourceFileData *src;

    if (!m_source_files->hasFile(params->getTextDocument()->getUri())) {
        // ERROR
        return;
    }

    src = m_source_files->getFile(params->getTextDocument()->getUri());
    // Revert to being a closed file
    src->setLiveContent("");

    // Now, queue this file for parsing
    jrpc::ITaskUP task(new TaskUpdateSourceFileData(
        m_factory,
        m_ast_builder.get(), 
        src));
    m_queue->addTask(task);
}

dmgr::IDebug *Server::m_dbg = 0;

}
}
