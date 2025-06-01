1. submodule管理
使用git submodule add https://github.com/example/mylib.git lib/mylib 
添加指定的子模块 其中lib/mylib是子模块的本地目录地址
示例：git submodule update --init --recursive --depth 1 
这条命令通常用于克隆一个包含子模块的 Git 仓库后，快速初始化并更新所有子模块。
2. CMake的知识点
   1. set、unset：用于设置、取消设置变量。
   ```
    set(<variable> <value> [CACHE <type> <docstring> [FORCE]])
    # Set variable
    set(AUTHOR_NAME Farmer)
    set(AUTHOR "Farmer Li")
    set(AUTHOR Farmer\ Li)  包含空格要使用"\"转义
    set(SOURCE_FILES main.cpp utils.cpp config.h)  # 空格或分号分隔
    list(APPEND SOURCE_FILES test.cpp)  # 追加元素
    list(REMOVE_ITEM SOURCE_FILES config.h) # 删除元素
   ```
   调用方式为${AUTHOR_NAME}
   2. message：消息打印。第一个参数是消息级别，第二个是消息
   ```
    STATUS(普通状态)、WARNING、SEND_ERROR(打印错误，继续执行但最终会失败)、FATAL_ERROR(打印错误并立即终止配置)
    示例：message(FATAL_ERROR "Required file not found: ${CRITICAL_FILE}")

   ```
   3. option：配置选项
   ```
    option(BUILD_SHARED_LIBS "Build shared libraries" ON)
   ```
   4. 常用的CMake内置变量：
   ```
    CMAKE_SOURCE_DIR	        顶级 CMakeLists.txt 所在目录
    CMAKE_BINARY_DIR	        构建目录(cmake -B 指定的路径)
    CMAKE_CURRENT_SOURCE_DIR	当前处理的 CMakeLists.txt 所在目录
    CMAKE_CURRENT_BINARY_DIR	当前构建目标对应的二进制目录
    CMAKE_MODULE_PATH	指定CMake查找模块文件的路径
    CMAKE_SYSTEM_NAME	操作系统（Linux, Windows, Darwin）
    CMAKE_SYSTEM        表示当前的操作系统
    UNIX	            是否为 UNIX 系统（包括 Linux/macOS）
    WIN32	            是否为 Windows
    APPLE	            是否为 macOS/iOS
    CMAKE_CXX_COMPILER          指定 C++ 语言编译器的路径。
    CMAKE_CXX_COMPILER_ID	    编译器类型（GNU, Clang, MSVC）
    CMAKE_BUILD_TYPE	        构建类型（Debug, Release, RelWithDebInfo, MinSizeRel）
    CMAKE_CXX_FLAGS_DEBUG	    Debug 模式的编译选项（默认 -g）
    CMAKE_CXX_FLAGS_RELEASE	    Release 模式的编译选项（默认 -O3 -DNDEBUG）
    CMAKE_CXX_STANDARD	            C++ 标准（如 11, 14, 17, 20）
    CMAKE_CXX_STANDARD_REQUIRED	    是否强制要求指定标准（ON/OFF）
    CMAKE_POSITION_INDEPENDENT_CODE	是否生成位置无关代码（ON 用于动态库，默认值为 OFF） 
    CMAKE_ARCHIVE_OUTPUT_DIRECTORY  指定静态库（.a 或 .lib）的输出目录。
    CMAKE_LIBRARY_OUTPUT_DIRECTORY  指定共享库（.so、.dylib 或 .dll）的输出目录。
    CMAKE_RUNTIME_OUTPUT_DIRECTORY  指定可执行文件的输出目录。  
   ```
   5. add_library：创建一个库目标
   6. target_link_libraries：为目标添加链接库
   7. target_include_directories： 为目标添加包含目录
   8. find_package： 查找并加载外部包
   9. find_library 和 find_path: 分别用于查找库文件和包含目录
   10. add_subdirectory：将指定子目录添加到当前构建过程中。
   11. if() - endif()
   12. target_compile_definitions：为指定目标添加编译定义，编译定义本质上就是在编译时传递给编译器的宏定义