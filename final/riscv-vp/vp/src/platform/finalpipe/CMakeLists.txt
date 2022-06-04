file(GLOB_RECURSE HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_library(platform-finalpipe
ethernet.cpp
display.cpp
${HEADERS})

target_include_directories(platform-finalpipe PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(riscv-vp-finalpipe
        main.cpp)

target_link_libraries(riscv-vp-finalpipe rv32 platform-finalpipe platform-common gdb-mc ${Boost_LIBRARIES} ${SystemC_LIBRARIES} pthread)

INSTALL(TARGETS riscv-vp RUNTIME DESTINATION bin)
