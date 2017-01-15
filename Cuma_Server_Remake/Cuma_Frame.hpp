//
//  Cuma_Frame.hpp
//  Cuma_Server_Remake
//
//  Created by ace on 2017. 1. 15..
//  Copyright © 2017년 ace. All rights reserved.
//

#ifndef Cuma_Frame_hpp
#define Cuma_Frame_hpp

#include <iostream>
#include <json/json.h>
#include <cstring>

using std::string;

class Cuma_Frame{
public:
    Cuma_Frame();
    Cuma_Frame(string fr_n,
               unsigned int fr_num,
               unsigned long long fr_siz);
    ~Cuma_Frame();
    
    unsigned long long  fr_siz();           //프레임 사이즈
    string              fr_binary();        //프레임 바이너리
    string              fr_name();          //프레임 이름
    string              f_name();           //파일 이름
    unsigned int        fr_num();           //프레임 넘버
    Json::Value         fr_json();          //프레임 json
    
    void  set_fr_siz(unsigned long long l);
    void  set_fr_binary(string s);
    void  set_fr_name(string s);
    void  set_f_name(string s);
    void  set_fr_num(unsigned int num);
    void  set_fr_json(Json::Value v);
private:
    unsigned long long  fr_siz_;                 //프레임 사이즈
    string              fr_binary_;              //프레임 바이너리
    string              fr_name_;                //프레임 이름
    string              f_name_;                 //파일 이름
    unsigned int        fr_num_;                 //프레임 넘버
    Json::Value         fr_json_;                 //프레임 json
};

#endif /* Cuma_Frame_hpp */
