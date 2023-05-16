/*
 * SourceFileFinder.cpp
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
#include "SourceFileFinder.h"
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>


namespace zsp {
namespace ls {


SourceFileFinder::SourceFileFinder() {

}

SourceFileFinder::~SourceFileFinder() {

}

void SourceFileFinder::find(
    SourceFileCollection        *collection,
    const std::string           &root) {
    m_collection = collection;
    find(root);
}

void SourceFileFinder::find(const std::string &path) {
    DIR *dir;
    struct stat stat_path, stat_entry;
    struct dirent *entry;

    stat(path.c_str(), &stat_path);

    // If the path isn't an existing directory, bail
    if (S_ISDIR(stat_path.st_mode) == 0) {
        return;
    }

    if ((dir=opendir(path.c_str())) == NULL) {
        // Bail, since we can't open the directory
        // for reading
        return;
    }

    // Iterate through directories
    while ((entry = readdir(dir)) != NULL) {
        if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, "..")) {
            continue;
        }

        std::string fullpath = path + "/";
        fullpath += entry->d_name;

        stat(fullpath.c_str(), &stat_path);

        if (S_ISREG(stat_path.st_mode) != 0) {
            const char *ep = strrchr(entry->d_name, '.');

            // TODO: does the LS protocol provide us extensions?
            if (ep && !strcmp(ep, ".pss")) {
                // Have a PSS file

                std::string uri = "file://" + fullpath;

                int64_t timestamp = stat_path.st_mtim.tv_sec * 1000;
                timestamp += stat_path.st_mtim.tv_nsec / 1000;

                SourceFileDataUP file(new SourceFileData(uri, timestamp));
                m_collection->addFile(file);
            }
        } else if (S_ISDIR(stat_path.st_mode) != 0) {
            find(fullpath);
        }
    }

    closedir(dir);
}

}
}