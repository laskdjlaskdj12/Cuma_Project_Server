//
//  main.cpp
//  Cuma_Server_Remake
//
//  Created by ace on 2016. 11. 25..
//  Copyright © 2016년 ace. All rights reserved.
//

#include <iostream>
#include "Cuma_Server.hpp"

int main(int argc, const char * argv[]) {
   
    //Cuma_Server의 shared_ptr를 제공함
    shared_ptr<Cuma_Server> serv_sck = std::make_shared<Cuma_Server>();
    
    //포트설정
    serv_sck->set_prt(33391);
    serv_sck->set_active(true);
    
    
    //시작함
    serv_sck->start();
    
    
}
