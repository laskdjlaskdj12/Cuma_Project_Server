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
void Cuma_Server::set_prt(const int p){
    cuma_sck->set_prt(p);
}


//서버의 listen 상태를 설정후 listen된 클라이언트들은 Start_cli로 보냄
void Cuma_Server::start(){
    try{
        
       
        //소켓을 Cuma_Sck클래스에서 구성
        cuma_sck->start();
        log(CS_START,true);
        
        
        
        
        //서버소켓에 kqueue를 등록
        EV_SET((&*CS_srv_kevent), (&*cuma_sck->get_Serv_Sock())->get_sck(), EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, 0);
        log(CS_EV_SET,true);
        
        
        //클라이언트가 connect 될때가지 listen
        listen(cuma_sck->get_Serv_Sock()->get_sck(),128);
        log(CS_EV_LST,true);
        
        
        //만약 is_start == true
        while(is_start){
            
            int nev = kevent(S_srv_kq,
                         (&* CS_srv_kevent), 1,
                         (&* CS_srv_kevent_t), 1,
                         NULL);
            log(CS_KEVENT,true);
            
            
            
            
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
                    shared_ptr<Client> Client_temp(new Client);
                    
                    
                    
                    //클라이언트의 접속을 허가
                    Client_temp->get_cli_sck_info()->sck = accept(cuma_sck->get_Serv_Sock()->get_sck(),
                                                                  (sockaddr*)&(Client_temp->get_cli_sck_info()->cli_sck_addr),
                                                                  (socklen_t*)&(Client_temp->get_cli_sck_info()->siz));
                    log(CS_ACCEPT,true);
                    
                    
                    //만약 128개의 사이즈가 넘었다면 큐 부족으로 connect false리턴
                    if(c_list.size() > 128){
                        
                        //에러코드를 클라이언트에게 전송
                        Json::Value ERR;
                        ERR["RCV_ERR"] = "NO_SPACE_QUEUE";
                        
                        
                        snd_val(Client_temp, ERR);
                        
                        //접속한 클라이언트 연결 종료
                        shutdown(Client_temp->get_cli_sck_info()->sck, SHUT_RDWR);
                        
                        //cli_tmp 디스크립터 close
                        close(Client_temp->get_cli_sck_info()->sck);
                        
                        
                        //Client object reset
                        Client_temp.reset();
                        ERR.clear();
                        log(CS_QUEUE_FULL,true);
                        
                        //만약 중복 재접속 했을 경우
                    }else if(std::find(Cli_des.begin(),
                                       Cli_des.end(),
                                       Client_temp->get_cli_sck_info()->sck) != Cli_des.end()){
                        
                        //에러코드를 클라이언트에게 전송
                        Json::Value ERR;
                        ERR["RCV_ERR"] = "SECOUND_SPACE";
                        
                        snd_val(Client_temp, ERR);
                        
                        
                        //접속한 클라이언트 연결 종료
                        shutdown(Client_temp->get_cli_sck_info()->sck, SHUT_RDWR);
                        
                        //cli_tmp 디스크립터 close
                        close(Client_temp->get_cli_sck_info()->sck);
                        
                        
                        //Client object reset
                        Client_temp.reset();
                        ERR.clear();
                        log(CS_QUEUE_DUP,true);
                        
                        
                        //정상적인 접속일 경우
                    }else{
                        
                        //정상코드를 클라이언트에게 전송
                        Json::Value CON;
                        CON["RCV_CLR"] = "CONNECT_CLEAR";
                        log(CS_JSON_RCV_CLR,true);
                        
                        snd_val(Client_temp, CON);
                        
                        
                        //접속된 클라이언트를 실행할 쓰레드 할당
                        auto thread_t = std::thread(&Cuma_Server::Start_Cli, this,Client_temp);
                        log(CS_THREAD_ALLOC,true);
                        
                        //클라이언트 객체제 할당된 쓰레드 insert
                        Client_temp->set_thread_id(move(thread_t));
                        
                        //쓰레드 큐에 push_back
                        t_list.push_back(move(thread_t));
                        
                        //Client 리스트에 push
                        c_list.push_back(Client_temp);
                        
                        //클라이언트 디스크립터에 push
                        Cli_des.push_back(Client_temp->get_cli_sck_info()->sck);
                        
                        
                        std::cout<<"cli_tmp use_count : "<<Client_temp->get_cli_sck_info().use_count()<<std::endl;
                        std::cout<<"Client_temp use_count : "<<Client_temp.use_count()<<std::endl;
                        
                        std::cout<<"t_f_list_siz : "<<t_j_list_.size()<<std::endl;
                        
                        Client_temp.reset();
                        CON.clear();
                        
                        //만약 t_j_list가  5이상 되면 join을 함
                        if(t_j_list_.size() > 1 ){
                            log(CS_THREAD_FLUSH,true);
                            //delete thread
                            delete_thread();
                            
                        }
                    }
                }
                
            }
        }
        
        //만약 is_start 가 false가 되었을 경우 cuma_sck->get_Serv_Sock()의 stop으로 넘어감
        stop();
        log(CS_STOP,true);
        
    }catch(std::exception& e){
        std::cout<<"[Error] : Error res"<<e.what()<<std::endl;
        
        //바로 stop으로 넘어감
        return;
    }catch(int err_num){
        std::cout<<"[Error] : Error res"<<std::strerror(err_num)<<std::endl;
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
        
        //프레임 파일을 읽어서 string으로 리턴을 함    (파일이름 : 파일.Cuma_Client(프레임 숫자))
        f_read.open(name+".Cuma_Client"+std::to_string(c->get_f_frame()),std::ios::binary | std::ios::beg);
        
        
        //만약 파일이 존재하지 않는다면
        if(f_read.is_open() == false){
            throw string("FILE_NOT_EXSIST");
        }
        
        //파일의 포인터를 마지막으로 옮김
        f_read.seekg(std::ios::beg,std::ios::end);
        
        //streampos으로 file_read의 크기를 구함
        size_t f_siz = f_read.tellg();
        
        //파일의 포인터를 제자리로 돌려둠
        f_read.seekg(std::ios::beg);
        
        //Client 파라미터에 파일 사이즈를 등록
        c->set_f_siz(f_siz);
        
        
        //file_buf로 new char[]을 할당
        char* file_buf = new char[512];
        
        
        //file_read로 읽음
        while(!f_read.eof()){
            
            f_read.read(file_buf, 512);
            f_temp.append(file_buf, 512);
            memset(file_buf,0,512);
            
        }
        
        
        //리턴할 string에 해당 파일 크기 만큼 바이너리를 input
        
        
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
        
        
        std::string name = c->get_f_name();
        std::string binary = c->get_file();
        unsigned long byte = c->get_f_siz();
        std::stringstream f_name;
        //뮤텍스 lock을 함
        mtx_lock.lock();
        
        //파일 이름을 (파일이름)+(.Cuma_Client)+(프레임넘버)
        f_name<<name<<".Cuma_Client"<<std::to_string(c->get_f_frame());
        
        //파일 쓰기(바이너리로 파일을 오픈)
        file_write.open(f_name.str(),std::ios::binary | std::ios::beg);
        
        
        //파일을 입력하기
        file_write.write(binary.c_str(), byte);
        
        
        file_write.close();
        
        //뮤텍스 unlock
        mtx_lock.unlock();
        
        name.clear();
        binary.clear();
        
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
        log(CS_START_RCV);
        
        //val_tmp로 클라이언트로부터 수신을 받음
        Json::Value val_tmp = cli->get_json();
        log(CS_START_SET_CLI_OBJ);
        
        //만약 클라이언트가 서버 상태 체크하는 접속시
        if(val_tmp["Active"].asBool() != false || !val_tmp["Active"].isNull()){
            val_tmp["Active"] = true;
            
            //val_tmp를 전송함
            snd_val(cli, val_tmp);
            log(CS_START_SND);
            
            val_tmp.clear();
            log(CS_START_JSON_CLR);
            
            //클라이언트 접속 connect 셧다운
            Close_Cli(cli);
            log(CS_START_CLOSE_CLI);
            
            return true;
        }
        
        
        
        //클라이언트에서 요청한 모드 확인
        switch (val_tmp["MODE"].asInt()){
                
            case READ_BINARY:{              //파일 참조 요청일시
                
                cli->set_f_name(val_tmp["F_name"].asString());
                cli->set_f_frame(val_tmp["F_frame_num"].asInt());
                
                //바이너리 읽기
                if(r_binary(cli) < 0){
                    throw string("NOFILE");
                }
                log(CS_START_READ_BINARY);
                
                //인덱스 json을 clear
                val_tmp.clear();
                
                //파일 이름, 크기, 바이너리 추가
                val_tmp["F_name"] = cli->get_f_name();
                val_tmp["F_siz"] = (Json::UInt64)cli->get_f_siz();
                val_tmp["F_frame_num"] = cli->get_f_frame();
                val_tmp["F_binary"] = cli->get_file();
                log(CS_START_ADD_JSON);
                
                
                break;
            }
                
            case WRITE_BINARY:{             //파일 저장 요청일시
                
                //클라이언트 객체의 파일 설정
                cli->set_f_name(val_tmp["F_name"].asString());
                cli->set_f_siz(val_tmp["F_siz"].asLargestUInt());
                cli->set_file(val_tmp["F_binary"].asString());
                cli->set_f_frame(val_tmp["F_frame_num"].asUInt64());
                
                //바이너리 쓰기
                if(w_binary(cli) < 0){
                    throw string("NOWRIGHT");
                }
                log(CS_START_WRITE_BINARY);
                
                //인덱스 json을 clear
                val_tmp.clear();
                
                //파일 이름, 크기, 바이너리 추가
                val_tmp["F_name"] = cli->get_f_name();
                val_tmp["F_siz"] = (Json::UInt64)cli->get_f_siz();
                val_tmp["F_frame_num"] = cli->get_f_frame();
                log(CS_START_ADD_JSON);
                
                
                
                break;
            }
                
            default:{    //파일모드가 잘못되었을시
                log(CS_START_WRONG_MOD);
                throw string("Wrong_MODE");
                break;
            }
        }
        
        //val_tmp를 전송함
        snd_val(cli, val_tmp);
        log(CS_START_SND);
        
        val_tmp.clear();
        log(CS_START_JSON_CLR);
        
        //클라이언트 접속 connect 셧다운
        Close_Cli(cli);
        log(CS_START_CLOSE_CLI);
        
        return true;
        
        //만약 exception이 났을 경우
    }catch(std::exception& e){
        
        Json::Value snd_err;
        //에러 문구 출력
        std::cout<<"[Error] "<<e.what()<<std::endl;
        
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
        
        snd_err["Error"] = "JSON_EXCEP";
        snd_err["Reason"] = e.what();
        
        //클라이언트로 전송
        snd_val(cli,snd_err);
        
        //클라이언트 접속 connect 셧다운
        Close_Cli(cli);
        
        
        //false를 리턴함
        return false;
        
        
    }catch(const std::string& e){
        Json::Value snd_err;
        std::cout<<"[Error] : Error of "<<e<<std::endl;
        
        //클라이언트에게 exception이 났다고 전달을 함
        
        snd_err["Error"] = "Client";
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
    
    thr_lst_lck_.lock();
    //해당 스레드 인스턴스를 pushback
    t_j_list_.push_back(move(c->get_thread_id()));
    log(CS_CLOSE,true);
    thr_lst_lck_.unlock();
    
    
    //큐에 등록된 Client_list, Cliet_sck_info를 remove 함
    c_list.remove(c);
    Cli_des.remove(c->get_cli_sck_info()->sck);
    log(CS_CLOSE_REM_SCK);
    
    
    //해당 소켓 디스크립터를 shutdown함
    c->stop();
    log(CS_CLOSE_SCK_STOP);
    
    
    //Client의 객체를 소멸시키기
    std::cout<<"참조 타임 : "<<c.use_count()<<std::endl;
    c.reset();
    std::cout<<"참조 타임 : "<<c.use_count()<<std::endl;
}


