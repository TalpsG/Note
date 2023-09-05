import torch 
from torch.utils import data
from d2l import torch as d2l
from torch import nn

def load_array(data_array,batch_size,is_train=True):
    dataset = data.TensorDataset(*data_array)
    return data.DataLoader(dataset,batch_size,is_train)

true_w = torch.tensor([3.0,5.8])
true_b = torch.tensor(8)
features,labels = d2l.synthetic_data(true_w,true_b,1000)
batch_size = 10
data_iter = load_array([features,labels],batch_size)

loss = nn.HuberLoss()
net = nn.Sequential(nn.Linear(2,1))
net[0].weight.data.normal_(0,0.01)
net[0].bias.data.fill_(0)
trainer = torch.optim.SGD(net.parameters(),0.5)
epochs = 3
for epoch in range(epochs):
    for X,Y in data_iter:
        trainer.zero_grad()
        l = loss(net(X),Y)
        trainer.zero_grad()
        l.backward()
        trainer.step()
    l = loss(net(features),labels)
    print('epoch :',epoch,f'loss{l:f}')

