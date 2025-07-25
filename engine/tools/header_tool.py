import os
from pathlib import Path

def find_header_guard(header):
    with open(header, 'r') as aFile:
        is_comment = False
        for line in aFile:
            line = line.strip()
            
            if line == '':
                continue
            
            line_length = len(line)
            if line_length >= 2:
                if line[:2] == '//':
                    continue
                elif line[:2] == '/*':
                    is_comment = True
                
                if is_comment:
                    if line[:line_length-2] == '*/':
                        is_comment = False
                    continue
            tokens = line.split()
            if len(tokens) == 2:
                if tokens[0] == '#pragma' and tokens[1] == 'once':
                    return
                else:
                    raise RuntimeError(f"Header guard not found. File={header}")


script_path = Path(__file__).resolve()
project_root = os.path.abspath(os.path.join(script_path, '..', '..', '..'))
engine_header_dir = os.path.abspath(os.path.join(project_root, 'engine', 'include', 'nexus'))
engine_header_path_len = len(engine_header_dir)

generated_header = 'Nexus.h'
header_list = []
# A list of header files that won't be automatically added to Nexus.h but are still
# subject for header-guard check.
ignored_headers = [
    'NxsDefine.h',
    'NxsCommon.h',
]

print(f"Checking header-guard...")
for root, _, files in os.walk(engine_header_dir):
    for file_name in files:
        if file_name != generated_header:
            find_header_guard(os.path.join(root, file_name))

        if file_name not in ignored_headers:
            header_full_path = os.path.join(root, file_name)[engine_header_path_len+1:]
            header_list.append(header_full_path)

print(f"Generating Nexus.h header...")
with open(os.path.join(engine_header_dir, 'Nexus.h'), 'w') as outFile:
    outFile.write('// This is an auto-generated file. Please DO NOT manually edit!!\n\n')
    outFile.write("#pragma once\n\n")
    outFile.write("#include \"NxsCommon.h\"\n\n")
    for header_file in header_list:
        header_file = str(header_file).replace(os.path.sep, '/')
        include_path = f"#include \"{header_file}\""
        # print(f"{include_path}")
        outFile.write(f"{include_path}\n")
print(f"DONE!!")