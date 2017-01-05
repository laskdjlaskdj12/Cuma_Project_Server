//
//  Cuma_Server.hpp
//  Cuma_Server_Remake
//
//  Created by ace on 2016. 12. 15..
//  Copyright © 2016년 ace. All rights reserved.
//

#ifndef Cuma_Server_hpp
#define Cuma_Server_hpp

#include <iostream>
#include <thread>
#include <fstream>
#include <list>
#include <string>

//파일을 읽을 때 뮤텍스를 호출함
#include <mutex>

//json파일
#include <json/json.h>
#include "Cuma_socket.hpp"
#include "Cuma_client.hpp"

#define READ_BINARY 1
#define WRITE_BINARY 2


using std::string;
using std::list;
using std::stringstream;
using std::shared_ptr;
using std::ofstream;
using std::ifstream;
using std::make_shared;

//클라이언트의 접속이 있었을 경우에 Cuma_server에서 클라이언트의 obj를 할당하고
//클라이언트 obj의 req를 파악해서 배분하는 코어 역활을 함











//Cuma_Server의 코어 객체 영역
class Cuma_Server{
    
//public으로 된 영역
public:
    Cuma_Server();
    ~Cuma_Server();
    
    
    //클라이언트의 소켓 kqueue로 recv되었을때 스레드에 할당하여 할당 명령을 수행함
    void start();
    void stop();
    
    void set_prt(const int p);
    
    //public bool 프로퍼티
    bool is_active();
    void set_active(bool b);
    
//========= private 함수 영역 ==============
    
    
//========= Thread 함수 영역 ==============
private:
    
    //클라이언트가 파일 읽는 함수 : 뮤텍스를 사용
    int r_binary(shared_ptr<Client>& c);
    int w_binary(const shared_ptr<Client>& c);
    
    
    //수신 함수
    void rcv_val(shared_ptr<Client>&c);
    void snd_val(shared_ptr<Client>&c,
                 Json::Value& j);
    
    
//========= Client 멤버 함수 영역 ==============
    
public:
    //쓰레드 가 클라이언트가 리퀘스트를 입수를 했을시에 클라이언트의 req를 실행함
    bool Start_Cli(shared_ptr<Client> cli);
    
private:
    //Client의 연결이 종료가 되었을때 연결 종료를 해주는 함수
    void Close_Cli(shared_ptr<Client>& C);
    
    
private:
//========= 디바이스 로컬 영역 ==============
    
    //코어 갯수
    unsigned int thr_pth;
    
    //쓰기는 뮤텍스 lock을 통해 critical section화 함
    ofstream file_write;
    
    //뮤텍스
    std::mutex mtx_lock;
    
    //Thread 리스트
    std::list<shared_ptr<std::thread>> t_list;
    
//========= 서버 클래스 영역 ==============
    
    //Cuma_sck 클래스
    shared_ptr<Cuma_Sck> cuma_sck;
    
    //active가 됬는지 확인
    bool is_start;
    
    //Cuma_sck의 쓰레드
    std::thread Cuma_sck_thr;
    
    
//========= 클라이언트 클래스 영역 ==============
    
    //접속된 클라이언트 obj 리스트
    std::list<shared_ptr<Client>> c_list;
    
    
    //클라이언트의 리스트 MAX 갯수
    int cli_max;
    
    //클라이언트 소켓 디스크립터 모음
    std::list<int> Cli_des;
    
    
    //클라이언트의 데이터 수신 struct kevent
    shared_ptr<struct kevent> CS_srv_kevent;
    shared_ptr<struct kevent> CS_srv_kevent_t;
    int S_srv_kq;
    
    //Json 쓰기
    Json::StyledWriter writ;
    Json::Reader read;
    Json::Value val;
    
};

#endif /* Cuma_Server_hpp */
