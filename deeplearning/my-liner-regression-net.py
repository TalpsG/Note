import random
from d2l import torch as d2l
import torch
import numpy


def generate_data(w,b,num):
    features = torch.normal(0,1,(num,len(w)))
    labels = torch.matmul(features,w)+b
    return features,labels

def liner_regression_net(X,w,b):
    return  torch.matmul(X,w)+b

def square_loss(y_hat,y):
    return (y_hat-y.reshape(y_hat.shape))**2/2


def sgd(params,lr,batch_size):
    with torch.no_grad():
        for param in params:
            param -= lr*param.grad/batch_size
            param.grad.zero_()

def data_iter(features,labels,batch_size):
    num = len(features)
    indices = list(range(num))
    random.shuffle(indices)
    for i in range(0,num,batch_size):
        batch_indices = indices[i:min(i+batch_size,num)]
        yield features[batch_indices],labels[batch_indices]


true_w = torch.tensor([3.0])
true_b = 5
num = 1000 
features,labels = generate_data(true_w,true_b,num)

batch_size = 10
epochs = 3
w = torch.zeros(true_w.shape,requires_grad=True)
b = torch.tensor(0.0,requires_grad=True)

lr = 0.05

img = []
x_axis = []

for epoch in range(3):
    for X,Y in data_iter(features,labels,batch_size):
        loss = square_loss(liner_regression_net(X,w,b),Y)
        img.append(float(loss.mean()))
        x_axis.append(epoch)
        loss.sum().backward()
        sgd([w,b],lr,batch_size)
        with torch.no_grad():
            loss = square_loss(liner_regression_net(features,w,b),labels)
            img.append(float(loss.mean()))
            x_axis.append(epoch)
    
d2l.plot(x_axis,img)
d2l.plt.show()