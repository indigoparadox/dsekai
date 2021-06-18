
cmake_minimum_required(VERSION 3.10)

project(dsekai)

set(DSEKAI_DEFINES)

set(DSEKAI_C_FILES
   src/main.c
   src/graphics/dosg.c
   src/tilemap.c
   src/input/dosi.c
   src/graphics.c
   src/mobile.c
   src/item.c
   src/window.c)

add_executable(dsekai ${DSEKAI_C_FILES})

if(CMAKE_C_COMPILER_ID MATCHES "wcc")
   list(APPEND DSEKAI_DEFINES
      USE_DOS
      USE_LOOKUPS)
   target_compile_definitions(dsekai PUBLIC ${DSEKAI_DEFINES})
endif()

