/**
 * zsp_ls_main.cc
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
#include <string>
#include <stdio.h>
#include "dmgr/FactoryExt.h"
#include "dmgr/impl/DebugMacros.h"
#include "jrpc/FactoryExt.h"
#include "lls/FactoryExt.h"
#include "zsp/parser/FactoryExt.h"
#include "zsp/ast/IFactory.h"
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include "Server.h"

static dmgr::IDebug             *m_dbg = 0;

using namespace zsp::ls;

extern "C" zsp::ast::IFactory *ast_getFactory();

int main(int argc, char **argv) {
    dmgr::IFactory *dmgr_f = dmgr_getFactory();

    jrpc::IFactory *jrpc_f = jrpc_getFactory();
    jrpc_f->init(dmgr_f->getDebugMgr());

    lls::IFactory *lls_f = lls_getFactory();
    lls_f->init(jrpc_f);

    dmgr::IDebugOutList *out_l = dmgr_f->mkDebugOutList();

    FILE *log_fp = fopen("/home/mballance/debug.log", "w");
    out_l->addOutput(dmgr_f->mkDebugOutFile(log_fp, true));
//    out_l->addOutput(dmgr_f->mkDebugOutFile(stdout, false));

    dmgr::IDebugMgr *dmgr = dmgr_getFactory()->getDebugMgr();
    dmgr->setDebugOut(out_l);

    DEBUG_INIT("zsp-langserver", dmgr);

    dmgr->registerSignalHandlers();
    dmgr->enable(true);

    DEBUG("Hello");

    jrpc::IEventLoopUP loop(jrpc_f->mkEventLoop());
    jrpc::IMessageTransportUP transport(jrpc_f->mkStdioMessageTransport(
        loop.get()
    ));

    zsp::ast::IFactory *zsp_ast_f = ast_getFactory();
    zsp::parser::IFactory *zsp_parser_f = zsp_parser_getFactory();
    zsp_parser_f->init(dmgr, zsp_ast_f);

    ServerUP server(new Server(loop.get(), lls_f, zsp_parser_f));
    lls::IServerMessageDispatcherUP dispatcher(lls_f->mkNBServerMessageDispatcher(
        server->getQueue(),
        transport.get(),
        server.get()
    ));

    while (loop->process_one_event(-1)) {
        ;
    }

}
