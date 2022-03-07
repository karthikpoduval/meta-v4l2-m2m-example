cmake_minimum_required(VERSION 3.0)

project(m2m-scaler-test)

include(FindPkgConfig)

pkg_check_modules(LIBCAMERA REQUIRED libcamera) 

include_directories(${LIBCAMERA_INCLUDE_DIRS})

add_executable(${PROJECT_NAME} m2m-example.cpp)

target_link_libraries(${PROJECT_NAME} ${LIBCAMERA_LIBRARIES})

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)

