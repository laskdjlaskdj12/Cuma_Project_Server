//
//  Cuma_Server.cpp
//  Cuma_Server_Remake
//
//  Created by ace on 2016. 12. 15..
//  Copyright © 2016년 ace. All rights reserved.
//

#include "Cuma_Server.hpp"




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
    
    
    
    
    //cuma_sck의 kevent struct와 kevent 가지고 오기
    CS_srv_kevent = cuma_sck->get_serv_kqueue();
    CS_srv_kevent_t = cuma_sck->get_serv_kqueue_t();
    S_srv_kq = cuma_sck->get_serv_kq();
    
    
    
}


Cuma_Server::~Cuma_Server(){
    cuma_sck.reset();
    val.clear();
}


//서버의 listen 상태를 설정후 listen된 클라이언트들은 Start_cli로 보냄
void Cuma_Server::start(){
    try{
        // 클라이언트 소켓의 kqueue의 리턴값을 저장할 temp를 만듬
        int nev;
        
        //서버소켓에 kqueue를 등록
        EV_SET((&*CS_srv_kevent), (&*cuma_sck->get_Serv_Sock())->get_sck(), EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, 0);
        
        //클라이언트가 connect 될때가지 listen
        listen(cuma_sck->get_Serv_Sock()->get_sck(),128);
        
        
        //만약 is_start == true
        while(is_start){
            
            nev = kevent(S_srv_kq,
                         (&* CS_srv_kevent), 1,
                         (&* CS_srv_kevent_t), 1,
                         NULL);
            
            
            
            
            //nev 가 에러일때
            if(nev < 0){
                throw errno;
            }
            
            
            else{
                
                //상대 소켓이 셧다운이 됬을경우
                if(CS_srv_kevent_t->flags & EV_EOF){
                    std::cout<<"[Error] : Socket is shutdown"<<std::endl;
                    break;
                }
                
                //서버 소켓을 트리거 했을경우에
                else if(CS_srv_kevent_t->ident == cuma_sck->get_Serv_Sock()->get_sck()){
                    
                    
                    
                    //클라이언트 구조체 할당
                    shared_ptr<Cli_Sck_Info> cli_tmp (new Cli_Sck_Info);
                    shared_ptr<Client> Client_temp  (new Client);
                    
                    
                    //accept
                    cli_tmp->sck = accept(cuma_sck->get_Serv_Sock()->get_sck(),
                                          (sockaddr*)&cli_tmp->cli_sck_addr,
                                          (socklen_t*)&cli_tmp->siz);
                    
                    
                    //만약 128개의 사이즈가 넘었다면 큐 부족으로 connect false리턴
                    if(c_list.size() < 128){
                        
                        //큐가 없다고 클라이언트에게 전송
                        send(cli_tmp->sck, "NO_SPACE_QUEUE", sizeof("NO_SPACE_QUEUE"), 0);
                        
                        
                        //접속한 클라이언트 연결 종료
                        shutdown(cli_tmp->sck, SHUT_RDWR);
                        
                        //cli_tmp 디스크립터 close
                        close(cli_tmp->sck);
                        
                        
                        //Client object reset
                        cli_tmp.reset();
                        Client_temp.reset();
                        
                        continue;
                        
                        
                        //만약 중복 재접속 했을 경우
                    }else if(std::find(Cli_des.begin(), Cli_des.end(), cli_tmp->sck) != Cli_des.end()){
                        
                        //중복접속으로 클라이언트에게 전송
                        send(cli_tmp->sck, "NO_SPACE_QUEUE", sizeof("NO_SPACE_QUEUE"), 0);
                        
                        
                        //접속한 클라이언트 연결 종료
                        shutdown(cli_tmp->sck, SHUT_RDWR);
                        
                        //cli_tmp 디스크립터 close
                        close(cli_tmp->sck);
                        
                        
                        //Client object reset
                        cli_tmp.reset();
                        Client_temp.reset();
                        
                        continue;
                        
                        //정상적인 접속일 경우
                    }else{
                        
                        
                        //cli_sck_info를 Client obj에 inheritage
                        Client_temp->set_cli_sck_info(cli_tmp);
                        
                        
                        //쓰레드 생성
                        std::thread Cli_proc(&Cuma_Server::Start_Cli, this, Client_temp);
                        
                        
                        //Client 리스트에 push
                        c_list.push_back(Client_temp);
                        
                        //클라이언트 디스크립터에 push
                        Cli_des.push_back(cli_tmp->sck);
                        
                        //cli_tmp와 Client_temp의 index를 reset
                        cli_tmp.reset();
                        Client_temp.reset();
                        
                        
                        //컨티뉴
                        continue;
                    }
                }
                
            }
        }
        
        //만약 is_start 가 false가 되었을 경우 cuma_sck->get_Serv_Sock()의 stop으로 넘어감
        stop();
        
    }catch(std::exception& e){
        std::cout<<"[Error] : Error res"<<e.what()<<std::endl;
        
        //바로 stop으로 넘어감
        return;
    }
}


