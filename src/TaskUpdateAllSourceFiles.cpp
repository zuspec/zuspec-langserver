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
    jrpc::ITaskGroup            *group,
    Context                     *ctxt,
    SourceFileCollection        *src_files) :
        TaskBase(group), m_ctxt(ctxt), m_src_files(src_files) {

}

TaskUpdateAllSourceFiles::~TaskUpdateAllSourceFiles() {

}

jrpc::TaskStatus TaskUpdateAllSourceFiles::run() {

}

}
}
