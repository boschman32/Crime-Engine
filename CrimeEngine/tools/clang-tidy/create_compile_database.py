import os, inspect
import json
from pathlib import Path

def main():
  
  #include path of visual studio
  vs_include_path = "C:\\Program Files (x86)\\Microsoft Visual Studio\\2017\\Community\\VC\\Tools\\MSVC\\14.16.27023\\include"

  #Where is the project located     
  root_dir = os.path.abspath("./../../../")

  #Find sources files inside the root directory
  source_files = []
  source_files = get_source_files(root_dir, source_files, ["libs", "UnitTesting", "Examples", "NVN", "NX", "Switch"])

  #Library includes
  lib_includes = [
    "glm", 
    "dx12", 
    "FMOD\\inc", 
    "lua-5.3.5\\include", 
    "sol2\\include",
    "spdlog\\include", 
    "tmxlite\\include",
    "eventpp\\include",
	  "Optick\\include"
    ]
  
  #Add the normal include folders under -I to say check for errors on non-system headers as well.
  includes = ' -I \"' + root_dir + '\\CrimeEngine\\include\"'
  includes += ' -I \"' + root_dir + '\\StreamlinedMastermind\\include\"'
  includes += ' -isystem \"' + root_dir + '\\CrimeEngine\\libs\" '

  #Gather any library paths that are included in the libIncludes list
  includes = get_includes(root_dir, includes, lib_includes, ["Examples", "UnitTesting"])
  
  #Add the visual studio path as last for stl stuff.
  includes += ' -isystem \"' + vs_include_path + '\"'

  define_list = [
    "_DEBUG", 
    "_MT",
    "_DLL",
    "CE_DEBUG",
    "PLATFORM_WINDOWS",
    "_MBCS"
  ]
  extra_list = [
    "-std=c++14",
    "-stdlib=libc++",
    "-lstdc++fs"
  ]

  create_compilation_database(source_files, includes, root_dir, define_list, extra_list)

def create_compilation_database(source_files : list, includes : str, root_dir : str, define_list : list, extra_list : list):
  database = []

  #Command line to run clang-tidy
  clang_tool_command = 'clang-tool -fms-extensions -fms-compatibility -fms-compatibility-version=19'

  defines = ""
  for d in define_list:
    defines += " -D " + d

  extras = ""
  for e in extra_list:
    extras += " " + e

  #Build up the database
  for s in source_files:
    database.append(
    {
      "directory": root_dir,
      "command": clang_tool_command + includes + defines + extras + ' \"' + s + '"',
      "file": s
    })

  #Write to json file.
  with open("compile_commands.json", "w+") as database_file:
    json.dump(database, database_file, indent=4)
    database_file.close()
      
def get_source_files(dir : str, source_files : list, exclude : list):
  for f in os.scandir(dir):
    #If we found directory go deeper to find files.
    if f.is_dir():
      source_files = get_source_files(f.path, source_files, exclude)
      #Found a file look if the path is only inside the given folders or if the file path has the needed path.
    elif f.is_file():
      if f.path.endswith(".cpp") and all(e not in f.path for e in exclude):
        source_files.append(f.path)
  return source_files

def get_includes(dir : str, includes : str, libs : list, excludes : list):
  for f in os.scandir(dir):
    if f.is_dir():
      if all(e not in f.path for e in excludes) and any(l in f.path for l in libs):
        includes += ' -isystem \"' + f.path + '\" '
      else:
        includes = get_includes(f.path, includes, libs, excludes)
  return includes

if __name__ == '__main__':
  main()