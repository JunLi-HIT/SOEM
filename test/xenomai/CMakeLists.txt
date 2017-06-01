project(xeno_test)

add_executable(xeno_test xeno_main.c)
target_link_libraries(xeno_test soem)
target_link_libraries(xeno_test pthread rt ${Xenomai_RTDM_LIBRARIES} ${Xenomai_NATIVE_LIBRARIES})

execute_process(COMMAND xeno-config --skin=posix --cflags OUTPUT_VARIABLE XENO_CFLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND xeno-config --skin=posix --ldflags OUTPUT_VARIABLE XENO_LDFLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${XENO_CFLAGS}")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${XENO_LDFLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${XENO_LDFLAGS}")


message(STATUS "CMAKE_C_FLAGS             = " ${CMAKE_C_FLAGS})
message(STATUS "CMAKE_SHARED_LINKER_FLAGS = " ${CMAKE_SHARED_LINKER_FLAGS})
message(STATUS "CMAKE_EXE_LINKER_FLAGS    = " ${CMAKE_EXE_LINKER_FLAGS})


add_executable(raw-ethernet raw-ethernet.c)


install(TARGETS xeno_test DESTINATION bin)
