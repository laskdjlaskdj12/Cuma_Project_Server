//
//  Cuma_main.cpp
//  Cuma_Server_Remake
//
//  Created by ace on 2016. 11. 25..
//  Copyright © 2016년 ace. All rights reserved.
//

#include "Cuma_socket.hpp"


Serv_Sck::~Serv_Sck(){
    if(active == true){
        close(srv_sock);
        active = false;
    }
}

//소켓 프로퍼티 영역
const int Serv_Sck::get_sck(){
    return srv_sock;
}
const char* Serv_Sck::get_addr(){
    return  inet_ntoa(srv_sck_addr->sin_addr);
}
const int Serv_Sck::get_prt(){
    return srv_prt;
}
const int Serv_Sck::get_lstn(){
    return srv_lstn;
}


//소켓 set 영역
void Serv_Sck::set_sock(int dis){
    srv_sock = dis;
}
void Serv_Sck::set_prt(int prt){
    srv_prt = prt;
}
void Serv_Sck::set_addr(const char* addr){
    srv_sck_addr->sin_addr.s_addr = inet_addr(addr);
}
void Serv_Sck::set_lst(int siz){
    srv_lstn = siz;
}


//서버의 기본적인 기능
void Serv_Sck::start_srv(){
    bool bnd_no_wat;
    
    srv_sock = socket(PF_INET,SOCK_STREAM,0);
    
    srv_sck_addr->sin_family = AF_INET;
    srv_sck_addr->sin_port = htons(srv_prt);
    
    bnd_no_wat = true;
    setsockopt(srv_sock, SOL_SOCKET, SO_REUSEADDR, &bnd_no_wat, sizeof(bool));
    
    bind(srv_sock, (sockaddr*)&srv_sck_addr, sizeof(sockaddr));
    
    active = true;
}

//서버 중단(종료 아님)
void Serv_Sck::stop_srv(){
    if(active == true){
        shutdown(srv_sock, SHUT_RDWR);
        active = false;
        
        std::cout<<"[Info] : Stop Server = "<<"success"<<std::endl;
    }
    return;
}
//서버 재시작
void Serv_Sck::restrt_srv(){
    if(active == true){
        
        shutdown(srv_sock, SHUT_RDWR);
        active = false;
        
        std::cout<<"[Info] : Stop Server = "<<"success"<<std::endl;
        
    }
    bind(srv_sock, (sockaddr*)&srv_sck_addr, sizeof(sockaddr));
    
    
    //소켓 옵션 설정
    bool bnd_no_wat;
    setsockopt(srv_sock, SOL_SOCKET, SO_REUSEADDR, &bnd_no_wat, sizeof(bool));
    
}

// 서버 소켓 디스크립터 종료
void Serv_Sck::exit_serv(){
    if(active == true){
        shutdown(srv_sock,SHUT_RDWR);
        active = false;
        std::cout<<"[Info] : Stop Server"<<"success"<<std::endl;
    }
}

//서버 확인 체크
bool Serv_Sck::is_start(){
    return active;
}

























//======================  Cuma_Sck 영역 ===========================

Cuma_Sck::Cuma_Sck(){
    try{
        
        serv_kq = kqueue();
        if(serv_kq == -1){
            throw errno;
        }
        
        
        //서버 초기 설정
        serv_sock->set_sock(socket(PF_INET,SOCK_STREAM,0));
        serv_sock->set_addr("127.0.0.1");
        serv_sock->set_prt(33097);
        
        //서버 가동
        serv_sock->start_srv();
    }catch(std::system_error& e){
        std::cout<<"[Error] : Cuma_Sck "<<e.what()<<std::endl;
    }
}



//프로퍼티
shared_ptr<Serv_Sck> Cuma_Sck::get_Serv_Sock(){
    return serv_sock;
}//서버 소켓 에 대한 정보



//서버 클라이언트 listen kevent 구조체 리턴
shared_ptr<struct kevent> Cuma_Sck::get_serv_kqueue(){
    return this->serv_kqueue;
}

shared_ptr<struct kevent> Cuma_Sck::get_serv_kqueue_t(){
    return this->serv_kqueue_t;
}


int Cuma_Sck::get_serv_kq(){
    return serv_kq;
}



//Cuma_sck 를 stop함
void Cuma_Sck::stop(){
    
    serv_sock->stop_srv();
    serv_kqueue.reset();
    serv_kqueue_t.reset();
    
}


Cuma_Sck::~Cuma_Sck(){
    
    //serv_sock의 포인터 reset
    if(serv_sock.use_count() > 0){
        serv_sock.reset();
    }
    
}
