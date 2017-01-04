//
//  Cuma_main.cpp
//  Cuma_Server_Remake
//
//  Created by ace on 2016. 11. 25..
//  Copyright © 2016년 ace. All rights reserved.
//

#include "Cuma_socket.hpp"

Serv_Sck::Serv_Sck():srv_sock_(0),srv_prt_(0),srv_lstn_(0){
    srv_sck_addr_.sin_addr.s_addr = INADDR_ANY;
}
Serv_Sck::~Serv_Sck(){
}

//소켓 프로퍼티 영역
const int Serv_Sck::get_sck(){
    return srv_sock_;
}
const char* Serv_Sck::get_addr(){
    return  inet_ntoa(srv_sck_addr_.sin_addr);
}
const int Serv_Sck::get_prt(){
    return srv_prt_;
}
const int Serv_Sck::get_lstn(){
    return srv_lstn_;
}
sockaddr_in Serv_Sck::get_srv_sck_addr(){
    return srv_sck_addr_;
}


void Serv_Sck::set_sock(int dis){
    srv_sock_ = dis;
}
void Serv_Sck::set_prt(int prt){
    srv_prt_ = prt;
}
void Serv_Sck::set_lst(int siz){
    srv_lstn_ = siz;
}
























//======================  Cuma_Sck 영역 ===========================

Cuma_Sck::Cuma_Sck(){
    try{
        serv_sock_ = make_shared<Serv_Sck>();
        
        serv_kq_ = kqueue();
        if(serv_kq_ == -1){
            throw errno;
        }
        
        serv_kqueue_ = make_shared<struct kevent>();
        serv_kqueue_t_= std::__1::make_shared<struct kevent>();
        
        //소켓 할당
        serv_sock_->set_sock(socket(PF_INET,SOCK_STREAM,0));
        
    }catch(std::system_error& e){
        std::cout<<"[Error] : Cuma_Sck "<<e.what()<<std::endl;
    }
}

void Cuma_Sck::set_prt(const int p){
    serv_sock_->set_prt(p);
}

string Cuma_Sck::ip(){
    return serv_sock_->get_addr();
}
int Cuma_Sck::prt(){
    return serv_sock_->get_prt();
}


//서버 active
void Cuma_Sck::start(){
    
    //sockaddr_in을 할당
    srv_sck_addr_ = serv_sock_->get_srv_sck_addr();
    srv_sck_addr_.sin_family = AF_INET;
    srv_sck_addr_.sin_addr.s_addr = inet_addr(serv_sock_->get_addr());
    srv_sck_addr_.sin_port = htons(serv_sock_->get_prt());
    
    
    bool bind_wait = true;
    setsockopt(serv_sock_->get_sck(), SOL_SOCKET, SO_REUSEADDR,&bind_wait , sizeof(bool));
    
    bind(serv_sock_->get_sck(), (sockaddr*)&srv_sck_addr_, sizeof(sockaddr));
}

//서버 deacitve
void Cuma_Sck::stop(){
    
    //소켓 디스크립터 shutdown
    shutdown(serv_sock_->get_sck(),SHUT_RDWR);
    close(serv_sock_->get_sck());
    
    //메모리 할당 해제
    serv_sock_.reset();
    serv_kqueue_.reset();
    serv_kqueue_t_.reset();
    
}

//프로퍼티
shared_ptr<Serv_Sck> Cuma_Sck::get_Serv_Sock(){
    return serv_sock_;
}//서버 소켓 에 대한 정보



//서버 클라이언트 listen kevent 구조체 리턴
shared_ptr<struct kevent> Cuma_Sck::get_serv_kqueue(){
    return this->serv_kqueue_;
}

shared_ptr<struct kevent> Cuma_Sck::get_serv_kqueue_t(){
    return this->serv_kqueue_t_;
}


int Cuma_Sck::get_serv_kq(){
    return serv_kq_;
}


Cuma_Sck::~Cuma_Sck(){
    
    //serv_sock의 포인터 reset
    if(serv_sock_.use_count() > 0){
        serv_sock_.reset();
    }
    
}
