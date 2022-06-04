file(GLOB_RECURSE HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_library(platform-final32
ethernet.cpp
display.cpp
${HEADERS})

target_include_directories(platform-final32 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(riscv-vp-final32
        main.cpp)

target_link_libraries(riscv-vp-final32 rv32 platform-final32 platform-common gdb-mc ${Boost_LIBRARIES} ${SystemC_LIBRARIES} pthread)

INSTALL(TARGETS riscv-vp RUNTIME DESTINATION bin)
