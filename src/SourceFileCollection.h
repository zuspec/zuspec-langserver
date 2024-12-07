/**
 * SourceFileCollection.h
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
#include <map>
#include <mutex>
#include <vector>
#include "dmgr/IDebugMgr.h"
#include "jrpc/ITaskQueue.h"
#include "jrpc/impl/LockRwData.h"
#include "zsp/ast/IRootSymbolScope.h"
#include "SourceFileData.h"

namespace zsp {
namespace ls {

class Context;

class SourceFileCollection;
using SourceFileCollectionUP=std::unique_ptr<SourceFileCollection>;
class SourceFileCollection {
public:
    SourceFileCollection(
        dmgr::IDebugMgr     *dmgr,
        jrpc::ITaskQueue    *queue);

    virtual ~SourceFileCollection();

    virtual int32_t addFile(SourceFileDataUP &file);

    virtual bool hasFile(const std::string &uri);

    virtual SourceFileData *getFile(const std::string &uri);

    dmgr::IDebugMgr *getDebugMgr() const { return m_dmgr; }

    virtual const std::vector<SourceFileDataUP> &getFiles() const {
        return m_file_l;
    }

    jrpc::LockRwData &getRoot() {
        return m_global;
    }

    virtual bool tryLockFile(
        const std::string           &uri,
        bool                        write);

    virtual void updateLiveContent(
        Context                     *ctxt,
        const std::string           &uri,
        const std::string           &liveContent);

    jrpc::LockRw *getLock() {
        return &m_lock;
    }

    bool hasFileUri(int32_t id);

    const std::string &getFileUri(int32_t id);

private:
    static dmgr::IDebug                     *m_dbg;
    dmgr::IDebugMgr                         *m_dmgr;
    jrpc::ITaskQueue                        *m_queue;
    jrpc::LockRw                            m_lock;
    std::map<std::string, jrpc::ITask *>    m_live_update_m;
    std::map<std::string,int32_t>           m_uri_id_m;
    std::map<int32_t,std::string>           m_id_uri_m;

    std::vector<SourceFileDataUP>           m_file_l;

    // Holds ast::IRootSymbolScope
    jrpc::LockRwData                        m_global;

    // Sequence id associated with updates to static AST
    int32_t                                 m_staticAstVersion;

};

}
}

