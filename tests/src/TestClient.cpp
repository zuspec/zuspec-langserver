/*
 * TestClient.cpp
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
#include "dmgr/impl/DebugMacros.h"
#include "TestClient.h"


namespace zsp {
namespace ls {


TestClient::TestClient() : m_dmgr(0) {

}

TestClient::~TestClient() {

}

void TestClient::init(dmgr::IDebugMgr *dmgr) {
    m_dmgr = dmgr;
    DEBUG_INIT("zsp::ls::TestClient", dmgr);
}

void TestClient::publishDiagnosticsNotification(
        lls::IPublishDiagnosticsParamsUP     &params) {
    DEBUG_ENTER("publishDiagnosticNotification");
    m_diagnostics.push_back(std::move(params));
    DEBUG_LEAVE("publishDiagnosticNotification");
}

void TestClient::sendNotification(
        const std::string               &method,
        lls::IJson                      *params) {
    DEBUG_ENTER("sendNotification");

    DEBUG_LEAVE("sendNotification");
}

void TestClient::sendRspSuccess(
        const std::string               &id,
        lls::IJson                      *result) {
    m_responses.push_back(lls::IJsonUP(result));
}

void TestClient::sendRspError(
        const std::string               &id,
        int32_t                         code,
        const std::string               &msg,
        lls::IJson                      *data) {

}

dmgr::IDebug *TestClient::m_dbg = 0;



}
}
