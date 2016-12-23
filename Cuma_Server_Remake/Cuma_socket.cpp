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



//클라이언트가 connect 프로시저
void Cuma_Sck::cli_chk_con(){
    
    try{
        
        int nev;
        
        //서버소켓에 kqueue를 등록
        EV_SET(serv_kqueue, serv_sock->get_sck(), EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, 0);
        
        
        //서버소켓을 listen 상태로 설정
        listen(serv_sock->get_sck(), 128);
        
        
        //서버 소켓 kqueue active
        while(serv_sock->is_start()){
            
            nev = kevent(serv_kq,
                         serv_kqueue, 1,
                         serv_kqueue_t, 1,
                         NULL);
            
            
            //nev 가 에러일때
            if(nev < 0){
                throw errno;
            }
            
            
            else{
                
                //상대 소켓이 셧다운이 됬을경우
                if(serv_kqueue_t->flags & EV_EOF){
                    std::cout<<"[Error] : Socket is shutdown"<<std::endl;
                    break;
                }
                
                //서버 소켓을 트리거 했을경우에
                else if(serv_kqueue_t->ident == serv_sock->get_sck()){
                    
                    
                    
                    //클라이언트 구조체 할당
                    shared_ptr<Cli_Sck_Info> cli_tmp (new Cli_Sck_Info);
                    
                    
                    //accept 시킴
                    cli_tmp->sck = accept(serv_sock->get_sck(), (sockaddr*)&cli_tmp->cli_sck_addr, (socklen_t*)&cli_tmp->siz);
                    
                    
                    //만약 128개의 사이즈가 넘었다면 큐 부족으로 connect false리턴
                    if(Cli_Info_Lst.size() < 128){
                        
                        //큐가 없다고 클라이언트에게 전송
                        send(cli_tmp->sck, "NO_SPACE_QUEUE", sizeof("NO_SPACE_QUEUE"), 0);
                        
                        
                        //접속한 클라이언트 연결 종료
                        shutdown(cli_tmp->sck, SHUT_RDWR);
                        
                        //s->sck를 flush함
                        if(cli_tmp)
                            cli_tmp.reset();
                        
                        //커넥션을 shutdown 후 클라이언트 struct를 reset
                        cli_tmp.reset();
                        continue;
                    }
                    
                    
                    //클라이언트 소켓 정보를 큐에 insert
                    Cli_Info_Lst.push_back(cli_tmp);
                    
                    
                    //shared_ptr에 등록
                    shared_ptr<struct kevent> k_tmp(new struct kevent);
                    shared_ptr<struct kevent> k_tmp_tri(new struct kevent);
                    
                    //kqueue 이벤트 큐에 등록
                    EV_SET(&*k_tmp,cli_tmp->sck , EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, 0);
                    
                    
                    //클라이언트 이벤트 모니터에 push
                    cli_kqueue_lst.push_back(k_tmp);
                    cli_kqueue_lst_t.push_back(k_tmp_tri);
                    
                    //클라이언트 소켓버퍼 큐에 추가
                    cli_lst.push_back(cli_tmp);
                    
                    
                    //클라이언트 struct를 reset
                    cli_tmp.reset();
                    
                    //컨티뉴
                    continue;
                }
                
            }
            
        }
        
        //std::exception exception catch
    }catch(std::exception& e){
        std::cout<<"[Error] : errno : "<<e.what()<<std::endl;
        
        //std::system_error exception catch
    }catch(std::system_error& e){
        std::cout<<"[Error] : errno : "<<e.code()<<" what : "<<e.what()<<std::endl;
    }
}


//이벤트 모니터 리턴
list<shared_ptr<struct kevent>> Cuma_Sck::get_cli_kqueue_lst(){
    return cli_kqueue_lst;
}


//이벤트 트리거 리턴
list<shared_ptr<struct kevent>> Cuma_Sck::get_cli_kqueue_lst_t(){
    return cli_kqueue_lst_t;
}


//kqueue() 리턴
int Cuma_Sck::get_cli_kqueue(){
    return cli_kq;
}


//클라이언트 소켓 버퍼 리스트
list<shared_ptr<Cli_Sck_Info>> Cuma_Sck::get_cli_sck_lst(){
    return cli_lst;
}

Cuma_Sck::~Cuma_Sck(){
    
    //serv_sock의 포인터 reset
    if(serv_sock.use_count() > 0){
        serv_sock.reset();
    }
    
    
    //클라이언트 수신 이벤트 큐 kevent 포인터 flush
    for(list<shared_ptr<struct kevent>>::iterator iter = cli_kqueue_lst.begin(); iter != cli_kqueue_lst.end(); iter++){
        
        if(iter->use_count() > 0){
            iter->reset();
        }
    }
    
    
    //cli_Info_lst의 포인터 reset
    for(list<shared_ptr<Cli_Sck_Info>>::iterator iter = Cli_Info_Lst.begin(); iter != Cli_Info_Lst.end(); iter++){
        
        if(iter->use_count() > 0){
            iter->reset();
        }
    }
    
}
