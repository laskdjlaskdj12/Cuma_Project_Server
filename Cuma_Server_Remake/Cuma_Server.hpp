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

//클라이언트의 접속이 있었을 경우에 Cuma_server에서 클라이언트의 obj를 할당하고
//클라이언트 obj의 req를 파악해서 배분하는 코어 역활을 함



//클라이언트 obj 객체
class Client{
    
    
public:
    
    //클라이언트의 정보를 입력
    Client(shared_ptr<Cli_Sck_Info> s);
    ~Client();
    
    
    //shared_ptr로 Client정보를 리턴
    shared_ptr<Cli_Sck_Info> get_shared_ptr();
    
    
    //Json::Reader로 string을 입력함
    Json::Value cha_to_json(const string s);
    
    
    //Json::styledWriter로 string으로 변환
    string cha_to_str(const Json::Value j);
    
    
    //파일 이름 세팅
    void set_file(const string s);
    string get_file();
    
    
    
private:
    
    //클라이언트 소켓 정보
    shared_ptr<Cli_Sck_Info> cli_sck_info;
    
    
    //클라이언트 메세지 버퍼
    shared_ptr<char> message;
    
    
    //클라이언트 kevent
    shared_ptr<struct kevent> cli_keven;
    
    //파일 이름
    std::string f_name;
    
    //파일 버퍼
    shared_ptr<unsigned char> f_buf;
    
    //json 스트링 버퍼
    std::string json_str;
    
    //json 값
    Json::Value root;
    
    //json string 쓰기
    Json::StyledWriter J_w;
    
    
    //json 읽기
    Json::Reader J_r;
    
};












//Cuma_Server의 코어 객체 영역
class Cuma_Server{
    
//public으로 된 영역
public:
    Cuma_Server();
    ~Cuma_Server();
    
    
    //클라이언트의 소켓 kqueue로 recv되었을때 스레드에 할당하여 할당 명령을 수행함
    void start();
    
    //Cuma_server로 stop
    void stop();
    
    
//========= private 함수 영역 ==============
private:
    
    //클라이언트가 파일 읽는 함수 : 뮤텍스를 사용
    std::string r_binary(const std::string name);
    
    std::string write_binary(const std::string name,
                             const std::string binary,
                             const unsigned long byte);
    
    //쓰레드 가 클라이언트가 리퀘스트를 입수를 했을시에 클라이언트의 req를 실행함
    bool replay(shared_ptr<Client> cli);
    
    
    //쓰레드가 에러가 났을때 에러를 보여줌
    void show_error();
    
    
    
private:
//========= 디바이스 로컬 영역 ==============
    
    //코어 갯수
    unsigned int thr_pth;
    
    
    //쓰레드 리스트
    std::list<std::thread> thr_lst;
    
    
    //파일 검색 변수
    ofstream file_read;
    ifstream file_write;
    
    
    //뮤텍스
    std::mutex mtx_lock;
    
    
//========= 서버 클래스 영역 ==============
    
    //Cuma_sck 클래스
    shared_ptr<Cuma_Sck> cuma_sck;
    
    
    //서버 소켓 정보
    std::__1::shared_ptr<Serv_Sck> serv_sck;
    
    bool is_start;
    
    
//========= 클라이언트 클래스 영역 ==============
    
    //접속된 클라이언트 obj 리스트
    std::list<shared_ptr<Client>> c_list;
    
    
    //클라이언트의 리스트 MAX 갯수
    int cli_max;
    
    //클라이언트 소켓 디스크립터 모음
    std::list<shared_ptr<Cli_Sck_Info>> Cli_Info;
    
    
    //클라이언트의 데이터 수신 struct kevent
    list<shared_ptr<struct kevent>> CS_cli_kqueue_lst;
    list<shared_ptr<struct kevent>> CS_cli_kqueue_lst_t;
    int S_cli_kq;
    
    //Json 쓰기
    Json::StyledWriter writ;
    
    //Json 읽기
    Json::Reader read;
    
    //Json 변수
    Json::Value val;
    
};

#endif /* Cuma_Server_hpp */
