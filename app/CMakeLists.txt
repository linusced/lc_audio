add_executable(${OUT} main.cpp)
target_link_libraries(${OUT} PUBLIC ${LINK_LIBS})
target_include_directories(${OUT} PUBLIC "${CMAKE_BINARY_DIR}/config/include/")
