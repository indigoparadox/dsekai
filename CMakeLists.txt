
cmake_minimum_required(VERSION 3.10)

set(CMAKE_C_STANDARD 90)
set(CMAKE_C_STANDARD_REQUIRED TRUE)

project(dsekai)

set(DSEKAI_C_FILES
   src/tilemap.c
   src/mobile.c
   src/item.c
   src/window.c
   src/script.c
   unilayer/dio.c
   src/control.c
   src/topdown.c)

if(UNIX)
   add_executable(dsekai ${DSEKAI_C_FILES})
   target_compile_definitions(dsekai PUBLIC ${DSEKAI_DEFINES})
   target_link_libraries(dsekai PUBLIC ${DSEKAI_LINK_EXTERNAL})
endif()

