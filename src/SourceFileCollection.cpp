/*
 * SourceFileCollection.cpp
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
#include "SourceFileCollection.h"
#include "TaskUpdateSourceFileData.h"

namespace zsp {
namespace ls {


SourceFileCollection::SourceFileCollection(
    dmgr::IDebugMgr         *dmgr,
    jrpc::ITaskQueue        *queue) : m_dmgr(dmgr), m_queue(queue) {

}

SourceFileCollection::~SourceFileCollection() {

}

int32_t SourceFileCollection::addFile(SourceFileDataUP &file) {
    int32_t id = m_file_l.size();
    file->setFileId(id);
    m_uri_id_m.insert({file->getUri(), id});
    m_id_uri_m.insert({id, file->getUri()});
    m_file_l.push_back(std::move(file));
    return id;
}

bool SourceFileCollection::hasFile(const std::string &uri) {
    return (m_uri_id_m.find(uri) != m_uri_id_m.end());
}

SourceFileData *SourceFileCollection::getFile(const std::string &uri) {
    std::map<std::string,int32_t>::const_iterator it = m_uri_id_m.find(uri);
    if (it != m_uri_id_m.end()) {
        return m_file_l.at(it->second).get();
    } else {
        return 0;
    }
}

bool SourceFileCollection::tryLockFile(
        const std::string           &uri,
        bool                        write) {
    DEBUG_ENTER("tryLockFile");

    DEBUG_LEAVE("tryLockFile");
    return false;
}

void SourceFileCollection::updateLiveContent(
        Context                     *ctxt,
        const std::string           &uri,
        const std::string           &liveContent) {
    DEBUG_ENTER("updateLiveContent: %s", uri.c_str());
    SourceFileData *src = getFile(uri);
    if (!src) {
        DEBUG_LEAVE("updateLiveContent - error, file not managed");
        return;
    }

    /** TODO:
    jrpc::ITaskUP task(new TaskUpdateSourceFileData(
        ctxt,
        this,
        src));
     */

    DEBUG_LEAVE("updateLiveContent: %s", uri.c_str());
}

bool SourceFileCollection::hasFileUri(int32_t id) {
    return m_id_uri_m.find(id) != m_id_uri_m.end();
}

const std::string &SourceFileCollection::getFileUri(int32_t id) {
    return m_id_uri_m.find(id)->second;
}

dmgr::IDebug *SourceFileCollection::m_dbg = 0;

}
}
