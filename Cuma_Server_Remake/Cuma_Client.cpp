//
//  Cuma_Client.cpp
//  Cuma_Server_Remake
//
//  Created by ace on 2016. 12. 27..
//  Copyright © 2016년 ace. All rights reserved.
//

#include "Cuma_Server.hpp"



// =================== Client 객체 ==============================

Client::Client():start(false){}

Client::Client(shared_ptr<Cli_Sck_Info> s):start(false){
    
    cli_sck = s;
}


Client::~Client(){
    cli_sck.reset();
    mess.reset();
    J_val.clear();
    f_buf.clear();
    start = false;
}


//Json::Reader로 string을 입력함
Json::Value Client::cha_to_json(const string s){
    Json::Value temp;
    J_r.parse(s, temp);
    return temp;
}


//Json::styledWriter로 string으로 변환
string Client::cha_to_str(const Json::Value j){
    return J_w.write(j);
}

//Client를 stop함
void Client::stop(){
    
}

//파일 바이너리 세팅
void Client::set_file(const string s){
    f_buf = s;
}


string Client::get_file(){
    return f_buf;
}

//파일 프레임 등록
void Client::set_f_frame(unsigned long long f){
    this->f_num = f;
    
}
unsigned long long Client::get_f_frame(){
    return f_num;
}



//파일 사이즈 설정
void Client::set_f_siz(unsigned long s){
    f_siz = s;
}
long Client::get_f_siz(){
    return f_siz;
}



//클라이언트 소켓 info를 등록
shared_ptr<Cli_Sck_Info> Client::get_cli_sck_info(){
    return cli_sck;
}
void Client::set_cli_sck_info(shared_ptr<Cli_Sck_Info> p){
    cli_sck = p;
}



//메세지를 등록
shared_ptr<char> Client::get_message(){
    return mess;
}
void Client::set_message(shared_ptr<char> m){
    mess = m;
}



//이름을 등록
std::string Client::get_f_name(){
    return f_name;
}
void Client::set_f_name(std::string nam){
    f_name = nam;
}


//json을 등록
Json::Value Client::get_json( ){
    return J_val;
}


void Client::set_json(Json::Value j_val){
    J_val = j_val;
}


//struct kevent를 등록
shared_ptr < struct kevent> Client::get_kevent(){
    return cli_keven;
}
void Client::set_kevent(shared_ptr<struct kevent> e){
    cli_keven = e;
}



//쓰레드로써 시작을 했는지 start로 bool 리턴
bool Client::is_start(){
    return start;
}

