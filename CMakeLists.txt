cmake_minimum_required(VERSION 3.2)

project("lc_audio" VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(BUILD_APP "Build test application" OFF)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

add_subdirectory(src)

if (BUILD_APP)
    add_subdirectory(config)
    set(OUT a.out)
    set(LINK_LIBS lc_audio)
    add_subdirectory(app)
else()
    install(TARGETS lc_audio DESTINATION lib)
endif()
