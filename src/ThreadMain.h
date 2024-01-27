/**
 * ThreadMain.h
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
#include <thread>
#include "dmgr/IDebugMgr.h"
#include "jrpc/IFactory.h"
#include "lls/IFactory.h"
#include "zsp/ast/IFactory.h"
#include "zsp/ls/IThreadMain.h"
#include "zsp/parser/IFactory.h"

namespace zsp {
namespace ls {


/**
 * @brief Entrypoint for running LS as a thread.
 * Note: this is currently only done for testing
 * 
 */
class ThreadMain : public virtual IThreadMain {
public:
    ThreadMain(
        dmgr::IDebugMgr         *dmgr,
        jrpc::IFactory          *jrpc_f,
        lls::IFactory           *lls_f,
        zsp::parser::IFactory   *parser_f,
        int                     port);

    virtual ~ThreadMain();

    bool start();

private:
    void thread_main();

private:
    static dmgr::IDebug         *m_dbg;
    dmgr::IDebugMgr             *m_dmgr;
    jrpc::IFactory              *m_jrpc_f;
    lls::IFactory               *m_lls_f;
    zsp::parser::IFactory       *m_parser_f;
    int                         m_port;
    std::thread                 m_thread;
};

}
}

extern "C" zsp::ls::ThreadMain *zsp_ls_createThread(
    dmgr::IDebugMgr             *dmgr,
    jrpc::IFactory              *jrpc_f,
    lls::IFactory               *lls_f,
    zsp::parser::IFactory       *parser_f,
    int                         port);

extern "C" void zsp_ls_termThread(
    zsp::ls::ThreadMain         *thread);

