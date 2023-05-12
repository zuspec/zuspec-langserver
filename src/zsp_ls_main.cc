#include <string>
#include <stdio.h>
#include "dmgr/FactoryExt.h"
#include "dmgr/impl/DebugMacros.h"
#include "jrpc/FactoryExt.h"
#include "lls/FactoryExt.h"
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include "Server.h"

static dmgr::IDebug             *m_dbg = 0;

using namespace zsp::ls;

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

    dmgr->enable(true);

    DEBUG("Hello");

/*
    const char *IPC_HOOK = getenv("VSCODE_IPC_HOOK_CLI");

    if (!IPC_HOOK || !IPC_HOOK[0]) {
        return 1;
    }

    int32_t client_fd = jrpc_f->mkSocketClientConnection(IPC_HOOK);

    if (client_fd == -1) {
        return 1;
    }
 */

    jrpc::IEventLoopUP loop(jrpc_f->mkEventLoop());
    jrpc::IMessageTransportUP transport(jrpc_f->mkStdioMessageTransport(
        loop.get()
    ));

    ServerUP server(new Server(loop.get(), lls_f));
    lls::IServerMessageDispatcherUP dispatcher(lls_f->mkNBServerMessageDispatcher(
        transport.get(),
        server.get()
    ));

    while (loop->process_one_event(-1)) {
        ;
    }

/*
    const char *IPC_HOOK_EQ = strchr(IPC_HOOK, '=');
    struct sockaddr_un server_sockaddr;
    struct sockaddr_un client_sockaddr;

    dmgr->flush();

    memset(&server_sockaddr, 0, sizeof(struct sockaddr_un));
    memset(&client_sockaddr, 0, sizeof(struct sockaddr_un));

    int client_sock = socket(AF_UNIX, SOCK_STREAM, 0);

    client_sockaddr.sun_family = AF_UNIX;
    strcpy(client_sockaddr.sun_path, IPC_HOOK);
    int len = sizeof(client_sockaddr);

    int rc = connect(client_sock, (struct sockaddr *)&client_sockaddr, len);

    DEBUG("connect rc=%d", rc);
 */

}
