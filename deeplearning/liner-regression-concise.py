import numpy as np
import torch
from torch.utils import data
from d2l import torch as d2l
from torch import nn

true_w = torch.tensor([2,-3.4])
true_b = 4.2
features , labels = d2l.synthetic_data(true_w,true_b,1000)
def load_array(data_array,batch_size,is_train=True):
    dataset = data.TensorDataset(*data_array)
    return data.DataLoader(dataset,batch_size,shuffle=is_train)
batch_size = 10
data_iter = load_array((features,labels),batch_size)
#新建网络
#在PyTorch中，torch.nn.Sequential是一个方便的类，
# 用于按顺序组合多个神经网络层。
# 它允许你按照顺序定义和堆叠神经网络层，构建一个完整的神经网络模型。
net  = nn.Sequential(nn.Linear(2,1))

# 初始化weight 和 bias net[0]代表选择网络中的第一层
# 下面的意思是权值初始化为 在期望 0方差0.01的正态分布上随机取值
# bias则全部为0
net[0].weight.data.normal_(0,0.01)
net[0].bias.data.fill_(0)

# 损失函数，使用的是L2范数，他返回所有样本loss的均值
loss = nn.HuberLoss()

# 定义优化算法 
# SGD只需要要优化的参数和学习率两个参数
# 下面的trainer 用net.parameters获得了所有的参数
trainer = torch.optim.SGD(net.parameters(),lr=0.3)


# 训练
num_epochs = 3
for epoch in range(num_epochs):
    for X,y in data_iter:
        l = loss(net(X),y)
        trainer.zero_grad()
        l.backward()
        # trainer通过step自动修改参数了
        trainer.step() 
    l = loss(net(features),labels)
    print(f'epoch {epoch+1},loss {l:f}')

w = net[0].weight.data
b = net[0].bias.data
print('w 误差:',true_w-w.reshape(true_w.shape))
print('b 误差:',true_b-b)