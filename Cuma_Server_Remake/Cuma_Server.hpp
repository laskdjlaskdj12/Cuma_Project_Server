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


using std::string;
using std::list;
using std::stringstream;
using std::shared_ptr;
using std::ofstream;
using std::ifstream;
using std::make_shared;

//클라이언트의 접속이 있었을 경우에 Cuma_server에서 클라이언트의 obj를 할당하고
//클라이언트 obj의 req를 파악해서 배분하는 코어 역활을 함



//클라이언트 obj 객체



class Client{
    
    
public:
    
    //클라이언트의 정보를 입력
    Client();
    Client(shared_ptr<Cli_Sck_Info> s);
    ~Client();
    
    
    //Json::Reader로 string을 입력함
    Json::Value cha_to_json(const string s);
    
    
    //Json::styledWriter로 string으로 변환
    string cha_to_str(const Json::Value j);
    
    
    //Client
    void stop();
    
    
    //================ 프로퍼티 =========================
    
    //파일 이름 세팅
     void set_file(const string s);
     string get_file();

    //파일 사이즈 등록
     void set_f_siz(unsigned long s);
     long get_f_siz();
    
    //클라이언트 소켓 info를 등록
     shared_ptr<Cli_Sck_Info> get_cli_sck_info();
     void set_cli_sck_info(shared_ptr<Cli_Sck_Info> p);
    
    
    //메세지를 등록
     shared_ptr<char> get_message();
     void set_message(shared_ptr<char> mess);
    
    
    //이름을 등록
     std::string get_f_name();
     void set_f_name(std::string nam);
    
    
    //json을 등록
     Json::Value get_json( );
     void set_json(Json::Value j_val);
    
    
    //struct kevent를 등록
     shared_ptr < struct kevent> get_kevent();
     void set_kevent(shared_ptr<struct kevent> e);
    
    
    //시작했는지 뮤텍스 측정
    bool is_start();
    
private:
    
    
//========= 클라이언트 소켓 영역 ===============
    //클라이언트 소켓 정보
    shared_ptr<Cli_Sck_Info> cli_sck;
    
    
    //클라이언트 메세지 버퍼
    shared_ptr<char> mess;
    
    
    //클라이언트 kevent
    shared_ptr<struct kevent> cli_keven;
    
    
//========= 파일 영역 ===============
    //파일 이름
    std::string f_name;
    
    //파일 버퍼
    std::string f_buf;
    
    //파일 버퍼 크기
    unsigned long f_siz;
    
//========= JSON 영역 ===============
    
    //json 값
    Json::Value J_val;
    
    //json string 쓰기
    Json::StyledWriter J_w;
    
    
    //json 읽기
    Json::Reader J_r;
    
    
//========= 뮤텍스 영역 ===============
    //뮤텍스로 쓰레드 제어하기
    std::mutex Cli_test;
    
    bool start;
    
};












//Cuma_Server의 코어 객체 영역
class Cuma_Server{
    
//public으로 된 영역
public:
    Cuma_Server();
    ~Cuma_Server();
    
    
    //클라이언트의 소켓 kqueue로 recv되었을때 스레드에 할당하여 할당 명령을 수행함
    void start();
    void stop();
    
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
    std::list<std::thread> t_list;
    
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
