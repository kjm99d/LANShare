# π‘ LANShare
[![MSBuild](https://github.com/kjm99d/LANShare/actions/workflows/msbuild.yml/badge.svg)](https://github.com/kjm99d/LANShare/actions/workflows/msbuild.yml)


LAN(Local Area Network) ν ν΄λ‘μ§μ μ°κ²°λ λ€νΈμν¬μμμ νμΌμ κ³΅μ νλ νλ‘μ νΈμλλ€.

μΌλ°μ μΌλ‘ μ€νλΌμΈ νκ²½μμ μ¬μ©νκΈ° μ ν©ν©λλ€.   

ν΄λΉ νλ‘μ νΈλ LANShare-WebConsole κ³Ό μ°λνμ¬ μ¬μ©ν  μ μμ΅λλ€.   
https://github.com/kjm99d/LANShare-WebConsole   
Demo : https://kjm99d.github.io/LANShare-WebConsole/   


![image](/doc/background.jpg)

RESTAPIλ₯Ό ν΅ν΄ μλ²μ μ°κ²°λ ν΄λΌμ΄μΈνΈ μνλ₯Ό μ‘°νν©λλ€.

RESTAPI λλ μλ²νλ‘κ·Έλ¨μ ν΅ν΄ κ° ν΄λΌμ΄μΈνΈμκ² νμΌκ³΅μ , λ©μΈμ§ μ λ¬ λ±μ μνν  μ μμ΅λλ€.


## π Supported REST API List
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
1. Server νλ‘κ·Έλ¨ μ€ν
2. Client νλ‘κ·Έλ¨ μ€ν
3. Server μμ μ»€λ§¨λλ₯Ό ν΅ν νμΌ μ μ‘


## π¨ Build
μλ κ³Όμ μ ν΅ν΄, μμ€μ½λμ μλΈλͺ¨λμ μλ°μ΄νΈν©λλ€.
```bash
git clone https://github.com/kjm99d/LANShare
cd LANShare
git submodule --init --recursive
```


## Wiki
[Wiki Link](https://github.com/kjm99d/LANShare/wiki)
