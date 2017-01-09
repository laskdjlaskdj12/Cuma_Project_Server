/*
 Cuma_main.hpp
 Cuma_Server_Remake
 
 Created by ace on 2016. 11. 25..
 Copyright © 2016년 ace. All rights reserved.
 
 해당 헤더는 클라이언트로부터 수신을 받기위해 처리를 합니다.
 전체적인 해당 해더의 역활은 클라이언트로부터 패킷을 수신을 하면 패킷을 수신큐에 저장한후 패킷내용들을 전송합니다.
 
 */
#ifndef Cuma_socket_hpp
#define Cuma_socket_hpp

#ifdef _WIN32                   //만약 윈도우일경우
#define Window
#elif __linux__                 //만약 리눅스일경우
#define Linux
#else                           //만약 유닉스일경우
#define POSIX
#endif

#include <iostream>
#include <memory>
#include <list>

//========== 소켓 헤더 =========

//윈도우 일경우
#ifdef Window
#error "Window is now developing. so We do not recommand using this in Window"


//리눅스일경우
#elif Linux
#error "Linux developing. so We do not recommand using this in Linux"

//유닉스 일경우
#else
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/unistd.h>
#include <unistd.h>
#endif

//============멀티플렉싱에 헤더 ========

#ifdef Window
//윈도우 IOCP


//리눅스 epoll
#elif Linux
#include <epoll.h>
#include <sys/event.h>
#include <sys/time.h>


//유닉스 kqueue
#else
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>

#endif

//시스템 에러 exception 리턴
#include <system_error>

//로그 import
#include "Log.hpp"
#include "Log_text.hpp"


using std::string;
using std::list;
using std::shared_ptr;
using std::make_shared;

//서버 소켓 디스크립터 정보
class Serv_Sck{
public:
    Serv_Sck();
    ~Serv_Sck();
    
    //소켓 프로퍼티 영역
    const int get_sck();            //소켓 디스크립터
    const char* get_addr();         //서버 ip
    const int get_prt();            //서버 prt
    const int get_lstn();           //서버 클라이언트 listen 크기
    sockaddr_in get_srv_sck_addr();
    
    
    //소켓 set 영역
    void set_sock(int dis);         //소켓 디스크립터
    void set_prt(int prt);          //서버 prt
    void set_lst(int siz);          //서버 클라이언트 listen 크기
    
    
private:
    int srv_sock_;                  //서버 소켓
    int srv_prt_;                   //서버 포트
    char* srv_addr_;                //서버 주소
    int srv_lstn_;                  //클라이언트 서버 대기 큐
    bool srv_bnd_wait;              //서버 바인드 대기
    sockaddr_in srv_sck_addr_;      //서버 sock_addr
};




//클라이언트 소켓의 정보
struct Cli_Sck_Info{
    
    Cli_Sck_Info():sck(0),srv_addr(0),prt(0){};
    ~Cli_Sck_Info(){
        sck = 0;
        srv_addr = 0;
        prt = 0;
        siz = 0;
    };
    
    
    //클라이언트 소켓 정보
    int sck;                    //소켓 디스크립터
    long srv_addr;              //소켓 주소
    int prt;                    //소켓 포트
    sockaddr_in cli_sck_addr;   //소켓 sockaddr_in
    int siz;                    //소켓 siz
};



//Serv_Sck의 기능들중 클라이언트 접속시 kevent로 클라이언트 큐에 넣는다.
class Cuma_Sck{
public:
    Cuma_Sck();
    ~Cuma_Sck();
    
    //네트워크 설정
    void set_prt(const int p);
    
    string ip();
    int prt();
    
    //서버 가동
    void start();
    void stop();
    
    //프로퍼티
    shared_ptr<Serv_Sck> get_Serv_Sock();  //서버 소켓 에 대한 정보
    
    
    //서버 클라이언트 listen kevent 구조체 리턴
    shared_ptr<struct kevent> get_serv_kqueue();
    shared_ptr<struct kevent> get_serv_kqueue_t();
    int get_serv_kq();
    
    
private:
    
    //서버 소켓에 대한 정보
    shared_ptr<Serv_Sck> serv_sock_;
    
    
    //서버 수신 이벤트 큐
    shared_ptr<struct kevent> serv_kqueue_;      //이벤트 모니터링
    shared_ptr<struct kevent> serv_kqueue_t_;    //이벤트 트리거
    int serv_kq_;
    
    sockaddr_in srv_sck_addr_;
    
    _CS_Log log;
    
};


#endif /* Cuma_socket_hpp */
