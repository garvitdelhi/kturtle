#Cmake minimum required version
cmake_minimum_required(VERSION 2.8.12)

#project Name
project (kturtle)

find_package(ECM REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})


#locate Qt5
find_package(Qt5 REQUIRED)

# Locate KF5
find_package(KF5 REQUIRED)

include_directories (${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} ${QT5_INCLUDES})
add_definitions(-DQT_USE_FAST_CONCATENATION -DQT_USE_FAST_OPERATOR_PLUS)


add_subdirectory (doc)
add_subdirectory (src)
add_subdirectory (icons)

install(FILES kturtle.appdata.xml DESTINATION ${SHARE_INSTALL_PREFIX}/appdata/)