//Cuma_server로 stop
void Cuma_Server::stop(){
    
    //is_start를 false로 함
    is_start = false;
    
    cuma_sck->stop();
    
    
}


bool Cuma_Server::is_active(){
    return is_start;
}


void Cuma_Server::set_active(bool b){
    is_start = b;
}


//========= private 함수 영역 ==============



//클라이언트가 파일 읽는 함수 : 뮤텍스를 사용
int Cuma_Server::r_binary(shared_ptr<Client>& c){
    
    try{
        
        //Client 파라미터에 파일 이름을 설정
        string name = c->get_f_name();
        
        //ifstream을 설정
        ifstream f_read;
        
        //리턴할 string의 크기를 f_temp에 지정함
        string f_temp;
        
        //파일을 읽어서 string으로 리턴을 함
        f_read.open(name,std::ios::binary);
        
        
        //만약 파일이 존재하지 않는다면
        if(f_read.is_open() == false){
            throw "FILE_NOT_EXSIST";
        }
        
        
        //streampos으로 file_read의 크기를 구함
        std::streampos f_siz = f_read.tellg();
        
        
        //Client 파라미터에 파일 사이즈를 등록
        c->set_f_siz(f_siz);
        
        
        //file_buf로 new char[]을 할당
        char* file_buf = new char[f_siz];
        
        
        //file_read로 읽음
        f_read.read(file_buf, f_siz);
        
        
        //리턴할 string에 해당 파일 크기 만큼 바이너리를 input
        f_temp.append(file_buf, f_siz);
        
        
        //file_buf는 메모리에서 삭제
        delete[] file_buf;
        
        //f_temp를 Client에 넣음
        c->set_file(f_temp);
        
        //file_read는 close를 함
        f_read.close();
        
        
        //f_temp를 리턴함
        return 0;
        
        //만약 파일이 없을 시에 NULL를 리턴함
    }catch(const string& e){
        std::cout<<"[Info] : No_File_name"<<std::endl;
        return -1;
    }
}


//쓰레드 가 클라이언트가 리퀘스트를 입수를 했을시에 클라이언트의 req를 실행함
int Cuma_Server::w_binary(const shared_ptr<Client>& c){
    
     try{
         
         //뮤텍스 lock을 함
         mtx_lock.lock();
         
         const std::string name = c->get_f_name();
         const std::string binary = c->get_file();
         const unsigned long byte = c->get_f_siz();
         
         //파일 쓰기(바이너리로 파일을 오픈)
         file_write.open(name,std::ios::binary);
         
         
         //파일을 입력하기
         file_write.write(binary.c_str(), byte);
         
         //뮤텍스 unlock
         mtx_lock.unlock();
         
         
         return 0;
         
         
         //만약 exception이 됬다면
     }catch(std::exception& e){
         std::cout<<"[Error] : "<<e.what()<<std::endl;
         return -1;
     }
 }


