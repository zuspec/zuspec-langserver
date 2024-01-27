/**
 * IFactory.h
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
#include "dmgr/IDebugMgr.h"
#include "jrpc/IFactory.h"
#include "lls/IFactory.h"
#include "zsp/ast/IFactory.h"
#include "zsp/parser/IFactory.h"
#include "zsp/ls/IThreadMain.h"

namespace zsp {
namespace ls {

class IFactory;
using IFactoryUP=std::unique_ptr<IFactory>;
class IFactory {
public:

    virtual ~IFactory() { }

    virtual void init(
        dmgr::IDebugMgr         *dmgr,
        jrpc::IFactory          *jrpc_f,
        lls::IFactory           *lls_f,
        zsp::parser::IFactory   *parser_f
    ) = 0;

    virtual IThreadMain *mkThreadMain(int port) = 0;

};

} /* namespace ls */
} /* namespace zsp */


