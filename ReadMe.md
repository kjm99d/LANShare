# 💡 LANShare
[![MSBuild](https://github.com/kjm99d/LANShare/actions/workflows/msbuild.yml/badge.svg)](https://github.com/kjm99d/LANShare/actions/workflows/msbuild.yml)


LAN(Local Area Network) 토폴로지에 연결된 네트워크상에서 파일을 공유하는 프로젝트입니다.

일반적으로 오프라인 환경에서 사용하기 적합합니다.   

해당 프로젝트는 LANShare-WebConsole 과 연동하여 사용할 수 있습니다.   
https://github.com/kjm99d/LANShare-WebConsole   
Demo : https://kjm99d.github.io/LANShare-WebConsole/   


![image](/doc/background.jpg)

RESTAPI를 통해 서버에 연결된 클라이언트 상태를 조회합니다.

RESTAPI 또는 서버프로그램을 통해 각 클라이언트에게 파일공유, 메세지 전달 등을 수행할 수 있습니다.


## 📃 Supported REST API List
---
|URI|Method|QueryString|Content-Type|Body|Response Example|
|:---|:---:|:---|:---|:---|:---|
|/status|GET||JSON||{</br>&nbsp;&nbsp;&nbsp;"data" : 0,</br>&nbsp;&nbsp;&nbsp;"message" : "success",</br>&nbsp;&nbsp;&nbsp;"status_code" : 200</br>}|
|/SendAll|POST||JSON|filepath - source file full path</br>filename - saved filename|
|/SendTo|POST||JSON|address - target client address</br>filepath - source file full path</br>filename - saved filename|
|/HeartBeat|GET|||  |[ { "address" : "127.0.0.1", "status" : true } ]|
|/ftp|GET|||path - search path |[ { "address" : "127.0.0.1", "status" : true } ]|


## Requirements
1. vcredist

   - https://learn.microsoft.com/ko-KR/cpp/windows/latest-supported-vc-redist?view=msvc-170#visual-studio-2015-2017-2019-and-2022
2. LANShare Server & Client
   
   - ...


## User GUIDE
### v1.0.0
1. Server 프로그램 실행
2. Client 프로그램 실행
3. Server 에서 커맨드를 통한 파일 전송


## 🔨 Build
아래 과정을 통해, 소스코드와 서브모듈을 업데이트합니다.
```bash
git clone https://github.com/kjm99d/LANShare
cd LANShare
git submodule --init --recursive
```


## Wiki
[Wiki Link](https://github.com/kjm99d/LANShare/wiki)
