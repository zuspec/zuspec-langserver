/*
 * SourceFileData.cpp
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
#include "SourceFileData.h"

namespace zsp {
namespace ls {


SourceFileData::SourceFileData(
    const std::string       &uri,
    int64_t                 timestamp) :
        m_uri(uri), m_id(-1), m_timestamp(timestamp) {

}

SourceFileData::~SourceFileData() {

}

}
}