# Configure with [fxsdk build-fx] or [fxsdk build-cg], which provide the
# toolchain file and module path of the fxSDK

cmake_minimum_required(VERSION 3.15)
project(fnac VERSION 1.0.0 LANGUAGES CXX)
enable_language(C)
include(GenerateG1A)
include(GenerateG3A)
include(GenerateHH2Bin)
include(Fxconv)
find_package(Gint 2.9 REQUIRED)

set(SOURCES
  src/main.cpp
  # ...
)
set(ASSETS_fx
  # ...
)
set(ASSETS_cg
  # ...
)

fxconv_declare_assets(${ASSETS} ${ASSETS_fx} ${ASSETS_cg} WITH_METADATA)

add_library(tinymt32 STATIC
  ${CMAKE_CURRENT_SOURCE_DIR}/src/tinymt32/tinymt32.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/tinymt32/rand.c
)
target_include_directories(tinymt32 PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/src/tinymt32
)

add_executable(fnac ${SOURCES} ${ASSETS} ${ASSETS_${FXSDK_PLATFORM}})
target_compile_options(fnac PRIVATE -Wall -Wextra -Os -std=c++23)
target_link_libraries(fnac Gint::Gint tinymt32 -lstdc++)

if("${FXSDK_PLATFORM_LONG}" STREQUAL fx9860G)
  generate_g1a(TARGET fnac OUTPUT "fnac.g1a"
    NAME "fnac" ICON assets-fx/icon.png)
elseif("${FXSDK_PLATFORM_LONG}" STREQUAL fxCP)
  generate_hh2_bin(TARGET fnac OUTPUT "fnac-hh2.bin")
endif()