//쓰레드 가 클라이언트가 리퀘스트를 입수를 했을시에 클라이언트의 req를 실행함
bool Cuma_Server::Start_Cli(shared_ptr<Client> cli){
    try{
        
        //클라이언트에서 전송한 recv를 받음
        rcv_val(cli);
        
        //val_tmp로 클라이언트로부터 수신을 받음
        Json::Value val_tmp = cli->get_json();
        
        //만약 Error인 오브젝트가 왔을시에
        if(val_tmp["Error"].asBool() == true){
            
            //throw string
            throw "CLI_RCV_ERR";
        }
        
        //클라이언트 객체의 내부 데이터로 지정
        cli->set_json(val_tmp);
        
        //클라이언트 객체의 파일 설정
        cli->set_f_name(val_tmp["File"].asString());
        cli->set_f_siz(val_tmp["F_Siz"].asLargestUInt());
        
        //오브젝트 모드 확인
        switch (val_tmp["MODE"].asInt()){
            case 1:     //파일 read일시
                if(r_binary(cli) < 0){
                    throw "NOFILE";
                }
                break;
                
            case 2:     //파일 write일시
                if(w_binary(cli) < 0){
                    throw "NOWRIGHT";
                }
                break;
                
            default:    //파일모드가 잘못되었을시
                throw "Wrong_MODE";
                break;
        }
        
        //val_tmp의 버퍼를 clear함
        val_tmp.clear();
        
        
        //파일 이름, 크기, 바이너리 추가
        val_tmp["File_name"] = cli->get_f_name();
        val_tmp["File_binary"] = cli->get_file();
        val_tmp["File_siz"] = (u_int64_t)cli->get_f_siz();
        
        //val_tmp를 전송함
        snd_val(cli, val_tmp);
        
        //클라이언트 접속 connect 셧다운
        Close_Cli(cli);
        
        return true;
        
        //만약 exception이 났을 경우
    }catch(std::exception& e){
        
        Json::Value snd_err;
        //에러 문구 출력
        std::cout<<"[Error]"<<e.what()<<std::endl;
        
        //클라이언트에게 exception이 났다고 전달을 함
        
        snd_err["Error"] = "STD_EXCEP";
        snd_err["Reason"] = e.what();
        
        //클라이언트로 전송
        snd_val(cli,snd_err);
        
        //클라이언트 접속 connect 셧다운
        Close_Cli(cli);
        
        //false를 리턴함
        return false;
        
        
        
        
        //Json::Exception
    }catch(Json::Exception& e){
        
        Json::Value snd_err;
        std::cout<<"[Error] : Json::Exception"<<e.what()<<std::endl;
        
        //클라이언트에게 exception이 났다고 전달을 함
        
        snd_err["error"] = "JSON_EXCEP";
        snd_err["Reason"] = e.what();
        
        //클라이언트로 전송
        snd_val(cli,snd_err);
        
        //클라이언트 접속 connect 셧다운
        Close_Cli(cli);
        
        
        //false를 리턴함
        return false;
        
        
    }catch(const std::string& e){
        Json::Value snd_err;
        std::cout<<"[Error] : Error of"<<e<<std::endl;
        
        //클라이언트에게 exception이 났다고 전달을 함
        
        snd_err["error"] = "Client";
        snd_err["Reason"] = e;
        
        //클라이언트로 전송
        snd_val(cli,snd_err);
        
        //클라이언트 접속 끊기
        Close_Cli(cli);
        
        
        
        //false를 리턴함
        return false;
    }
}



//Client의 연결이 종료가 되었을때 Client 커넥션 종료
void Cuma_Server::Close_Cli(shared_ptr<Client>& c){
    
    //해당 소켓 디스크립터를 shutdown함
    c->stop();
    
}


//리시브할 함수
void Cuma_Server::rcv_val(shared_ptr<Client>& c){
    try{
        
        //변수들
        unsigned long s;        //파일 크기
        long t;                 //파일버퍼 템프
        unsigned long t1;
        char* f;                //파일 버퍼 temp
        string s_t;             //버퍼 스트링 temp
        Json::Value rcv_f;
        
        
        
        //클라이언트 전송 버퍼 siz를 먼저 recv
        if(recv(c->get_cli_sck_info()->sck, &s , 8, 0) < 0){
            throw errno;
        }
        
        //리시브 버퍼 temp를 생성
        f = new char[s];
        
        
        //recv의 버퍼가 완전히 될때 까지 recv
        while(t1 < s){
            
            //파일 버퍼를 수신
            t = recv(c->get_cli_sck_info()->sck, f, s, 0);
            
            //만약 t가 0보다 작다면
            if(t < 0){
                throw errno;
            }
            
            //크기를 증가시킴
            t1 += t;
            
            //append를 함
            s_t.append(f, t);
        }
        
        //수신받은 temp를 json으로 세팅함
        rcv_f = c->cha_to_str(s_t);
        
        //json을 클라이언트로 등록
        c->set_json(rcv_f);
        
        //f를 딜리트
        delete[] f;
        
        //리턴
        return;
        
    }catch(std::exception& e){
        std::cout<<"[Error] : "<<e.what()<<std::endl;
        return;
    }catch(Json::Exception& e){
        std::cout<<"[Error] : "<<e.what()<<std::endl;
        return;
    }
    
}

//보낼 함수
void Cuma_Server::snd_val(shared_ptr<Client>&c,
                          Json::Value& j){
    
    string temp = writ.write(j);
    unsigned long siz = temp.size();
    
    //사이즈 전송
    send(c->get_cli_sck_info()->sck, &siz, sizeof(unsigned long), 0);
    
    //버퍼 전송
    send(c->get_cli_sck_info()->sck, temp.c_str(), siz,0);
    
}
