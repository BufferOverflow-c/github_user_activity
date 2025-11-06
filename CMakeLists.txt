cmake_minimum_required(VERSION 3.20)
project(github_activity)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(nlohmann_json 3.12.0 REQUIRED)

file(GLOB_RECURSE MY_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

list(FILTER MY_SOURCES EXCLUDE REGEX ".*/curlpp-0.8.1/examples/.*")

add_executable(${CMAKE_PROJECT_NAME} ${MY_SOURCES})


target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/src/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/vendor/curlpp-0.8.1/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/vendor/curlpp-0.8.1/include/curlpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/vendor/curlpp-0.8.1/include/utilspp"
)

add_subdirectory(src/vendor/curlpp-0.8.1)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE
    curlpp curl
    nlohmann_json::nlohmann_json
)
