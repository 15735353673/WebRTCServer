cmake_minimum_required(VERSION 3.5)

project(xrtcserver)


set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fno-pie")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-pie")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -no-pie")


add_definitions("-g -pipe -W -Wall -fPIC -std=gnu++14 -DWEBRTC_POSIX
    -DWEBRTC_LINUX")

include_directories(
    "./src"
    "./third_party/include"
	"../rtcbase/src"
)

link_directories(
    "./third_party/lib"
	"../rtcbase/out"
)

file(GLOB all_src
    "./src/*.cpp"
    "./src/base/*.cpp"
	"./src/server/*.cpp"
	"./src/stream/*.cpp"
	"./src/pc/*.cpp"
	"./src/ice/*.cpp"
	"./src/module/rtp_rtcp/*.cpp"

)

add_executable(xrtcserver ${all_src})

target_link_libraries(xrtcserver libyaml-cpp.so librtcbase.a 
	libabsl_strings.a libabsl_throw_delegate.a libev.a libjsoncpp.a
	libabsl_bad_optional_access.a libssl.a libcrypto.a libsrtp2.a
	-lpthread -ldl
)



