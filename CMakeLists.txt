cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(PROJECT_NAME KeyframeEditor)
set(RUNNER_NAME  OneTimeRunner)

project(${PROJECT_NAME} VERSION 1.0.0)

file(GLOB_RECURSE SOURCES 
    dllmain.cpp
    src/*.cpp
)

set(WIN32 ON)
add_library(${PROJECT_NAME} SHARED ${SOURCES})

set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "")

option(BUILD_SHARED_LIBS "" ON)
add_subdirectory(sub/MinHook)
target_link_libraries(${PROJECT_NAME} sub/MinHook/Release/minhook.x32)

target_include_directories(
    ${PROJECT_NAME} PRIVATE
    sub/cocos/cocos2dx
    sub/cocos/cocos2dx/include
    sub/cocos/cocos2dx/kazmath/include
    sub/cocos/cocos2dx/platform/third_party/win32/OGLES
    sub/cocos/cocos2dx/platform/win32
    sub/cocos/cocos2dx/support/zip_support
    sub/cocos/extensions
    sub/MinHook/include
    sub/gd/include
    sub/gd
)

target_link_libraries(
    ${PROJECT_NAME}
    ${CMAKE_SOURCE_DIR}/sub/cocos/cocos2dx/libcocos2d.lib
    ${CMAKE_SOURCE_DIR}/sub/cocos/extensions/libExtensions.lib
)


# runner

if (COMPILE_RUNNER)

    add_executable(${RUNNER_NAME} runner.cpp)

    set_source_files_properties( runner.cpp PROPERTIES LANGUAGE CXX )
    set_source_files_properties( runner.cpp PROPERTIES CXX_STANDARD 17 )
    set_source_files_properties( runner.cpp PROPERTIES CXX_STANDARD_REQUIRED ON )

    target_link_libraries(${RUNNER_NAME} WtsApi32)

    add_definitions(${PROJECT_SOURCE_DIR}/runner.cpp "-DNOMSGBOX")

endif()

