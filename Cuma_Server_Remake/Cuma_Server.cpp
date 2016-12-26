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
            
            //여기에서 list<shared_ptr<Client>>는 temp list로 connect된 클라이언트들을 리스트 작성함
            
            
            
            //Cli_Sck_Info의 iterator 로 Cli_Info의 값들을 넣음
            for(std::list<shared_ptr<Cli_Sck_Info>>::iterator iter = Cli_Info.begin();
                iter != Cli_Info.end();
                iter++){
                
                //Client의 tmp를 만듬
                shared_ptr<Client> c_tmp = std::shared_ptr<Client>(new Client);
                
                //c_list에 Cli_sck_Info 프로퍼티를 세팅
                c_tmp->set_cli_sck_info(*iter);
                
                //c_list에 push
                c_list.push_back(c_tmp);
                
                //c_tmp에 리셋
                c_tmp.reset();
            }
            
            
            
            
            //c의 크기는 Cli_Info의 리스트에 들어있는 원소들의 갯수로 함
            
            //kevent 모니터 구조체
            shared_ptr<struct kevent> k_mon = std::shared_ptr<struct kevent>(new struct kevent[Cli_Info.size()]);
            
            
            //kevent 트리거 구조체
            shared_ptr<struct kevent> k_tri = std::shared_ptr<struct kevent>(new struct kevent[Cli_Info.size()]);
            
            
            //Client 배열의 인덱스 설정
            int keve_idx = 0;
            
            
            //모니터할 클라이언트의 kevent를 Client객체에 넣음
            for(std::list<shared_ptr<struct kevent>>::iterator iter = CS_cli_kqueue_lst.begin();
                iter != CS_cli_kqueue_lst.end();
                iter++){
                
                
                //kevent를 등록
                (&*k_mon)[keve_idx] = *(*iter);
                
                
                //인덱스 추가
                keve_idx++;
                
                
                //iter 가 참조한 CS_cli_kqueue_lst의 shared_ptr를 소멸함
                iter->reset();
            }
            
            
            
            
            //kevent를 통해 nev를 형성
            int nev = kevent(S_cli_kq,
                             &*k_mon,
                             keve_idx,
                             &*k_tri,
                             keve_idx,
                             NULL);
            
            
            
            
            //만약 kevent의 에러가 발생했을 경우
            if(nev < 0){
                std::cout<<"[Info] : nev is fail"<<std::endl;
                throw errno;
            }else{
                for(int i = 0; i < keve_idx; i++){
                    
                    //클라이언트 디스크립터중 read_buffer가 왔을시에
                    auto c_list_itel = c_list.begin();
                    std::advance(c_list_itel, i);
                    
                    //만약 클라이언트가 shutdown이 됬을경우
                    if( ( &*k_tri )[i].flags & EV_EOF){
                        
                        //iterator로 c_list 리스트의 Client 인덱스 검색
                        auto c_list_it = c_list.begin();
                        std::advance(c_list_it, i);
                        
                        Close_Cli((*c_list_it));
                        
                        //해당 클라이언트의 디스크립터를 disable함
                        EV_SET(&(&*k_tri)[i], (*c_list_it)->get_cli_sck_info()->sck ,EVFILT_READ , EV_DELETE, NULL, NULL, NULL);
                        
                        
                        //break;로 탈출
                        break;
                        
                    }
                    
                    
                    //클라이언트가 EV_ERROR를 리턴했을시에
                    else if( ( &*k_tri )[i].flags & EV_ERROR){
                        std::cout<<"[Info : Client temp_trig_keven[i] is error"<<std::endl;
                        throw errno;
                    }
                    
                    //클라이언트 디스크립터 에 read buffer가 왔을시
                    else if((&*k_tri)[i].ident == (*c_list_itel)->get_cli_sck_info()->sck){
                        
                        //만약 쓰레드에 할당이 되었을 경우
                        if((*c_list_itel)->is_start() == true){
                            
                            //continue로 넘어감
                            continue;
                            
                            
                            //만약 아닐 경우
                        }else{
                            
                            //쓰레드를 replay()를 할당함
                            std::thread start(&Cuma_Server::Start_Cli, this, (*c_list_itel));
                            
                            //쓰레드를 리스트에 넣고 joinable로 관찰을 함
                            t_list.push_back(start);
                        }
                        
                    }
                }
            }
        }
        
        
        
    }catch(std::exception& e){
        std::cout<<"[Error] : Error res"<<e.what()<<std::endl;
        
        //바로 stop으로 넘어감
        return;
    }
}

//Cuma_server로 stop
void Cuma_Server::stop(){
    
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
int Cuma_Server::w_binary(const shared_ptr<Client>& c)
                        /*const std::string name,
                          const std::string binary,
                          const unsigned long byte)*/{
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
bool Cuma_Server::Start_Cli(shared_ptr<Client>& cli){
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
    
    
    shutdown(c->get_cli_sck_info()->sck, SHUT_RDWR);
    close(c->get_cli_sck_info()->sck);
    
}


//리시브할 함수
void Cuma_Server::rcv_val(shared_ptr<Client>& c){
    try{
        
        unsigned long s;        //파일 크기
        long t;                 //파일버퍼 템프
        unsigned long t1;
        char* f;                //파일 버퍼 temp
        string s_t;             //버퍼 스트링 temp
        
        //클라이언트 전송 버퍼 siz를 먼저 recv
        if(recv(c->get_cli_sck_info()->sck, &s , 8, 0) < 0){
            throw errno;
        }
        
        
        f = new char[s];
        
        
        //recv의 버퍼가 완전히 될때 까지 recv
        while(t1 < s){
            
            
            t = recv(c->get_cli_sck_info()->sck, f, s, 0);
            
            //만약 t가 0보다 작다면
            if(t < 0){
                throw errno;
            }
            
            //크기를 증가시킴
            t1 += t;
            
            s_t.append(f, t);
        }
        
        
        
    }catch(std::exception& e){
        
    }catch(Json::Exception& e){
        
    }
    
}

//보낼 함수
void Cuma_Server::snd_val(shared_ptr<Client>&c,
                          Json::Value& j){
    
}

