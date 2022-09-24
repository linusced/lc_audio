set(FILES
    source.cpp filters.cpp file.cpp engine.cpp engine_thread.cpp buffer.cpp beats.cpp)

add_library(lc_audio STATIC ${FILES})
target_link_libraries(lc_audio PUBLIC ${CONAN_LIBS_OPENAL} ${CONAN_LIBS_LIBSAMPLERATE}) 
target_include_directories(lc_audio PUBLIC include/)
