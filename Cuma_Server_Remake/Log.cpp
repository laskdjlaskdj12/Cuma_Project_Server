//
//  Log.cpp
//  Cuma_Server_Remake
//
//  Created by ace on 2017. 1. 8..
//  Copyright © 2017년 ace. All rights reserved.
//

#include "Log.hpp"

void _CS_Log::operator()(const string& s){
    std::cout<<"[Info] : "<<s<<std::endl;
}
void _CS_Log::operator()(const string& s, bool b){
    if(b != true){  std::cout<<"[Info] : "<<s<< " false" <<std::endl;}
    else{   std::cout<<"[Info] : "<<s<< " true" <<std::endl; }
}
void _CS_Log::operator()(const string& s, unsigned long long l){
    std::cout<<"[Info] : "<<s<< ": "<<l<<std::endl;
}
