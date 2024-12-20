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
#include "jrpc/impl/LockRw.h"
#include "zsp/ast/IGlobalScope.h"
#include "zsp/ast/IRootSymbolScope.h"
#include "zsp/parser/IMarker.h"

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

    virtual void setFileId(int32_t id) {
        m_id = id;
    }

    virtual int32_t getFileId() const {
        return m_id;
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

    virtual zsp::ast::IGlobalScope *getLiveAst() const {
        return m_liveAst.get();
    }

    virtual void setLiveAst(zsp::ast::IGlobalScopeUP &ast) {
        m_liveAst = std::move(ast);
    }

    virtual ast::IGlobalScope *getDiskAst() const {
        return m_diskAst.get();
    }

    virtual void setDiskAst(ast::IGlobalScopeUP &ast) {
        m_diskAst = std::move(ast);
    }

    virtual zsp::ast::ISymbolScope *getFileSymtab() {
        return m_fileSymtab.get();
    }

    virtual void setFileSymtab(zsp::ast::IRootSymbolScopeUP &symt) {
        m_fileSymtab = std::move(symt);
    }
    
    virtual void clrFileSymtab() {
        m_fileSymtab.reset();
    }

    virtual bool haveMarkers() const {
        return m_haveMarkers;
    }

    virtual void setHaveMarkers(bool h) {
        m_haveMarkers = h;
    }

    virtual void clearLinkMarkers(bool live);

    virtual void clearSyntaxMarkers(bool live);

    virtual void addSyntaxMarker(zsp::parser::IMarkerUP &marker, bool live);

    virtual const std::vector<zsp::parser::IMarkerUP> & getSyntaxMarkers(bool live) const {
        return (live)?m_syntaxMarkersLive:m_syntaxMarkers;
    }

    virtual void addLinkMarker(zsp::parser::IMarkerUP &marker, bool live);

    virtual const std::vector<zsp::parser::IMarkerUP> & getLinkMarkers(bool live) const {
        return (live)?m_linkMarkersLive:m_linkMarkers;
    }

    virtual void addSemanticMarker(zsp::parser::IMarkerUP &marker, bool live);

    virtual const std::vector<zsp::parser::IMarkerUP> & getSemanticMarkers() const {
        return m_semanticMarkers;
    }

    bool hasSeverity(zsp::parser::MarkerSeverityE s, bool live) {
        return (live)?m_hasLive[(int)s]:m_has[(int)s];
    }

    void closeLiveView();

private:
    std::string                             m_uri;
    int32_t                                 m_id;
    int64_t                                 m_timestamp;
    bool                                    m_isOpen;
    std::string                             m_liveContent;
    zsp::ast::IGlobalScopeUP                m_liveAst;
    ast::IGlobalScopeUP                     m_diskAst;
    zsp::ast::IRootSymbolScopeUP            m_fileSymtab;
    int32_t                                 m_fileSymtabVersion;
    bool                                    m_haveMarkers;
    // All markers are relative to the 'static' (on-disk) file view
    std::vector<zsp::parser::IMarkerUP>     m_syntaxMarkers;
    std::vector<zsp::parser::IMarkerUP>     m_linkMarkers;
    std::vector<zsp::parser::IMarkerUP>     m_semanticMarkers;
    std::vector<zsp::parser::IMarkerUP>     m_syntaxMarkersLive;
    std::vector<zsp::parser::IMarkerUP>     m_linkMarkersLive;
    std::vector<zsp::parser::IMarkerUP>     m_semanticMarkersLive;
    int                                     m_has[(int)zsp::parser::MarkerSeverityE::NumLevels];
    int                                     m_hasLink[(int)zsp::parser::MarkerSeverityE::NumLevels];
    int                                     m_hasLive[(int)zsp::parser::MarkerSeverityE::NumLevels];
    int                                     m_hasLinkLive[(int)zsp::parser::MarkerSeverityE::NumLevels];
    jrpc::LockRw                            m_lock;

};

}
}

