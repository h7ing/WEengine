cmake_minimum_required(VERSION 3.25.0)
project(hoka C)
set(CMAKE_C_STANDARD 17)

if(MSVC)
  set(CMAKE_C_FLAGS "/utf-8")
endif()

include_directories(game)
include_directories(libs/SDL3/include libs/glew/include)
include_directories(modules/cglm modules/cgltf modules/stb)

link_directories(libs/SDL3/lib libs/glew/lib)

file(GLOB_RECURSE game_src "${CMAKE_CURRENT_SOURCE_DIR}/game/*.c")

add_executable(hoka ${game_src})

# target_compile_definitions(hoka PRIVATE GL_GLEXT_PROTOTYPES)

target_link_libraries(hoka SDL3)

# windows
if(MSVC)
  # add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/source-charset:utf-8>")
  target_link_libraries(hoka opengl32 glew32)
endif()

# mac
if (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
  target_link_libraries(hoka "-framework OpenGL")
  target_link_libraries(hoka GLEW)
endif()
