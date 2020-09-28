cmake_minimum_required(VERSION 3.17)
project(FOSSACommsInterpreter CXX)

set(CMAKE_CXX_STANDARD 11)
# set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} -O0")


#########################################
# Add the FOSSACommsInterpreter sources #
#########################################
file(GLOB_RECURSE ALL_SRCS
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.*")

#######################################
# Add the FOSSA-Comms library sources #
#######################################
file(GLOB_RECURSE FOSSACOMMSLIB_SRCS
        "${CMAKE_CURRENT_SOURCE_DIR}/libs/FOSSA-Comms/*.c*")
file(GLOB_RECURSE FOSSACOMMSLIB_HEADERS
        "${CMAKE_CURRENT_SOURCE_DIR}/libs/FOSSA-Comms/*.h*")


####################################
# Add the tiny-aes library sources #
####################################
file(GLOB_RECURSE TINYAES_SRCS
        "${CMAKE_CURRENT_SOURCE_DIR}/libs/tiny-aes/*.c*")
file(GLOB_RECURSE TINYAES_HEADERS
        "${CMAKE_CURRENT_SOURCE_DIR}/libs/tiny-aes/*.h*")

add_executable(FOSSACommsInterpreter
        main.cpp
        ${ALL_SRCS}
        ${FOSSACOMMSLIB_SRCS} ${FOSSACOMMSLIB_HEADERS}
        ${TINYAES_SRCS} ${TINYAES_HEADERS})

#######################################
# Add the library include directories #
#######################################
target_include_directories(FOSSACommsInterpreter PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/libs/tiny-aes/")
target_include_directories(FOSSACommsInterpreter PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/libs/FOSSA-Comms/")
