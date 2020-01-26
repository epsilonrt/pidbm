# lib/CMakeLists.txt
#project(${LIB_TARGET})

file(GLOB LIB_SOURCES *.cpp)

add_library(${LIB_TARGET} SHARED ${LIB_SOURCES} ${PROJECT_RCC_FILE})
target_link_libraries(${LIB_TARGET} ${CPPDB_LIBRARIES})
