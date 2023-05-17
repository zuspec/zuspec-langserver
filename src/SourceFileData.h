/**
 * SourceFileData.h
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
#include <stdint.h>
#include <memory>
#include <string>
#include "zsp/ast/IGlobalScope.h"

namespace zsp {
namespace ls {

class SourceFileData;
using SourceFileDataUP=std::unique_ptr<SourceFileData>;
class SourceFileData {
public:
    SourceFileData(
        const std::string       &uri,
        int64_t                 timestamp
    );

    virtual ~SourceFileData();

    virtual const std::string &getUri() const {
        return m_uri;
    }

    virtual int32_t getId() const {
        return m_id;
    }

    virtual void setId(int32_t id)  {
        m_id = id;
    }

    virtual int64_t getTimestamp() const {
        return m_timestamp;
    }

    virtual bool isOpen() const {
        return m_isOpen;
    }

    virtual void setIsOpen(bool isOpen) {
        m_isOpen = isOpen;
    }

    virtual const std::string &getLiveContent() const {
        return m_liveContent;
    }

    virtual void setLiveContent(const std::string &c) {
        m_liveContent = c;
    }

    virtual zsp::ast::IGlobalScope *getStaticAst() const {
        return m_staticAst.get();
    }

    virtual void setStaticAst(zsp::ast::IGlobalScopeUP &ast) {
        m_staticAst = std::move(ast);
    }

    virtual zsp::ast::IGlobalScope *getLiveAst() const {
        return m_liveAst.get();
    }

    virtual void setLiveAst(zsp::ast::IGlobalScopeUP &ast) {
        m_liveAst = std::move(ast);
    }

    virtual bool haveMarkers() const {
        return m_haveMarkers;
    }

    virtual void setHaveMarkers(bool h) {
        m_haveMarkers = h;
    }

private:
    std::string                 m_uri;
    int32_t                     m_id;
    int64_t                     m_timestamp;
    bool                        m_isOpen;
    std::string                 m_liveContent;
    zsp::ast::IGlobalScopeUP    m_staticAst;
    zsp::ast::IGlobalScopeUP    m_liveAst;
    bool                        m_haveMarkers;

};

}
}

