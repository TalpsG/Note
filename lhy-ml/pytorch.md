# pytorch
## dataset & dataloader
`dataset`就是存放训练训练所需的数据对象
`dataloader`就是读取数据的。

重写的`dataset`类包括了一些方法，供`dataloader`调用然后生成`batch`以供训练。

```python
# tensor直接生成对应的张量
torch.tensor([1,-1])
# zeors 和 ones 是生成对应形状的填充过0 或1 的张量
torch.zeros([2,2])
```
`x.transpose(n1,n2)`可以将张量的维度做转换
`x.sequeeze(n)`可以挤压掉一个维度
`x.unsequeeze(n)`可以添加一个维度

`torch.cat([x,y,z],dim=n)`可以合并张量


### 梯度计算
```python
x = torch.tensor([1,2,3],requires_grad=True)
# 需要梯度的参数
z = x.pow(2).sum()
z.backward() 
# 会计算z之前的张量的梯度也就是x的
x.grad
```


## 定义网络
```python
layer = torch.nn.Linear(32,64) 
layer.weight.shape
layer.bias.shape
nn.Sigmoid()
nn.ReLU()
```
先定义自己的模型
```python
class Mymodel(nn.Module):
  def __init__(self):
    super(Mymodel,self).__init__()
    self.net = nn.Sequential(
      nn.Linear(10,32),
      nn.Sigmoid(),
      nn.Linear(32,1)
    )
  def forward(self,x):
    return self.net(x)
```
上述代码定义了一个网络，`forward`返回网络的结果。
也可以不用`Sequential`包起来网络


```python
class Mymodel(nn.Module):
  def __init__(self):
    super(Mymodel,self).__init__()
    self.layer1 = nn.Linear(10,32)
    self.layer2 = nn.Sigmoid()
    self.layer3 = nn.Linear(32,1)
  def forward(self,x):
    res = self.layer1(x)
    res = self.layer2(res)
    res = self.layer3(res)
    return res
```

## 损失函数
`nn`下有很多损失函数，下面举例用法
```python
lossfunc = nn.MSELoss()
lossfunc = nn.CrossEntropyLoss()
loss = lossfunc(model_output,expect_value)
```


## 优化optim
`torch.optim.SGD(model.parameters(),lr,momentum = 0)`


对于每一个`batch`
1. 将上一轮的梯度清零`optimizer.zero_grad()`
2. `loss.backward`求出梯度
3. `optimizer.step()`调整参数


## 总结
训练前的准备大致代码如下
```python
dataset = Mydataset(file)
tr_set = DataLoader(dataset,16,shuffle= True)
model = Mymodel().to(device) # device = cpu or cuda
criterion = nn.MSELoss()
optimizer = torch.optim.SGD(model.parameters(),0.1)

```

训练代码大致
```python
for epoch in range(n_epoch):
  model.train() # 将模型调整至训练模式
  for x,y in tr_set:
    optimizer.zero_grad()
    x,y = x.to(device) ,y.to(device)
    pred = model(x)
    loss= criterion(pred,y)
    loss.backward()
    optimizer.step()
```


测试的部分大致很像训练的代码

```python
model.eval() # 将模型调整至评估模式
total_loss = 0
for x,y in dv_set:
  x,y = x.to(device) ,y.to(device)
  with torch.no_grad():
    pred = model(x)
    loss= criterion(pred,y)
  total_loss += loss.cpu().item()* len(x)
  avg_loss = total_loss / len(dv_set.dataset)
```


**为什么要`model.eval()`,某些模型的层在训练和评估的行为会不一样**

## 保存加载模型
保存:`torch.save(model.state_dict(),path)`
加载:
`ckpt = torch.load(path)`
`model.load_state_dict(ckpt)`




## 实际使用
### 数据

可以使用`train = pandas.read_csv(path,drop(columns=['c1,c2'])).values`读取`csv`数据
之后使用`x ,y = train[:,:-1],tran_data[:,-1]`


### 数据集
数据集类需要三个函数
```python
class mydata(Dataset):
  def __init__(self,x,y=None):
    if y is None:
      self.y = y
    else:
      self.y = torch.FloatTensor(y)
    self.x = x
  def __getitem__(self,idx):
    if self.y is None:
      return self.x[idx]
    else:
      return self.x[idx].self.y[idx]
  def __len__(self):
    return len(self.x)
```

### dataloader
`train_loader = DataLoader(train_dataset,batch,shuffle=True,pin_memory = True)`


### 设置模型
```python
class Mymodel(nn.Module):
  def __init__(self,input_dim):
    self.layers = nn.Sequential(
      nn.Linear(input_dim,64)
      nn.ReLU(),
      nn.Linear(64,32),
      nn.ReLU(),
      nn.Linear(32,1)
    )
  def forward(self,x):
    x = self.layers(x)
    x = x.sequeeze(1)
    return x
```
### 设置损失函数
`criterion = torch.nn.MSELoss(reduction='mean')`

### 优化
`optimizer = torch.optim.SGD(model.parameters(),lr=1e-5,momentum=0.9)`


### 训练
```python
for epoch in range(n):
  model.train()
  train_pbar = tqdm(train_loader,position= 0,leave=True)
  for x,y in train_pbar:
    x,y = x.to('cuda'),y.to('cuda')
    pred = model(x)
    loss = criterion(pred,y)
    loss.backward()
    optimizer.step()
    optimizer.zero_grad()
```
