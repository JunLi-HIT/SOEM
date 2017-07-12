project(xeno_test)

message(STATUS "CMAKE_C_FLAGS             = " ${CMAKE_C_FLAGS})
message(STATUS "CMAKE_SHARED_LINKER_FLAGS = " ${CMAKE_SHARED_LINKER_FLAGS})
message(STATUS "CMAKE_STATIC_LINKER_FLAGS = " ${CMAKE_STATIC_LINKER_FLAGS})
message(STATUS "CMAKE_EXE_LINKER_FLAGS    = " ${CMAKE_EXE_LINKER_FLAGS})


add_executable(raw-ethernet raw-ethernet.c)
target_link_libraries(raw-ethernet ${Xenomai_POSIX_LIBRARIES})

add_executable(xeno_test xeno_main.c)
target_link_libraries(xeno_test soem)

add_executable(thesis thesis.c)
target_link_libraries(thesis soem)

# install(TARGETS xeno_test DESTINATION bin)
