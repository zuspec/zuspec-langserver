/*
 * TaskFindSourceFiles.cpp
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
#include "TaskFindSourceFiles.h"
#include "SourceFileFinder.h"
#include "TaskUpdateSourceFileData.h"


namespace zsp {
namespace ls {


TaskFindSourceFiles::TaskFindSourceFiles(
    jrpc::ITaskQueue                    *queue,
    lls::IFactory                       *factory,
    SourceFileCollection                *src_files,
    const std::vector<std::string>      &roots) :
        TaskBase(queue), m_factory(factory), m_src_files(src_files),
        m_roots(roots.begin(), roots.end()) {
    DEBUG_INIT("TaskFindSourceFiles", factory->getDebugMgr());
}

TaskFindSourceFiles::TaskFindSourceFiles(TaskFindSourceFiles *o) :
    TaskBase(o), m_factory(o->m_factory), m_src_files(o->m_src_files),
    m_roots(o->m_roots.begin(), o->m_roots.end()) {

}

TaskFindSourceFiles::~TaskFindSourceFiles() {

}

jrpc::ITask *TaskFindSourceFiles::run(jrpc::ITask *parent, bool initial) {
    DEBUG_ENTER("run");
    runEnter(parent, initial);

    SourceFileFinder finder(m_factory->getDebugMgr());

    for (std::vector<std::string>::const_iterator
        it=m_roots.begin();
        it!=m_roots.end(); it++) {
        finder.find(m_src_files, *it);
    }

    // Now, setup jobs to process the new files
    for (std::vector<SourceFileDataUP>::const_iterator
        it=m_src_files->getFiles().begin();
        it!=m_src_files->getFiles().end(); it++) {
        DEBUG("File: %s", (*it)->getUri().c_str());
    }

    DEBUG_LEAVE("run");

    return runLeave(parent, initial);
}

TaskFindSourceFiles *TaskFindSourceFiles::clone() {
    return new TaskFindSourceFiles(this);
}

dmgr::IDebug *TaskFindSourceFiles::m_dbg = 0;

}
}
