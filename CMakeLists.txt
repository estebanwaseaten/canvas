cmake_minimum_required(VERSION 3.0.0)

project(canvas)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

#set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR})

#set(OpenGL_GL_PREFERENCE GLVND)
#set(OpenGL_GL_PREFERENCE LEGACY)
#find_package(Vulkan REQUIRED FATAL_ERROR)
#find_package(OpenGL REQUIRED)

set(HEADER_LIST
    "include/canvas/canvas.h"
    "include/canvas/element.h"
    "include/canvas/geometry.h"

    "src/debug.h"
    "src/canvasutils.h"
    "src/engineDumb.h"
)
set(SOURCE_LIST
    "src/canvas.cpp"
    "src/canvasutils.cpp"
    "src/engineDumb.cpp"
    "src/element.cpp"
    "src/geometry.cpp"
)

#defines the target:
add_library(${PROJECT_NAME} SHARED ${SOURCE_LIST} )

target_link_libraries(${PROJECT_NAME} drm)
#target_link_libraries (${PROJECT_NAME} ${VULKAN_LIB_LIST})

target_include_directories(${PROJECT_NAME}
    PUBLIC include
    PUBLIC /usr/include/libdrm          # public --> main prog sees it
    PRIVATE src
)

set_target_properties(${PROJECT_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR})
