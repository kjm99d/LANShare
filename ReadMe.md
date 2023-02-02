# LANShare

LAN(Local Area Network) 토폴로지에 연결된 네트워크상에서 파일을 공유하는 프로젝트입니다.

일반적으로 오프라인 환경에서 사용하기 적합합니다.



![image](/doc/background.jpg)

RESTAPI를 통해 서버에 연결된 클라이언트 상태를 조회합니다.

RESTAPI 또는 서버프로그램을 통해 각 클라이언트에게 파일공유, 메세지 전달 등을 수행할 수 있습니다.


### Supported REST API List
---
|URI|Method|Parameter|
|:---|:---:|:---|
|/SendAll|GET|QueryString </br>filePath - source file full path</br>filename - saved filename|
|/SendTo|GET|QueryString </br>address - target client address</br>filePath - source file full path</br>filename - saved filename|


## Requirements
---
1. vcredist

   - https://learn.microsoft.com/ko-KR/cpp/windows/latest-supported-vc-redist?view=msvc-170#visual-studio-2015-2017-2019-and-2022
2. LANShare Server & Client
   
   - ...


## Guide
---
### v1.0.0
1. Server 프로그램 실행
2. Client 프로그램 실행
3. Server 에서 커맨드를 통한 파일 전송
