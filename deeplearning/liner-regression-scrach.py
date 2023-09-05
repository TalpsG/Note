import random
import torch
from d2l import  torch as d2l

def generate_data(w,b,num):
    X = torch.normal(0,1,(num,len(w)))
    y = torch.matmul(X,w)+b
    y += torch.normal(0,0.01,y.shape)
    return X,y.reshape((-1,1))
w = torch.normal(0,0.01,size=(2,1),requires_grad=True)
b = torch.zeros(1,requires_grad=True)

def linreg(X,w,b):
    return torch.matmul(X,w)+b
    
def square_loss(y_hat,y):
    return (y_hat-y.reshape(y_hat.shape))**2/2

def sgd(params,learning_rates,batch_size):
    with torch.no_grad():
        for param in params:
            param -= learning_rates*param.grad/batch_size
            param.grad.zero_()

def data_iter(batch_size,features,labels):
    num = len(features)
    indices = list(range(num))
    random.shuffle(indices)
    for i in range(0,num,batch_size):
        batch_indice = torch.tensor(indices[i:min(i+batch_size,num)])
        print(torch.tensor(indices[i:min(i+batch_size,num)]))
        yield features[batch_indice],labels[batch_indice]

lr = 0.03
num_epochs = 3
net = linreg
loss = square_loss
batch_size = 10
true_w = torch.tensor([2, -3.4])
true_b = 4.2
features, labels = generate_data(true_w, true_b, 1000)

for epoch in range(num_epochs):
    for X,y in data_iter(batch_size,features,labels):
        l = loss(net(X,w,b),y)
        l.sum().backward()
        sgd([w,b],lr,batch_size)
        with torch.no_grad():
            train_l = loss(net(features,w,b),labels)
            print(f'epoch {epoch+1} ,loss {float(train_l.mean()):f}')