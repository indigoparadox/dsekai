
cmake_minimum_required(VERSION 3.10)

set(CMAKE_C_STANDARD 90)
set(CMAKE_C_STANDARD_REQUIRED TRUE)

project(dsekai)

set(DSEKAI_DEFINES)

set(DSEKAI_SOURCE_FILES
   "src/main.c"
   "src/tilemap.c"
   "src/mobile.c"
   "src/item.c"
   "src/window.c"
   "src/script.c"
   "src/control.c"
   "src/topdown.c"
   "src/pov.c"
   "src/asn.c"
   "src/title.c")

if(NOT DEFINED TILEMAP)
   set(DSEKAI_DEFINES ${DSEKAI_DEFINES} -DTILEMAP_FMT_JSON)
   list(APPEND DSEKAI_SOURCE_FILES "src/json.c" "src/tmjson.c")
endif()

set(DSEKAI_TILES_RES "16x16x4")

set(UNILAYER_DEFINES
   -DSCREEN_W=320
   -DSCREEN_H=200
   -DNO_RESEXT
   -DDEPTH_SPEC=\"${DSEKAI_TILES_RES}\"
   -DUNILAYER_PROJECT_NAME=\"dsekai\"
   -DASSETS_PATH=\"assets\")

file(GLOB UNILAYER_ASSET_FILES
   assets/${DSEKAI_TILES_RES}/*.bmp)

add_subdirectory(unilayer)

add_executable(dsekai ${DSEKAI_SOURCE_FILES})
target_link_libraries(dsekai unilayer)
target_include_directories(dsekai PUBLIC unilayer/src)
target_compile_definitions(dsekai PUBLIC ${UNILAYER_DEFINES} ${DSEKAI_DEFINES})

