/*
 * TestSoureFileFinder.cpp
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
#include "TestSourceFileFinder.h"
#include "SourceFileFinder.h"


namespace zsp {
namespace ls {


TestSourceFileFinder::TestSourceFileFinder() {

}

TestSourceFileFinder::~TestSourceFileFinder() {

}

TEST_F(TestSourceFileFinder, single_level) {
    createTree({
        {"file1.pss", "Dummy content"},
        {"file2.pss", "Dummy content"},
        {"file3.pss", "Dummy content"},
    });

    SourceFileFinderUP finder(new SourceFileFinder());
    SourceFileCollectionUP collection(new SourceFileCollection());

    finder->find(
        collection.get(),
        m_testdir);

    std::string root_uri = "file://" + m_testdir;
    ASSERT_TRUE(collection->hasFile(root_uri + "/file1.pss"));
    ASSERT_TRUE(collection->hasFile(root_uri + "/file2.pss"));
    ASSERT_TRUE(collection->hasFile(root_uri + "/file3.pss"));
}

}
}
