# CMakeList.txt: CMakeProject1 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#
project(CMakeProject1)
find_library(SHLWAPI_LIBRARY shlwapi)
include_directories(D:/envirmentSupport/vcpkg/vcpkg/installed/x64-windows-static/include)
link_directories(D:/envirmentSupport/vcpkg/vcpkg/installed/x64-windows-static/lib)
# 将源代码添加到此项目的可执行文件。
find_package(CURL)
add_executable (CMakeProject1 "CMakeProject1.cpp" "CMakeProject1.h" "Win32FSHook.cpp" "Win32FSHook.h" "WatchData.cpp" "WatchData.h" "objToUpLoadOrSave.cpp" "objToUpLoadOrSave.h" "tools.cpp" "tools.h" "DeviceNotify.cpp" "DeviceNotify.h")
target_link_libraries(CMakeProject1   libcurl.lib  libcrypto.lib Ws2_32.Lib zlib  Wldap32.Lib Crypt32.Lib shlwapi)
set_target_properties(CMakeProject1 PROPERTIES OUTPUT_NAME "fileDriver")
if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET CMakeProject1 PROPERTY CXX_STANDARD 20)
endif()

# TODO: 如有需要，请添加测试并安装目标。
