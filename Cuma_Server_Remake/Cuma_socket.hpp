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

using std::string;
using std::list;
using std::shared_ptr;

//서버 소켓정보
class Serv_Sck{
public:
    Serv_Sck():srv_sock(0),srv_prt(0),srv_lstn(0),active(false){};
    ~Serv_Sck();
    
    //소켓 프로퍼티 영역
    const int get_sck();            //소켓 디스크립터
    const char* get_addr();         //서버 ip
    const int get_prt();            //서버 prt
    const int get_lstn();           //서버 클라이언트 listen 크기
    
    
    //소켓 set 영역
    void set_sock(int dis);         //소켓 디스크립터
    void set_prt(int prt);          //서버 prt
    void set_addr(const char* addr);//서버 ip
    void set_lst(int siz);          //서버 클라이언트 listen 크기
    
    
    //서버의 기본적인 기능
    void start_srv();               //서버의 시작
    void stop_srv();                //서버 정지
    void restrt_srv();              //서버 재시작
    void exit_serv();               //서버 정료
    bool is_start();                //서버가 시작되었는지?
    
    
private:
    int srv_sock;                   //서버 소켓
    int srv_prt;                    //서버 포트
    char* srv_addr;                  //서버 주소
    int srv_lstn;                 //클라이언트 서버 대기 큐
    bool active;                    //서버가 시작되었는지
    
    shared_ptr<sockaddr_in> srv_sck_addr;  //서버 sock_addr
};




//클라이언트 소켓의 정보
struct Cli_Sck_Info{
    
    Cli_Sck_Info():sck(0),srv_addr(0),prt(0){};
    ~Cli_Sck_Info();
    
    
    //클라이언트 소켓 정보
    int sck;
    long srv_addr;
    int prt;
    std::shared_ptr<sockaddr_in> cli_sck_addr;
};



//Serv_Sck의 기능들에 추가하여 담당을 함
class Cuma_Sck{
public:
    Cuma_Sck();
    ~Cuma_Sck();
    
    
    //프로퍼티
    shared_ptr<Serv_Sck> get_Serv_Sock();  //서버 소켓 에 대한 정보
    
    //클라이언트가 connect 프로시저
    void chk_con();
    
    //클라이언트의 데이터 수신 프로시저
    void rcv_cli_data();
    
    //클라이언트 버퍼 큐
    list<shared_ptr<Cli_Sck_Info>> get_rcv_buff();
    
private:
    
    
    shared_ptr<Serv_Sck> serv_sock;         //서버 소켓에 대한 정보
    list<shared_ptr<Cli_Sck_Info>> Cli_Info;//클라이언트 소켓에 대한 정보
    
    
    //서버 수신 이벤트 큐
    shared_ptr<struct kevent> serv_kqueue;
    int serv_kq;
    
    
    //클라이언트 수신 이벤트 큐
    list<shared_ptr<struct kevent>> cli_kqueue;
    int cli_kq;
    
    
    //클라이언트 버퍼 큐
    list<shared_ptr<Cli_Sck_Info>> cli_buffer;
    
};


#endif /* Cuma_socket_hpp */
