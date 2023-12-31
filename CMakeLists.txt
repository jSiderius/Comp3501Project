cmake_minimum_required(VERSION 2.6)

# Name of project
set(PROJ_NAME TextureMappingDemo)
project(${PROJ_NAME} C CXX)

# Specify project files: header files and source files
set(HDRS
    camera.h game.h resource.h resource_manager.h scene_graph.h scene_node.h title_screen.h player.h orb.h model_loader.h
)
 
set(SRCS
    title_screen.cpp orb.cpp camera.cpp game.cpp main.cpp player.cpp resource.cpp resource_manager.cpp scene_graph.cpp scene_node.cpp lit_fp.glsl lit_vp.glsl textured_material_fp.glsl textured_material_vp.glsl particle1_fp.glsl particle1_gp.glsl particle1_vp.glsl particle2_fp.glsl particle2_gp.glsl particle2_vp.glsl particle3_fp.glsl particle3_gp.glsl particle3_vp.glsl
)

# Add executable based on the source files
add_executable(${PROJ_NAME} ${HDRS} ${SRCS})

# Require OpenGL library
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})
target_link_libraries(${PROJ_NAME} ${OPENGL_gl_LIBRARY})

# Other libraries needed
set(LIBRARY_PATH D:/Development/Comp\ 3501/Libraries)
include_directories(${LIBRARY_PATH}/include)
if(NOT WIN32)
    find_library(GLEW_LIBRARY GLEW)
    find_library(GLFW_LIBRARY glfw)
    find_library(SOIL_LIBRARY SOIL)
elseif(WIN32)
    find_library(GLEW_LIBRARY glew32s HINTS ${LIBRARY_PATH}/lib)
    find_library(GLFW_LIBRARY glfw3 HINTS ${LIBRARY_PATH}/lib)
    find_library(SOIL_LIBRARY SOIL HINTS ${LIBRARY_PATH}/lib)
endif(NOT WIN32)
target_link_libraries(${PROJ_NAME} ${GLEW_LIBRARY})
target_link_libraries(${PROJ_NAME} ${GLFW_LIBRARY})
target_link_libraries(${PROJ_NAME} ${SOIL_LIBRARY})

# The rules here are specific to Windows Systems
if(WIN32)
    # Avoid ZERO_CHECK target in Visual Studio
    set(CMAKE_SUPPRESS_REGENERATION TRUE)
 
    # This will use the proper libraries in debug mode in Visual Studio
    set_target_properties(${PROJ_NAME} PROPERTIES DEBUG_POSTFIX _d)
endif(WIN32)
