//
//  Log.hpp
//  Cuma_Server_Remake
//
//  Created by ace on 2017. 1. 8..
//  Copyright © 2017년 ace. All rights reserved.
//

#ifndef Log_hpp
#define Log_hpp

#include <iostream>
using std::string;

class _CS_Log{
public:
    void operator()(const string s);
    void operator()(const string s, bool b);
    void operator()(const string s, unsigned long long l);
    
    
    
};

#endif /* Log_hpp */
