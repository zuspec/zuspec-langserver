/*
 * TaskUpdateSourceFileData.cpp
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
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include "dmgr/impl/DebugMacros.h"
#include "TaskUpdateSourceFileData.h"


namespace zsp {
namespace ls {


TaskUpdateSourceFileData::TaskUpdateSourceFileData(
    lls::IFactory                   *factory,
    lls::IClient                    *client,
    zsp::parser::IAstBuilder        *ast_builder,
    SourceFileData                  *file) :
        m_factory(factory), m_client(client), 
        m_ast_builder(ast_builder), m_file(file) {
    DEBUG_INIT("TaskUpdateSourceFileData", factory->getDebugMgr());
    memset(m_has, 0, sizeof(m_has));
    DEBUG("src.getLiveContent.size()=%d", file->getLiveContent().size());
}

TaskUpdateSourceFileData::~TaskUpdateSourceFileData() {

}

bool TaskUpdateSourceFileData::run(jrpc::ITaskQueue *queue) {
    DEBUG_ENTER("run");
    zsp::ast::IGlobalScopeUP global(
        m_ast_builder->getFactory()->mkGlobalScope(m_file->getId()));
    m_ast_builder->setMarkerListener(this);
    std::istream *is = 0;
    std::stringstream sstr;
    std::ifstream fstr;

    DEBUG("File: %s ; content: %d",
        m_file->getUri().c_str(),
        m_file->getLiveContent().size());

    if (m_file->getLiveContent().size()) {
        // We're working with live content
        sstr = std::stringstream(m_file->getLiveContent());
        is = &sstr;
    } else {
        // We're parsing content off disk
        // TODO:
        std::string path = m_file->getUri().substr(7);
        fstr.open(path.c_str(), std::ios::in);
        is = &fstr;
    }

    if (is) {
    m_ast_builder->build(
        global.get(),
        is
    );
    }

    if (is == &fstr) {
        fstr.close();
    }
    
    if (m_file->getLiveContent().size()) {
        if (m_diagnostics.size() == 0) {
            m_file->setLiveAst(global);
        }
    } else {
        m_file->setStaticAst(global);
    }

    // We need to publish diagnostics if we have new ones (from the parse)
    // as well as if a prior parse had markers and the current one does not.
    // This operation clears the previously-set markers
    if (true /*m_diagnostics.size() > 0 || m_file->haveMarkers() */) {
        m_file->setHaveMarkers(m_diagnostics.size() > 0);
        lls::IPublishDiagnosticsParamsUP params(m_factory->mkPublishDiagnosticsParams(
            m_file->getUri(),
            -1,
            m_diagnostics));

        m_client->publishDiagnosticsNotification(params);
    }

    DEBUG_LEAVE("run");
    return false;
}

void TaskUpdateSourceFileData::marker(const zsp::parser::IMarker *m) {
    DEBUG_ENTER("marker: %s", m->msg().c_str());
    m_has[(int)m->severity()]++;

    lls::DiagnosticSeverity severity = lls::DiagnosticSeverity::Information;
    switch (m->severity()) {
        case zsp::parser::MarkerSeverityE::Error:
            severity = lls::DiagnosticSeverity::Error;
            break;
        case zsp::parser::MarkerSeverityE::Warn:
            severity = lls::DiagnosticSeverity::Warning;
            break;
        case zsp::parser::MarkerSeverityE::Info:
            severity = lls::DiagnosticSeverity::Information;
            break;
        case zsp::parser::MarkerSeverityE::Hint:
            severity = lls::DiagnosticSeverity::Hint;
            break;
    }

    lls::IPositionUP start(m_factory->mkPosition(m->loc().lineno, m->loc().linepos));
    lls::IPositionUP end(m_factory->mkPosition(m->loc().lineno, m->loc().linepos+1));
    lls::IRangeUP range(m_factory->mkRange(start, end));
    lls::IDiagnosticUP diagnostic(m_factory->mkDiagnostic(
        range,
        severity,
        m->msg()
    ));
    m_diagnostics.push_back(std::move(diagnostic));

    DEBUG_LEAVE("marker: %s", m->msg().c_str());
}

bool TaskUpdateSourceFileData::hasSeverity(zsp::parser::MarkerSeverityE s) {
    return m_has[(int)s];
}

dmgr::IDebug *TaskUpdateSourceFileData::m_dbg = 0;

}
}
