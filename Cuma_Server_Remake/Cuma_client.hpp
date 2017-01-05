//
//  Cuma_client.hpp
//  Cuma_Server_Remake
//
//  Created by ace on 2017. 1. 5..
//  Copyright © 2017년 ace. All rights reserved.
//

#ifndef Cuma_client_hpp
#define Cuma_client_hpp

#include <iostream>
#include <thread>
#include <fstream>
#include <list>
#include <string>
#include <json/json.h>
#include "Cuma_socket.hpp"


using std::string;
using std::list;
using std::stringstream;
using std::shared_ptr;
using std::ofstream;
using std::ifstream;
using std::make_shared;

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
    
    //파일 버퍼 세팅
    void set_file(const string s);
    string get_file();
    
    //파일 사이즈 등록
    void set_f_siz(unsigned long s);
    long get_f_siz();
    
    //파일 프레임 등록
    void set_f_frame(unsigned long long f);
    unsigned long long get_f_frame();
    
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
    unsigned long long f_siz;
    
    //파일 프레임 넘버
    unsigned long long f_num;
    
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

#endif /* Cuma_client_hpp */
