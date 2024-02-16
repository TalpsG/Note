# networking
## intro
## ethernet
以太网中frame就是发送数据前的包
```c
struct eth{
  uint8 dhost[6];
  uint8 shost[6];
  uint16 type;
}
```
里面有源和目的以太网地址，标识了唯一网卡
## arp
下图为arp 包的格式
![arp](./img/arp.png)

