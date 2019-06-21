cmake_minimum_required(VERSION 3.0)

set(CMAKE_CXX_STANDARD 17)

project(dynamical)

include_directories("./src")
include_directories("./3rdparty")
file(GLOB_RECURSE sources main.cpp src/*.cpp src/*.h)

add_executable(dynamical ${sources})

find_package(Vulkan REQUIRED)
target_link_libraries(dynamical PRIVATE Vulkan::Vulkan)

find_package(SDL2 CONFIG REQUIRED)
target_link_libraries(dynamical PRIVATE SDL2::SDL2 SDL2::SDL2main)
include_directories(${SDL2_INCLUDE_DIRS})






set(GLSL_VALIDATOR "glslangValidator")

file(GLOB_RECURSE GLSL_SOURCE_FILES "src/renderer/shaders/*.glsl")

foreach(GLSL ${GLSL_SOURCE_FILES})
  get_filename_component(FILE_NAME ${GLSL} NAME)
  set(SPIRV "${PROJECT_BINARY_DIR}/shaders/${FILE_NAME}.spv")
  add_custom_command(
    OUTPUT ${SPIRV}
    COMMAND ${CMAKE_COMMAND} -E make_directory "${PROJECT_BINARY_DIR}/shaders/"
    COMMAND ${GLSL_VALIDATOR} -V --target-env vulkan1.1 ${GLSL} -o ${SPIRV}
    DEPENDS ${GLSL})
  list(APPEND SPIRV_BINARY_FILES ${SPIRV})
endforeach(GLSL)

add_custom_target(
    shaders 
    DEPENDS ${SPIRV_BINARY_FILES}
    )

add_dependencies(dynamical shaders)

file(
    COPY resources/
    DESTINATION ${PROJECT_BINARY_DIR}/resources/
    )

