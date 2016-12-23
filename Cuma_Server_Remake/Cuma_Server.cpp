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


// 서버의 소켓을 생성자로 구성후 클라이언트 connection을 받을 준비 함
Cuma_Server::Cuma_Server(){
    
    //Cuma_Sck에 대한 생성자 생성
    //Cuma_Sck::serv_sock 의 kqueue가 생성후 33097 포트로 설정
    //서버 소켓 옵션에서 bnd_no_wat로 설정후 바인딩
    //active = true로 설정
    
    cuma_sck = std::make_shared<Cuma_Sck>();
    
    
    //쓰레드 코어의 갯수만큼 쓰레드 생성
    
    thr_pth = std::thread::hardware_concurrency();
    
    
    //서버 소켓 정보 받기
    serv_sck = cuma_sck->get_Serv_Sock();
    
    
    //client struct kevent 정보를 받음
    CS_cli_kqueue_lst = cuma_sck->get_cli_kqueue_lst();
    CS_cli_kqueue_lst_t = cuma_sck->get_cli_kqueue_lst_t();
    S_cli_kq = cuma_sck->get_cli_kqueue();
    
    
    //클라이언트 소켓 info 구조체 포인터 전달
    Cli_Info = cuma_sck->get_cli_sck_lst();
    
    
    
}


Cuma_Server::~Cuma_Server(){
    cuma_sck.reset();
    val.clear();
}


//서버의 listen 상태로 설정후
void Cuma_Server::start(){
    try{
        //먼저 서버가 active를 true로 시킴
        is_start = true;
        
        //Cuma_Sck의 chkconnect를 시작후 클라이언트의 input이 들어오면
        cuma_sck->cli_chk_con();
        
        
        //kqueue에 대한 정보를 받아서 클라이언트 소켓 버퍼 큐에 대해 읽음
        
        //만약 is_start가 false가 될때까지
        while(is_start){
            
            
            //temp_mot_keven은 list로 된 CS_cli_kqueue_lst를 array형식으로 읽는것
            struct kevent* temp_mot_keven(new struct kevent [128]);
            int tmp_keven = 0;
            
            
            //temp_trig_keven은 list로 된 CS_cli_kqueue_lst_t를 array형식으로 읽는것
            struct kevent* temp_trig_keven(new struct kevent [128]);
            int tmp_trig_keven = 0;
            
            
            //클라이언트 소켓 디스크립터list를 array화 함
            Cli_Sck_Info* cli_sck(new Cli_Sck_Info [128]);
            int cli_ite = 0;
            
            //iterator로 array형식으로 치환
            for(list<shared_ptr<struct kevent>>::iterator ite = CS_cli_kqueue_lst.begin(); ite != CS_cli_kqueue_lst.end(); ite++){
                
                temp_mot_keven[tmp_keven] = *(*ite);
                
                tmp_keven++;
            }
            
            
            //위와 같음
            for(list<shared_ptr<struct kevent>>::iterator ite = CS_cli_kqueue_lst_t.begin(); ite != CS_cli_kqueue_lst_t.end(); ite++){
                
                temp_trig_keven[tmp_keven] = *(*ite);
                
                tmp_trig_keven++;
            }
            
            
            //클라이언트의 리스트를 array 화하여 만듬
            for(list<shared_ptr<Cli_Sck_Info>>::iterator ite = Cli_Info.begin();
                ite != Cli_Info.end(); ite++)
            {
                cli_sck[cli_ite] = *(*ite);
                
                cli_ite++;
            }
            
            
            
            
            //kevent를 통해 nev를 형성
            int nev = kevent(S_cli_kq,
                             temp_mot_keven,
                             CS_cli_kqueue_lst.size(),
                             temp_trig_keven,
                             CS_cli_kqueue_lst_t.size(),
                             NULL);
            
            
            
            
            //만약 kevent의 에러가 발생했을 경우
            if(nev < 0){
                std::cout<<"[Info] : nev is fail"<<std::endl;
                throw errno;
            }else{
                for(int i = 0; i < tmp_keven; i++){
                    
                    
                    //만약 클라이언트가  shutdown이 됬을경우
                    if( temp_trig_keven[i].flags & EV_EOF){
                        
                    }
                    
                    
                    //클라이언트가 EV_ERROR를 리턴했을시에
                    else if(temp_trig_keven[i].flags & EV_ERROR){
                        std::cout<<"[Info : Client temp_trig_keven[i] is error"<<std::endl;
                    }
                    
                    
                    //클라이언트 디스크립터 에 read buffer가 왔을시
                    else if(temp_trig_keven[i].ident == cli_sck[i].sck){
                        
                        shared_ptr<Client> temp_cli(cli_sck[i]);
                        
                        
                        //replay로 넘어감
                        replay(<#shared_ptr<Client> cli#>);
                        
                    }
                }
            }
            
            tmp_keven = 0;
            temp_trig_keven = 0;
            
            
        }
        
        
        
    }catch(std::exception& e){
        std::cout<<"[Error] : Error res"<<e.what()<<std::endl;
        return;
    }
}

//Cuma_server로 stop
void Cuma_Server::stop(){
    
}




//========= private 함수 영역 ==============



//클라이언트가 파일 읽는 함수 : 뮤텍스를 사용
std::string Cuma_Server::r_binary(const std::string name){
    
}


//쓰레드 가 클라이언트가 리퀘스트를 입수를 했을시에 클라이언트의 req를 실행함
std::string Cuma_Server::write_binary(const std::string name,
                                      const std::string binary,
                                      const unsigned long byte){
    
}


//쓰레드 가 클라이언트가 리퀘스트를 입수를 했을시에 클라이언트의 req를 실행함
bool Cuma_Server::replay(shared_ptr<Client> cli){
    
}

//쓰레드가 에러가 났을때 에러를 보여줌
void Cuma_Server::show_error(){
    
}

