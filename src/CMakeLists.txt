set(TARGET_NAME ferrugo-md)

add_executable(${TARGET_NAME} main.cpp)

target_include_directories(
    ${TARGET_NAME}
    PUBLIC
    "${PROJECT_SOURCE_DIR}/include"
    "${ferrugo-core_SOURCE_DIR}/include")