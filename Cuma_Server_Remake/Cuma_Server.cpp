//
//  Cuma_Server.cpp
//  Cuma_Server_Remake
//
//  Created by ace on 2016. 12. 15..
//  Copyright © 2016년 ace. All rights reserved.
//

#include "Cuma_Server.hpp"

// =================== Client 객체 ==============================
Client::Client(shared_ptr<Cli_Sck_Info> s){
    
    cli_sck_info = s;
}


Client::~Client(){
    cli_sck_info.reset();
    message.reset();
    root.clear();
}

//shared_ptr로 Client정보를 리턴
shared_ptr<Cli_Sck_Info> Client::get_shared_ptr(){
    return cli_sck_info;
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




// =================== Cuma_Server 코어 객체 ==============================


//Cuma_Server의 코어 객체 영역

    
// 서버의 소켓을 오픈후 
Cuma_Server::Cuma_Server(){
    
}


Cuma_Server::~Cuma_Server(){
    
}
    
    
    //클라이언트의 소켓 kqueue로 recv되었을때 스레드에 할당하여 할당 명령을 수행함
void Cuma_Server::start(){
    
}
    
    //Cuma_server로 stop
void Cuma_Server::stop(){
    
}
    
    
    //========= private 함수 영역 ==============
    
    //클라이언트가 파일 읽는 함수 : 뮤텍스를 사용
void Cuma_Server::r_binary(const std::string name){
    
}
    
    
    //쓰레드 가 클라이언트가 리퀘스트를 입수를 했을시에 클라이언트의 req를 실행함
bool Cuma_Server::replay(shared_ptr<Client> cli){
    
}
    
    
    //쓰레드가 에러가 났을때 에러를 보여줌
void Cuma_Server::show_error(){
    
}
    
    
