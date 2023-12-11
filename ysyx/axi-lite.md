# axi lite
读地址通道

araddr 
arvalid 地址有效
arready Slave 准备好接收地址


读数据

rdata 读出的数据
rresp 读操作成功信号
rvalid 读出的数据有效
rready 准备好读数据 


写地址
awaddr 地址
awvalid 读地址有效
awready 准备好接受地址

写数据
wdata
wstrb mask
wvalid 写数据有效
wready slave准备好接收写数据

bresp 写成功
bvalid bresp的有效信号
bready 准备好接收bresp
