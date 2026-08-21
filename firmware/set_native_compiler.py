import os
from SCons.Script import DefaultEnvironment

env = DefaultEnvironment()

compiler_path = r"C:\msys64\mingw64\bin"

env.PrependENVPath("PATH", compiler_path)
env.Replace(CC=os.path.join(compiler_path, "gcc.exe"))
env.Replace(CXX=os.path.join(compiler_path, "g++.exe"))