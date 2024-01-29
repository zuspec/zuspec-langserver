/*
 * Context.cpp
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
#include "Context.h"


namespace zsp {
namespace ls {


Context::Context(
        dmgr::IDebugMgr         *dmgr,
        jrpc::ITaskQueue        *queue,
        lls::IFactory           *lls_f,
        lls::IClient            *client,
        zsp::parser::IFactory   *zspp_f) :
    m_dmgr(dmgr), m_queue(queue), m_lls_f(lls_f), m_client(client),
    m_zspp_f(zspp_f) {

}

Context::~Context() {

}

zsp::parser::IAstBuilder *Context::allocAstBuilder() {
    zsp::parser::IAstBuilder *ret = 0;
    m_mutex.lock();
    if (m_ast_builders.size()) {
        ret = m_ast_builders.back().release();
        m_ast_builders.pop_back();
    } else {
        ret = m_zspp_f->mkAstBuilder(0);
    }
    m_mutex.unlock();
    return ret;
}

void Context::freeAstBuilder(zsp::parser::IAstBuilder *builder) {
    m_mutex.lock();
    m_ast_builders.push_back(zsp::parser::IAstBuilderUP(builder));
    m_mutex.unlock();
}

}
}
