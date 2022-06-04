file(GLOB_RECURSE HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_library(platform-finalpara
ethernet.cpp
display.cpp
${HEADERS})

target_include_directories(platform-finalpara PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(riscv-vp-finalpara
        main.cpp)

target_link_libraries(riscv-vp-finalpara rv32 platform-finalpara platform-common gdb-mc ${Boost_LIBRARIES} ${SystemC_LIBRARIES} pthread)

INSTALL(TARGETS riscv-vp RUNTIME DESTINATION bin)
