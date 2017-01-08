//
//  main.cpp
//  Cuma_Server_Remake
//
//  Created by ace on 2016. 11. 25..
//  Copyright © 2016년 ace. All rights reserved.
//

#include <iostream>
#include <csignal>
#include <functional>
#include "Cuma_Server.hpp"

void sig_pipe_handler(int signo){
    switch (signo) {
        case SIGPIPE:{
            std::cout<<"[Error] : Sig_pipe has Occured "<<std::endl;
            exit(1);
            break;
        }
        default:
            break;
    }
}

int main(int argc, const char * argv[]) {
   
    Cuma_Server serv_sck;
    
    
    /*struct sigaction act;
    act.sa_handler = sig_pipe_handler;
    if(sigaction(SIGPIPE, &act, 0) == -1){
        std::cout<<"sig action init is fail"<<std::endl;
        serv_sck.reset();
        exit(1);
    }*/
    
    
    //포트설정
    serv_sck.set_prt(33391);
    serv_sck.set_active(true);
    
    
    //시작함
    serv_sck.start();
    
    
}
