/**
 * Context.h
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
#pragma once
#include <memory>
#include <mutex>
#include "dmgr/IDebugMgr.h"
#include "jrpc/ITask.h"
#include "lls/IFactory.h"
#include "zsp/ast/IFactory.h"
#include "zsp/parser/IAstBuilder.h"
#include "zsp/parser/IFactory.h"

namespace zsp {
namespace ls {

class Context;
using ContextUP=std::unique_ptr<Context>;
class Context {
public:
    Context(
        dmgr::IDebugMgr         *dmgr,
        jrpc::ITaskQueue        *queue,
        lls::IFactory           *lls_f,
        lls::IClient            *client,
        zsp::parser::IFactory   *zspp_f
    );

    virtual ~Context();

    virtual dmgr::IDebugMgr *getDebugMgr() const {
        return m_dmgr;
    }

    virtual zsp::ast::IFactory *getAstFactory() const {
        return m_zspp_f->getAstFactory();
    }

    virtual lls::IFactory *getLspFactory() const {
        return m_lls_f;
    }

    virtual lls::IClient *getClient() const {
        return m_client;
    }

    virtual void setClient(lls::IClient *client) {
        m_client = client;
    }

    virtual zsp::parser::IAstBuilder *allocAstBuilder();

    virtual void freeAstBuilder(zsp::parser::IAstBuilder *builder);

private:
    dmgr::IDebugMgr                             *m_dmgr;
    jrpc::ITaskQueue                            *m_queue;
    lls::IFactory                               *m_lls_f;
    lls::IClient                                *m_client;
    zsp::parser::IFactory                       *m_zspp_f;
    std::vector<zsp::parser::IAstBuilderUP>     m_ast_builders;
    std::mutex                                  m_mutex;

};

}
}


