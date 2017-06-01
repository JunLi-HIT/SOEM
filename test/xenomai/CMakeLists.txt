project(xeno_test)

#execute_process(COMMAND xeno-config --skin=posix --cflags OUTPUT_VARIABLE XENO_CFLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
#execute_process(COMMAND xeno-config --skin=posix --ldflags OUTPUT_VARIABLE XENO_LDFLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${XENO_CFLAGS}")
#set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${XENO_LDFLAGS}")
#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${XENO_LDFLAGS}")


message(STATUS "CMAKE_C_FLAGS             = " ${CMAKE_C_FLAGS})
message(STATUS "CMAKE_SHARED_LINKER_FLAGS = " ${CMAKE_SHARED_LINKER_FLAGS})
message(STATUS "CMAKE_STATIC_LINKER_FLAGS = " ${CMAKE_STATIC_LINKER_FLAGS})
message(STATUS "CMAKE_EXE_LINKER_FLAGS    = " ${CMAKE_EXE_LINKER_FLAGS})


add_executable(raw-ethernet raw-ethernet.c)
target_link_libraries(raw-ethernet ${Xenomai_POSIX_LIBRARIES})


# add_executable(xeno_test xeno_main.c
#   ${CMAKE_SOURCE_DIR}/oshw/xenomai/nicdrv.c
#   ${CMAKE_SOURCE_DIR}/oshw/xenomai/oshw.c
#   ${CMAKE_SOURCE_DIR}/osal/xenomai/osal.c
#   ${CMAKE_SOURCE_DIR}/soem/ethercatbase.c
#   ${CMAKE_SOURCE_DIR}/soem/ethercatcoe.c
#   ${CMAKE_SOURCE_DIR}/soem/ethercatconfig.c
#   ${CMAKE_SOURCE_DIR}/soem/ethercatdc.c
#   ${CMAKE_SOURCE_DIR}/soem/ethercatfoe.c
#   ${CMAKE_SOURCE_DIR}/soem/ethercatmain.c
#   ${CMAKE_SOURCE_DIR}/soem/ethercatprint.c
#   ${CMAKE_SOURCE_DIR}/soem/ethercatsoe.c)

add_executable(xeno_test xeno_main.c)
target_link_libraries(xeno_test soem)


# install(TARGETS xeno_test DESTINATION bin)
