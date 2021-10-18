'''
A tool for generating engine's header file.
'''
import os
import sys

# Declaring this in the global scope so that we can reuse this script for another project.
ENGINE_FOLDER = "nexus"
ENGINE_HEADER_FILENAME = "nexus.h"
ENGINE_INCLUDE_GUARD = "__NEXUS_HEADER__"
IGNORE_LIST = {
    "thirdparty",
    ENGINE_HEADER_FILENAME
}

def isHeader(file):
    '''Checking whether the specified file is a header file.'''
    headerExts = {
        ".h", ".hpp"
    }
    return len(list(filter(lambda ext, _file = file: ext in _file, headerExts))) > 0

def main():
    scriptRoot, _ = os.path.split(os.path.abspath(sys.argv[0]))
    engineRoot = os.path.abspath(os.path.join(scriptRoot, "..", ENGINE_FOLDER))
    headerList = set()

    for _root, _dirs, _files in os.walk(engineRoot):
        for _file in _files:
            ignored = list(filter(lambda path, ignoredPath = os.path.join(_root, _file): path in ignoredPath, IGNORE_LIST))
            if len(ignored) > 0:
                continue
            if isHeader(_file):
                headerList.add(os.path.join(_root, _file))

    fileContent  = "/** This file is auto generated and MUST NOT be manually modified **/\n"
    fileContent += f"#ifndef {ENGINE_INCLUDE_GUARD}\n#define {ENGINE_INCLUDE_GUARD}\n\n"
    sortedHeaders = list(headerList)
    sortedHeaders.sort()

    engineRootPathLen = len(engineRoot) + 1
    for headerFullPath in sortedHeaders:
        if engineRootPathLen >= len(headerFullPath):
            continue

        header = headerFullPath[engineRootPathLen:]
        # print(header)
        fileContent += f"#include \"{header}\"\n"

    fileContent += f"\n#endif // {ENGINE_INCLUDE_GUARD}"

    engineHeaderPath = os.path.join(engineRoot, ENGINE_HEADER_FILENAME)
    with open(engineHeaderPath, "w") as outFile:
        outFile.write(fileContent)

if __name__ == '__main__':
    main()
