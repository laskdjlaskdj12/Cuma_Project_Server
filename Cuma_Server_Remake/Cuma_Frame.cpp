//
//  Cuma_Frame.cpp
//  Cuma_Server_Remake
//
//  Created by ace on 2017. 1. 15..
//  Copyright © 2017년 ace. All rights reserved.
//

#include "Cuma_Frame.hpp"

Cuma_Frame::Cuma_Frame():fr_siz_(0){
    
}
Cuma_Frame::Cuma_Frame(string fr_n,
                       unsigned int fr_num,
                       unsigned long long fr_siz){
    fr_name_ = fr_n;
    fr_num_ = fr_num;
    fr_siz_ = fr_siz;
    
}
Cuma_Frame::~Cuma_Frame(){}

unsigned long long Cuma_Frame::fr_siz(){ return fr_siz_; }
string Cuma_Frame::fr_binary(){ return fr_binary_; }
string Cuma_Frame::fr_name(){ return fr_name_; }
string Cuma_Frame::f_name(){ return f_name_; }
unsigned int Cuma_Frame::fr_num(){  return fr_num_; }
Json::Value  Cuma_Frame::fr_json(){ return fr_json_; }


void  Cuma_Frame::set_fr_siz(unsigned long long l){ fr_siz_ = l; }
void  Cuma_Frame::set_fr_binary(string s){ fr_binary_ = s; }
void  Cuma_Frame::set_fr_name(string s){ fr_name_ = s; }
void  Cuma_Frame::set_f_name(string s){ f_name_ = s; }
void  Cuma_Frame::set_fr_num(unsigned int num){ fr_num_ = num; }
void Cuma_Frame::set_fr_json(Json::Value j){ fr_json_ = j; }
