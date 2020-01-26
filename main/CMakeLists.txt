# main/CMakeLists.txt

file(GLOB_RECURSE MAIN_SOURCES *.cpp)

add_executable(${BIN_TARGET} ${MAIN_SOURCES} ${PROJECT_RCC_FILE})
target_link_libraries(${BIN_TARGET} ${PROJECT_NAME} ${CPPDB_LIBRARIES})
add_dependencies(${BIN_TARGET} ${LIB_TARGET})
set_target_properties(${BIN_TARGET} PROPERTIES OUTPUT_NAME ${PROJECT_NAME})
