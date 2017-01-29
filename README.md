# Cuma_Server_Remake

##Cuma Project
Cluster unit memory architecture 의 약자인 Cuma는 다중 서버를 기반으로 파일 분할을 통해 온라인 파일 스토어를 하는 방식입니다.
Cuma 의 기초적인 작동 방식은 다음와 같습니다.

파일 전송프로토콜 로서 파일을 클라이언트에서 지정된 서버의 갯수만큼 프레임을 분활시켜서 전송을 하는 방식으로 송신시 지정된 서버에 접속을 하는것 이외 
중계노드에 접속 단계를 거치지 않습니다.


##Cuma_Server
본 소스는 Cuma 프로토콜의 서버 코어 소스로써 기본적인 코어 기능만을 포함하고 있습니다.

#install Guide

1. boost 패키지를 다운로드.
    ex) wget https://sourceforge.net/projects/boost/files/boost/1.63.0/
2. boost 패키지를 빌드합니다.
    ex) ./boostrap.sh
3. (2)번 과정 후 b2가 빌드가 boostreap를 b2로 빌드합니다.
    ex) ./b2
4. 빌드가 성공적이면 stage 디렉토리 와 boost 디렉토리 를 프로젝트 폴더에 이동시키십시오.
    ex) mv {Boost_Directory}/stage  {Project_Directory}/
    
5. JsonCpp 패키지를 다운받습니다. 
    ex) wget https://github.com/open-source-parsers/jsoncpp

6. JsonCpp 패키지를 빌드하여 헤더파일와 소스파일을 생성합니다. 
    ex) python amalgamate.py

7. 생성된 Jsoncpp/dist 디렉토리를 {Cuma_Server }/에 옮김니다. 
    ex) mv {Jsoncpp_location}/dist {Project_location}

8. Cmake를 이용하여 소스코드를 빌드합니다.
    command: <cmake> --build . [--config <config>] [--target <target>] [-- -i]
    ex) cmake --build .  {Install_Dir}                                  ==================> 반드시 --build 에 점이 들어가 있을것
    
9.  (8)번에서 빌드된 디렉토리 에서 build 명령어로 실행파일을 생성합니다.
    ex) build 
    
10. 끝

##Cuma Project
Cluster unit memory architecture (A.K.A Cuma) is way to store file based on Multi Base Server.
As File Send protocol, when file is requested send by Client, file splits amount of Selected Cuma_Server list count and send it direct to Server.
Althought there is no contect of any repeter server.

#Cuma_Server
About this Source is Server prototype Source that include Basic Core functional.

#Insatall Guide

1. download Boost library package
    ex) wget https://sourceforge.net/projects/boost/files/boost/1.63.0/
    
2. Build Boost package
    ex) ./boostrap.sh
    
3. after (2) sequence is end, build boostreap using b2 
    ex) ./b2
    
4. if the build is successful move "dirstage" directory & "boost" directory to This Project directory.
    ex) mv {Boost_Directory}/stage  {Project_Directory}/
    
5. Download JsonCpp package 
   ex) wget https://github.com/open-source-parsers/jsoncpp

6. Generate Header File and Source File after Build JsonCpp Package. 
   ex) python amalgamate.py

7. move to {Project_Directory}/ after generate Jsoncpp/dist directory
   ex) mv {Jsoncpp_location}/dist {Project_location}

8. Build the Source Code Using Cmake.
    command: <cmake> --build . [--config <config>] [--target <target>] [-- -i]
    ex) cmake --build .  {Install_Dir}                                  ==================> 반드시 --build 에 점이 들어가 있을것
    
9.  Generate execute file using "build" command in after build in (8) directory.
    ex) build 
    
10. done

CopyRight (C) 2016 laskdjlaskdj12 Allright Reserved.
