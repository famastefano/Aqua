if(NOT WIN32)
    return()
endif()

set(CMAKE_C_COMPILER "clang.exe")
set(CMAKE_CXX_COMPILER "clang++.exe")

list(APPEND CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES
    "C:/Program Files (x86)/Windows Kits/10/Include/10.0.26100.0/ucrt"
    "C:/Program Files (x86)/Windows Kits/10/Include/10.0.26100.0/um"
    "C:/Program Files (x86)/Windows Kits/10/Include/10.0.26100.0/shared"
    "C:/Program Files/Microsoft Visual Studio/18/Community/VC/Tools/MSVC/14.50.35717/include"
)
link_directories(
    "C:/Program Files (x86)/Windows Kits/10/Lib/10.0.26100.0/ucrt/x64"
    "C:/Program Files (x86)/Windows Kits/10/Lib/10.0.26100.0/um/x64"
    "C:/Program Files/Microsoft Visual Studio/18/Community/VC/Tools/MSVC/14.50.35717/lib/x64"
)