/*
 * Factory.cpp
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
#include "zsp/ls/FactoryExt.h"
#include "Factory.h"
#include "ThreadMain.h"


namespace zsp {
namespace ls {


Factory::Factory() :
    m_dmgr(0), m_jrpc_f(0), m_lls_f(0), m_parser_f(0) {

}

Factory::~Factory() {

}

void Factory::init(
        dmgr::IDebugMgr         *dmgr,
        jrpc::IFactory          *jrpc_f,
        lls::IFactory           *lls_f,
        zsp::parser::IFactory   *parser_f) {
    m_dmgr = dmgr;
    m_jrpc_f = jrpc_f;
    m_lls_f = lls_f;
    m_parser_f = parser_f;
}

IThreadMain *Factory::mkThreadMain(int port) {
    return new ThreadMain(m_dmgr, m_jrpc_f, m_lls_f, m_parser_f, port);
}

IFactory *Factory::inst() {
    if (!m_inst) {
        m_inst = IFactoryUP(new Factory());
    }
    return m_inst.get();
}

IFactoryUP Factory::m_inst;

}
}

zsp::ls::IFactory *zsp_ls_getFactory() {
    return zsp::ls::Factory::inst();
}