//리시브할 함수
void Cuma_Server::rcv_val(shared_ptr<Client>& c){
    try{
        
        //변수들
        unsigned long long f_siz = 0;        //파일 크기
        long long f_tmp;            //파일버퍼 템프
        unsigned long long f_tmp1 = 0;
        char* f_bin_tmp;                     //파일 버퍼 temp
        string f_bin_s_tmp;                  //버퍼 스트링 temp
        Json::Value rcv_f;
        
        
        
        //클라이언트 전송 버퍼 siz를 먼저 recv
        if(recv(c->get_cli_sck_info()->sck, &f_siz , 8, 0) < 0){
            throw errno;
        }
        
        //리시브 버퍼 temp를 생성
        f_bin_tmp = new char[f_siz];
        
        
        //recv의 버퍼가 완전히 될때 까지 recv
        while(f_tmp1 < f_siz){
            
            //파일 버퍼를 수신
            f_tmp = recv(c->get_cli_sck_info()->sck, f_bin_tmp , f_siz, 0);
            
            //만약 t가 0보다 작다면
            if(f_tmp < 0){
                throw errno;
            }
            
            //크기를 증가시킴
            f_tmp1 += f_tmp;
            
            //append를 함
            f_bin_s_tmp.append(f_bin_tmp, f_tmp);
        }
        
        //수신받은 temp를 json으로 세팅함
        rcv_f = c->cha_to_json(f_bin_s_tmp);
        
        //클라이언트의 json 멤버변수에 등록함
        c->set_json(rcv_f);
        
        rcv_f.clear();
        f_bin_s_tmp.clear();
        
        //f를 딜리트
        delete[] f_bin_tmp;
        
        //리턴
        return;
        
    }catch(std::exception& e){
        std::cout<<"[Error] : "<<e.what()<<std::endl;
        return;
    }catch(Json::Exception& e){
        std::cout<<"[Error] : "<<e.what()<<std::endl;
        return;
    }catch(unsigned int& e){
        std::cout<<"[Error] : "<<std::strerror(e)<<std::endl;
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
    
    temp.clear();
    
}

void Cuma_Server::delete_thread(){
    
    for(int i = 0; i < t_j_list_.size(); i++){
        
        if( t_j_list_[i].joinable()){
            t_j_list_[i].join();
        }
    }
    t_j_list_.clear();
}
