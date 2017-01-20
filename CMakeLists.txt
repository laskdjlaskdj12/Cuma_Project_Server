#
# Makefile
# Cuma_Project
# Cuma_Server
#
# Create by laskdjlaskdj12@gmail.com.
# Copyright (c) 2016 InhoeKu LP. All right Reserved.
#
# LICENCED by JSONCPP (C) 2016 by cdunn2001 All right Reserved.
#


cmake_minimum_required (VERSION 2.6)
project (CUMA_SERVER_REMAKE)

# 소스들이 들어있는 디렉토리
add_subdirectory (Cuma_Server_Remake)

# jsoncpp header include
include_directories ("${PROJECT_SOURCE_DIR}/dist")

# boost header include
include_directories ("${PROJECT_SOURCE_DIR}/boost_1_62_0/include")

#로그
add_library (Log_obj SHARED "${PROJECT_SOURCE_DIR}/Cuma_Server_Remake/Log.cpp")

# 프레임
add_library (Frame_obj SHARED "${PROJECT_SOURCE_DIR}/Cuma_Server_Remake/Cuma_Frame.cpp")

#소켓
add_library (Socket_obj SHARED "${PROJECT_SOURCE_DIR}/Cuma_Server_Remake/Cuma_socket.cpp")
target_link_libraries (Socket_obj Log_obj)

#클라이언트
add_library (Client_obj SHARED "${PROJECT_SOURCE_DIR}/Cuma_Server_Remake/Cuma_client.cpp")
target_link_libraries (Client_obj Socket_obj Log_obj)

#서버
add_library (Server_obj SHARED "${PROJECT_SOURCE_DIR}/Cuma_Server_Remake/Cuma_Server.cpp")
target_link_libraries (Server_obj Client_obj Socket_obj Log_obj)

#main 실행_executive_link
add_executable(Cuma_Server_Remake main.cpp)

#타겟 링크 라이브러리
target_link_library(exec Server_obj)
target_link_library(exec "${PROJECT_SOURCE_DIR}/boost_1_62_0/lib/libboost_signals.a")
target_link_library(exec "${PROJECT_SOURCE_DIR}/boost_1_62_0/lib/libboost_signals.dylib")
