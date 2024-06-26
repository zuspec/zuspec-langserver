/**
 * TestClient.h
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
#include "dmgr/IDebugMgr.h"
#include "lls/IClient.h"

namespace zsp {
namespace ls {



class TestClient : public virtual lls::IClient {
public:
    TestClient();

    virtual ~TestClient();

    void init(dmgr::IDebugMgr *dmgr);

    virtual void publishDiagnosticsNotification(
        lls::IPublishDiagnosticsParamsUP     &params) override;

    virtual void sendNotification(
        const std::string               &method,
        lls::IJson                      *params) override;

    virtual void sendRspSuccess(
        const std::string               &id,
        lls::IJson                      *result) override;

    virtual void sendRspError(
        const std::string               &id,
        int32_t                         code,
        const std::string               &msg,
        lls::IJson                      *data) override;

    const std::vector<lls::IPublishDiagnosticsParamsUP> &getDiagnostics() const {
        return m_diagnostics;
    }

    const std::vector<lls::IJsonUP> &getResponses() const {
        return m_responses;
    }

    void clearDiagnostics() {
        m_diagnostics.clear();
    }

    void clearAll() {
        clearDiagnostics();
        m_responses.clear();
    }

private:
    static dmgr::IDebug                                 *m_dbg;
    dmgr::IDebugMgr                                     *m_dmgr;
    std::vector<lls::IPublishDiagnosticsParamsUP>       m_diagnostics;
    std::vector<lls::IJsonUP>                           m_responses;


};

}
}


