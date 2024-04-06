/*
 * TaskUpdateAllSourceFiles.cpp
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
#include "TaskUpdateAllSourceFiles.h"


namespace zsp {
namespace ls {


TaskUpdateAllSourceFiles::TaskUpdateAllSourceFiles(
    jrpc::ITaskQueue            *queue,
    Context                     *ctxt,
    SourceFileCollection        *src_files) :
        TaskBase(queue), m_ctxt(ctxt), m_src_files(src_files) {

}

TaskUpdateAllSourceFiles::TaskUpdateAllSourceFiles(TaskUpdateAllSourceFiles *o) :
    TaskBase(o), m_ctxt(o->m_ctxt), m_src_files(o->m_src_files) { }

TaskUpdateAllSourceFiles::~TaskUpdateAllSourceFiles() {

}

jrpc::ITask *TaskUpdateAllSourceFiles::run(jrpc::ITask *parent, bool initial) {
    runEnter(parent, initial);

    return runLeave(parent, initial);
}

}
}
