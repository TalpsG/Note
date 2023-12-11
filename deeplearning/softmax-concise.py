import torch
from torch import nn
from d2l import torch as d2l

batchsize = 256
train_iter, test_iter = d2l.load_data_fashion_mnist(batchsize)

net = nn.Sequential(nn.Flatten(), nn.Linear(784, 10))
print(net)
