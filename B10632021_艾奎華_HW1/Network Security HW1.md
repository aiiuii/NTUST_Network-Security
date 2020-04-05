# **Network Security HW1**

###### tags: `NTUST` `NS`

## 建置環境與使⽤說明
ubuntu 18.04 : gcc & g++
### Run
```
make
./server [-k] [port]
```

> ***-k*** - Turn on TCP Keepalive
> ***port*** - Default == 8080
> 

### Test
- 使用 Chrome
- `127.0.0.1:${port}` ，應能看見 `B10632021 NS HW1` 
- `127.0.0.1:${port}/env.cgi`，應能看見環境參數的表格
- `127.0.0.1:${port}/mul.cgi?m=%ld&n=%ld`，應能看見 `The product of ${m} and ${n} is ${m*n}.`
- `127.0.0.1:${port}/${invaild_path}`，應能看見 404 Not Found




## 重要程式碼說明
    void setSocketTCP(int *sockfd, int keepalive, char *port);
    
> ***sockfd*** - socket 的檔案描述符 
> ***keepalive*** - 決定 socket 是否啟用 keep-alive
> ***port*** - Server 採用的 port

使用 socket(), bind(), listen() 設定好 Server

---
    void parseHTTP(char *inputData, char *outputData, ssize_t size);
    
> ***inputData*** - 接收 Client 傳入的 request 
> ***outputData*** - 存放 Server 將要傳出的 response
> ***size*** - Client 傳入的 request 的大小

分析 recv() 接收到的資料，只接收 GET request，若符合格式則呼叫對應的 CGI program 並使用 setenv() 將資訊傳入 CGI

---
    void executeCGI(char *filename, char *outputData);

> ***filename*** - 將要執行的 CGI 檔名
> ***outputData*** - CGI 執行後傳出的資料
> 
Base on "CGI_example/host.c". 

## 成果截圖
![](https://i.imgur.com/thJsDi0.png)
![](https://i.imgur.com/pL9QwxD.png)
![](https://i.imgur.com/B9tNV2h.png)
![](https://i.imgur.com/CgazzBh.png)
![](https://i.imgur.com/KEikQmS.png)

## 困難與⼼得

由於之前都是用 Windows 沒接觸過 Linux 環境，這次的作業光是一開始要 compile CGI_example 就讓我卡關了。第一次做關於網路的程式，感覺學到了不少新知識，棒！