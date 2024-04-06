/**
 * Server.h
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
#pragma once
#include "jrpc/ITaskQueue.h"
#include "lls/impl/ServerBase.h"
#include "zsp/parser/IFactory.h"
#include "Context.h"
#include "SourceFileCollection.h"

namespace zsp {
namespace ls {

class Server;
using ServerUP=std::unique_ptr<Server>;
class Server : public virtual lls::ServerBase {
public:
    Server(
        jrpc::IEventLoop        *loop,
        lls::IFactory           *lls_factory,
        zsp::parser::IFactory   *parser_factory);

    virtual ~Server();
    
    virtual void init(lls::IClient *client) override;

    virtual jrpc::ITaskQueue *getQueue() const { return m_queue.get(); }

    virtual lls::IHoverUP hover(
        const std::string       &id,
        lls::IHoverParamsUP     &params) override;

	virtual lls::IInitializeResultUP initialize(lls::IInitializeParamsUP &params) override;

    virtual void didOpen(lls::IDidOpenTextDocumentParamsUP &params) override;

    virtual void didChange(lls::IDidChangeTextDocumentParamsUP &params) override;

    virtual void didClose(lls::IDidCloseTextDocumentParamsUP &params) override;

    virtual lls::IDocumentSymbolResponseUP documentSymbols(
        const std::string               &id,
        lls::IDocumentSymbolParamsUP    &params) override;

protected:
    static dmgr::IDebug         *m_dbg;
    jrpc::ITaskQueueUP          m_queue;
    ContextUP                   m_ctxt;
    SourceFileCollectionUP      m_source_files;
    std::vector<std::string>    m_roots;

};

}
}


