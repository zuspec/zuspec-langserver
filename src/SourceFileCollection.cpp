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
#include "SourceFileCollection.h"

namespace zsp {
namespace ls {


SourceFileCollection::SourceFileCollection() {

}

SourceFileCollection::~SourceFileCollection() {

}

void SourceFileCollection::addFile(SourceFileDataUP &file) {
    file->setId(m_file_l.size());
    m_uri_id_m.insert({file->getUri(), file->getId()});
    m_file_l.push_back(std::move(file));
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

}
}
