/**
 * TestBase.h
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
#pragma once
#include <map>
#include "gtest/gtest.h"
#include "dmgr/IDebugMgr.h"
#include "jrpc/IFactory.h"
#include "lls/IFactory.h"
#include "Server.h"

namespace zsp {
namespace ls {

class TestBase : public ::testing::Test {
public:

    struct ClientServerData;
    using ClientServerDataUP=std::unique_ptr<ClientServerData>;
    struct ClientServerData {
        ServerUP                            server;
        lls::IServerMessageDispatcherUP     server_dispatch;
        lls::IClientMessageDispatcherUP     client;
        jrpc::IEventLoopUP                  loop;

        lls::IClientMessageDispatcherUP     client_dispatch;
    };

public:
    TestBase();

    virtual ~TestBase();

    virtual void SetUp() override;

    virtual void TearDown() override;

    ClientServerData mkClientServer();

protected:
    void enableDebug(bool en);

    bool mkdir(const std::string &path);

    bool rmdir(const std::string &path);

//    int mkdirs(const std::string &path);

    bool isdir(const std::string &path);

    std::string basename(const std::string &path);

    std::string dirname(const std::string &path);

    bool createTree(const std::map<std::string,std::string> &files);

protected:
    std::string         m_testdir;
    dmgr::IDebugMgr     *m_dmgr;
    jrpc::IFactory      *m_jrpc_factory;
    lls::IFactory       *m_lls_factory;

};

}
}


