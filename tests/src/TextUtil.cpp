/*
 * TextUtil.cpp
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
#include "TextUtil.h"


namespace zsp {
namespace ls {


TextUtil::TextUtil(const std::string &doc) : 
    m_doc(doc), m_idx(0), m_lineno(1), m_linepos(1) {

}

TextUtil::~TextUtil() {

}

int32_t TextUtil::find(const std::string &str) {
    int32_t idx = m_doc.find(str, m_idx);

    if (idx != -1) {
        advance(idx);
    }

    return idx;
}

void TextUtil::advance(int32_t idx) {
    while (m_idx < idx) {
        if (m_doc.at(m_idx) == '\n') {
            m_lineno++;
            m_linepos = 1;
        } else {
            m_linepos++;
        }
        m_idx++;
    }
}

}
}
