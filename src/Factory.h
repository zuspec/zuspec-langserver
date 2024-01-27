/**
 * Factory.h
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
#include "zsp/ls/IFactory.h"

namespace zsp {
namespace ls {



class Factory : public virtual IFactory {
public:
    Factory();

    virtual ~Factory();

    virtual void init(
        dmgr::IDebugMgr         *dmgr,
        jrpc::IFactory          *jrpc_f,
        lls::IFactory           *lls_f,
        zsp::parser::IFactory   *parser_f) override;

    virtual IThreadMain *mkThreadMain(int port) override;

    static IFactory *inst();

private:
    static IFactoryUP               m_inst;
    dmgr::IDebugMgr                 *m_dmgr;
    jrpc::IFactory                  *m_jrpc_f;
    lls::IFactory                   *m_lls_f;
    zsp::parser::IFactory           *m_parser_f;

};

}
}


