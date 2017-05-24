
set(SOURCES dv_test.c)
add_executable(dv_test ${SOURCES})
target_link_libraries(dv_test soem)

if(WIN32)
   target_link_libraries(dv_test wpcap.lib Packet.lib Ws2_32.lib Winmm.lib)
elseif(UNIX)
   target_link_libraries(dv_test pthread rt)
endif()

install(TARGETS dv_test DESTINATION bin)
