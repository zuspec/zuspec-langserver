/*
 * TaskDocumentSymbols.cpp
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
#include "jrpc/impl/TaskLockRead.h"
#include "TaskDocumentSymbols.h"
#include "TaskBuildDocumentSymbols.h"


namespace zsp {
namespace ls {


TaskDocumentSymbols::TaskDocumentSymbols(
    Context             *ctxt,
    const std::string   &uri,
    const std::string   &id) : TaskBase(ctxt->getQueue()),
        m_ctxt(ctxt), m_uri(uri), m_id(id), m_idx(0) {
    DEBUG_INIT("zsp::ls::TaskDocumentSymbols", ctxt->getDebugMgr());
}

TaskDocumentSymbols::~TaskDocumentSymbols() {

}

jrpc::ITask *TaskDocumentSymbols::run(jrpc::ITask *parent, bool initial) {
    DEBUG_ENTER("run");
    runEnter(parent, initial);

    switch (m_idx) {
        case 0: {
            m_idx = 1;
            jrpc::ITask *n = jrpc::TaskLockRead(m_queue, m_ctxt->getSourceFiles()->getLock()).run(this, true);
            if (n && !n->done()) {
                break;
            }
        }
        case 1: {
            ast::IGlobalScope *ast = 0;
            m_idx = 2;
            if (m_ctxt->getSourceFiles()->hasFile(m_uri)) {
                DEBUG("Has source file");
                SourceFileData *file = m_ctxt->getSourceFiles()->getFile(m_uri);
                if (file->getLiveAst()) {
                    DEBUG("Has live AST");
                    ast = file->getLiveAst();
                } else {
                    DEBUG("TODO: Use static AST");
//                    ast = file->getStaticAst();
                }
            } else {
                DEBUG("Doesn't have source file");
            }

            lls::IDocumentSymbolResponseUP response;
            DEBUG("ast: %p", ast);
            if (ast) {
                TaskBuildDocumentSymbols builder(m_ctxt->getLspFactory());
                response = builder.build(ast);
            } else {
                // Send an empty response
                DEBUG("No AST, so sending empty response");
                std::vector<lls::IDocumentSymbolUP> symbols;
                response = m_ctxt->getLspFactory()->mkDocumentSymbolResponse(symbols);
            }

            m_ctxt->getClient()->sendRspSuccess(m_id, response.release());
        }
        case 2: {
            m_ctxt->getSourceFiles()->getLock()->unlock_read();
            setFlags(jrpc::TaskFlags::Complete);
        }
    }

    DEBUG_LEAVE("run");
    return runLeave(parent, initial);
}

dmgr::IDebug *TaskDocumentSymbols::m_dbg = 0;

}
}
