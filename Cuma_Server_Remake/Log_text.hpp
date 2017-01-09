//
//  Log.hpp
//  Cuma_Server_Remake
//
//  Created by ace on 2017. 1. 6..
//  Copyright © 2017년 ace. All rights reserved.
//

#ifndef Log_text
#define Log_text

#define serv_sck_init               "serv_sock_init"
#define serv_sck_init_kqueue_func   "serv_sock_kqueue()_init"
#define serv_sck_init_kqueue        "serv_kqueue_init"
#define serv_sck_init_kqueue_t      "serv_kqueue_t_init"
#define serv_sck_init_socket        "serv_socket_init"
#define serv_sck_init_sockaddr_in   "serv_socket_sockaddr_in"
#define serv_sck_setsockopt         "serv_socket_sockopt"
#define serv_sck_bind               "serv_socket_bind"


#define CS_START            "CS_SERV_START"
#define CS_EV_SET           "CS_SERV_EV_SET"
#define CS_EV_LST           "CS_SERV_LST"
#define CS_KEVENT           "CS_RCV_RED"
#define CS_ACCEPT           "CS_ACCEPT"
#define CS_QUEUE_FULL       "CS_ACCEPT_QUEUE_FULL"
#define CS_QUEUE_DUP        "CS_ACCEPT_DUP"
#define CS_JSON_RCV_CLR     "CS_RCV_CLR"
#define CS_THREAD_ALLOC     "CS_THREAD_ALLOC"
#define CS_THREAD_FLUSH     "CS_THREAD_FLUSH"
#define CS_STOP             "CS_SERV_STOP"


#define CS_START_RCV            "CS_START_RCV_VAL"
#define CS_START_SET_CLI_OBJ    "CS_START_SET_CLI_OBJ"
#define CS_START_READ_BINARY    "CS_START_READ_BINARY"
#define CS_START_WRITE_BINARY   "CS_START_WRIT_BINARY"
#define CS_START_WRONG_MOD      "CS_START_WRONG_MOD"
#define CS_START_ADD_JSON       "CS_START_ADD_JSON"
#define CS_START_JSON_CLR       "CS_START_JSON_CLR"
#define CS_START_SND            "CS_START_SND"
#define CS_START_CLOSE_CLI      "CS_START_CLOSE_CLI"

#define CS_CLOSE    "CS_STOP_push_thr_id"
#define CS_CLOSE_REM_SCK    "CS_STOP_REMOVE_sck_des"
#define CS_CLOSE_SCK_STOP   "CS_STOP_SCK_STOP"
#endif /* Log_text */
