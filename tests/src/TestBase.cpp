/*
 * TestBase.cpp
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
#include "dmgr/FactoryExt.h"
#include "jrpc/FactoryExt.h"
#include "lls/FactoryExt.h"
#include "zsp/ast/IFactory.h"
#include "zsp/parser/FactoryExt.h"
#include "Server.h"
#include "TestBase.h"
#include <netinet/in.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>
#include <limits.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <spawn.h>
#include <unistd.h>
#include <fcntl.h>

extern "C" zsp::ast::IFactory *ast_getFactory();

namespace zsp {
namespace ls {


TestBase::TestBase() {

}

TestBase::~TestBase() {

}

void TestBase::SetUp() {
    m_dmgr = dmgr_getFactory()->getDebugMgr();
    m_jrpc_factory = jrpc_getFactory();
    m_jrpc_factory->init(m_dmgr);
    m_lls_factory = lls_getFactory();
    m_lls_factory->init(m_jrpc_factory);

    zsp::ast::IFactory *zsp_ast_f = ast_getFactory();
    m_zsp_factory = zsp_parser_getFactory();
    m_zsp_factory->init(m_dmgr, zsp_ast_f);

    m_rm_testdir = true;

    // Create the run directory
    std::string testname = ::testing::UnitTest::GetInstance()->current_test_info()->test_suite_name();
    testname += ".";
    testname += ::testing::UnitTest::GetInstance()->current_test_info()->name();

    char tmp[2048];
    getcwd(tmp, sizeof(tmp)-1);

    std::string rundir = tmp;
    rundir += "/rundir";
    m_testdir = rundir + "/" + testname;

    ASSERT_TRUE(mkdir(m_testdir));

}

void TestBase::TearDown() {
    if (::testing::Test::HasFailure() || !m_rm_testdir) {
        fprintf(stdout, "Note: preserving test directory %s\n", m_testdir.c_str());
    } else {
        fprintf(stdout, "Note: removing test directory %s\n", m_testdir.c_str());
        rmdir(m_testdir);
    }
}

void TestBase::enableDebug(bool en) {
    m_dmgr->enable(en);
}

bool TestBase::mkdir(const std::string &path) {
    std::string tmp = path;
    if (tmp.at(tmp.size()-1) == '/') {
            tmp = tmp.substr(0, tmp.size()-1);
    }
    int32_t idx=1;

    while ((idx=tmp.find('/', idx)) != std::string::npos) {
            std::string elem = tmp.substr(0, idx);
            ::mkdir(elem.c_str(), S_IRWXU);
            idx++;
    }

    ::mkdir(path.c_str(), S_IRWXU);

    return true;
}

bool TestBase::rmdir(const std::string &path) {
    size_t path_len;
    char *full_path;
    DIR *dir;
    struct stat stat_path, stat_entry;
    struct dirent *entry;

    // stat for the path
    stat(path.c_str(), &stat_path);

    // if path does not exists or is not dir - exit with status -1
    if (S_ISDIR(stat_path.st_mode) == 0) {
        // Doesn't exist
        return false;
    }

    // if not possible to read the directory for this user
    if ((dir = opendir(path.c_str())) == NULL) {
        return false;
    }

    // the length of the path
    path_len = strlen(path.c_str());

    // iterate through directories
    while ((entry = readdir(dir)) != NULL) {

        // skip entries "." and ".."
        if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, "..")) {
            continue;
        }

        // determinate a full path of an entry
        std::string filename = path + "/" + entry->d_name;

        // recursively remove a nested directory
        if (isdir(filename)) {
            rmdir(filename);
            continue;
        }

        // remove a file object
        if (unlink(filename.c_str()) != 0) {
            fprintf(stdout, "Error: Can't remove file: %s\n", filename.c_str());
        }
    }

    // Remove the empty directory
    if (::rmdir(path.c_str()) != 0) {
        fprintf(stdout, "Error: Can't remove directory: %s\n", path.c_str());
        closedir(dir);
        return false;
    } else {
        closedir(dir);
        return true;
    }
}

// int TestBase::mkdirs(const std::string &path) {

// }


bool TestBase::isdir(const std::string &path) {
    struct stat sb;
    return (stat(path.c_str(), &sb) == 0 && S_ISDIR(sb.st_mode));
}

std::string TestBase::basename(const std::string &path) {
    int idx;

    if ((idx=path.rfind("/")) != -1) {
        return path.substr(idx+1);
    } else {
        return path;
    }
}

std::string TestBase::dirname(const std::string &path) {
    int idx;

    if ((idx=path.rfind("/")) != -1) {
        return path.substr(0, idx);
    } else {
        return "";
    }
}

bool TestBase::createTree(const std::map<std::string,std::string> &files) {
    for (std::map<std::string,std::string>::const_iterator 
        it=files.begin();
        it!=files.end(); it++) {
        std::string dir = dirname(it->first);
        std::string file = basename(it->first);

        fprintf(stdout, "dir=%s ; base=%s\n", dir.c_str(), file.c_str());

        if (dir == "") {
            dir = m_testdir;
        } else {
            dir = m_testdir + "/" + dir;
        }

        if (!isdir(dir)) {
            mkdir(dir);
        }

        std::string filename = dir + "/" + file;

        FILE *fp = fopen(filename.c_str(), "w");
        fwrite(it->second.c_str(), 1, it->second.size(), fp);
        fclose(fp);
    }
}

TestBase::ClientServerData TestBase::mkClientServer() {
    ClientServerData ret;

    std::pair<int32_t,int32_t> server_port_fd = m_jrpc_factory->mkSocketServer();
    int32_t client_fd = m_jrpc_factory->mkSocketClientConnection(server_port_fd.first);

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    int addrlen = sizeof(addr);

    int32_t server_fd = ::accept(
        server_port_fd.second,
        (struct sockaddr *)&addr,
        (socklen_t *)&addrlen);

    ret.loop = jrpc::IEventLoopUP(m_jrpc_factory->mkEventLoop());

    zsp::parser::IFactory *zsp_parser_f = zsp_parser_getFactory();
    zsp::ast::IFactory *zsp_ast_f = ast_getFactory();
    zsp_parser_f->init(m_dmgr, zsp_ast_f);

    ret.client = lls::IClientMessageDispatcherUP(
        m_lls_factory->mkClientMessageDispatcher(
            0,
            m_jrpc_factory->mkNBSocketMessageTransport(
                ret.loop.get(),
                client_fd),
        0 // TODO: client
    ));

    ret.server = ServerUP(new Server(
        ret.loop.get(),
        m_lls_factory,
        zsp_parser_f
    ));

    ret.server_dispatch = lls::IServerMessageDispatcherUP(
        m_lls_factory->mkNBServerMessageDispatcher(
            0,
            m_jrpc_factory->mkNBSocketMessageTransport(
                ret.loop.get(),
                server_fd
            ),
            ret.server.get()
        )
    );

    return ret;
}

}
}
