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

// 서버 종료
void Serv_Sck::exit_serv(){
    if(active == true){
        shutdown(srv_sock,SHUT_RDWR);
        active = false;
        std::cout<<"[Info] : Stop Server"<<"success"<<std::endl;
    }
}

bool Serv_Sck::is_start(){
    return active;
}

Cuma_Sck::Cuma_Sck(){
    
    //서버 초기 설정
    serv_sock->set_sock(socket(PF_INET,SOCK_STREAM,0));
    serv_sock->set_addr("127.0.0.1");
    serv_sock->set_prt(33097);
    serv_sock->set_prt(10);
    
    //서버 가동
    serv_sock->start_srv();
    
    
}

//프로퍼티
shared_ptr<Serv_Sck> Cuma_Sck::get_Serv_Sock(){
    return serv_sock;
}//서버 소켓 에 대한 정보

//클라이언트가 connect 프로시저
void Cuma_Sck::chk_con(){
    
    
}

//클라이언트의 데이터 수신 프로시저
void Cuma_Sck::rcv_cli_data(){
    
    
}

//클라이언트 버퍼 큐
list<shared_ptr<Cli_Sck_Info>> Cuma_Sck::get_rcv_buff(){
    
}

