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
#include <string.h>
#include "SourceFileData.h"

namespace zsp {
namespace ls {


SourceFileData::SourceFileData(
    const std::string       &uri,
    int64_t                 timestamp) :
        m_uri(uri), m_id(-1), m_timestamp(timestamp),
        m_haveMarkers(false) {
    memset(m_has, 0, sizeof(m_has));
    memset(m_hasLink, 0, sizeof(m_hasLink));
    memset(m_hasLive, 0, sizeof(m_hasLive));
    memset(m_hasLinkLive, 0, sizeof(m_hasLinkLive));
}

SourceFileData::~SourceFileData() {

}

void SourceFileData::clearMarkers(bool live) {
    if (live) {
        m_syntaxMarkersLive.clear();
        m_linkMarkersLive.clear();
        m_semanticMarkersLive.clear();
        memset(m_hasLive, 0, sizeof(m_hasLive));
    } else {
        m_syntaxMarkers.clear();
        m_linkMarkers.clear();
        m_semanticMarkers.clear();
        memset(m_has, 0, sizeof(m_has));
    }
}

void SourceFileData::addSyntaxMarker(zsp::parser::IMarkerUP &marker, bool live) {
    if (live) {
        m_hasLive[(int)marker->severity()]++;
        m_syntaxMarkersLive.push_back(std::move(marker));
    } else {
        m_has[(int)marker->severity()]++;
        m_syntaxMarkers.push_back(std::move(marker));
    }
}

void SourceFileData::addLinkMarker(zsp::parser::IMarkerUP &marker, bool live) {
    if (live) {
        m_hasLinkLive[(int)marker->severity()]++;
        m_linkMarkersLive.push_back(std::move(marker));
    } else {
        m_hasLink[(int)marker->severity()]++;
        m_linkMarkers.push_back(std::move(marker));
    }
}

void SourceFileData::addSemanticMarker(zsp::parser::IMarkerUP &marker, bool live) {
    if (live) {
        m_hasLive[(int)marker->severity()]++;
        m_semanticMarkersLive.push_back(std::move(marker));
    } else {
        m_has[(int)marker->severity()]++;
        m_semanticMarkers.push_back(std::move(marker));
    }
}

ast::IGlobalScope *SourceFileData::getLastGoodAst() {
    return 0;
}

void SourceFileData::closeLiveView() {
    // Clear live markers
    m_syntaxMarkersLive.clear();
    m_linkMarkersLive.clear();
    m_semanticMarkersLive.clear();

    m_liveContent.clear();
    m_liveAst.reset();
}

}
}

