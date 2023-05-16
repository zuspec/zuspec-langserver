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
#include "dmgr/impl/DebugMacros.h"
#include "TaskUpdateSourceFileData.h"


namespace zsp {
namespace ls {


TaskUpdateSourceFileData::TaskUpdateSourceFileData(
    lls::IFactory                   *factory,
    zsp::parser::IAstBuilder        *ast_builder,
    SourceFileData                  *file) :
        m_factory(factory), m_ast_builder(ast_builder), m_file(file) {
    DEBUG_INIT("TaskUpdateSourceFileData", factory->getDebugMgr());
    memset(m_has, 0, sizeof(m_has));
}

TaskUpdateSourceFileData::~TaskUpdateSourceFileData() {

}

bool TaskUpdateSourceFileData::run(jrpc::ITaskQueue *queue) {
    DEBUG_ENTER("run");
    zsp::ast::IGlobalScopeUP global(
        m_ast_builder->getFactory()->mkGlobalScope(m_file->getId()));
    m_ast_builder->setMarkerListener(this);

/*
    m_ast_builder->build(
        global.get(),
    );
 */

    DEBUG_LEAVE("run");
    return false;
}

void TaskUpdateSourceFileData::marker(const zsp::parser::IMarker *m) {
    DEBUG_ENTER("marker: %s", m->msg().c_str());
    m_has[(int)m->severity()]++;

    // TODO: construct marker to send back
    DEBUG_LEAVE("marker: %s", m->msg().c_str());
}

bool TaskUpdateSourceFileData::hasSeverity(zsp::parser::MarkerSeverityE s) {
    return m_has[(int)s];
}

dmgr::IDebug *TaskUpdateSourceFileData::m_dbg = 0;

}
}
