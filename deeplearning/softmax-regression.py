import torch
import torchvision
from torch.utils import data
from torchvision import transforms
from d2l import torch as d2l


d2l.use_svg_display()

trans = transforms.ToTensor()

minist_train = torchvision.datasets.FashionMNIST(
    root='/home/talps/gitrepo/d2l-zh/pytorch/data', train=True, transform=trans, download=False)
minist_test = torchvision.datasets.FashionMNIST(
    root='/home/talps/gitrepo/d2l-zh/pytorch/data', train=False, transform=trans, download=False)

print(len(minist_test), len(minist_train))


def get_fashion_minist_lables(labels):
    text_labels = ['tee', 'trouser', 'pullover', 'dress',
                   'coat', 'sandal', 'shirt', 'sneaker', 'bag', 'ankel boot']
    return [text_labels[int(i)] for i in labels]


def show_images(imgs, num_rows, num_cols, titles=None, scale=1.5):
    figsize = (num_cols*scale, num_rows*scale)
    _, axes = d2l.plt.subplots(num_rows, num_cols, figsize=figsize)
    axes = axes.flatten()
    for i, (ax, img) in enumerate(zip(axes, imgs)):
        if torch.is_tensor(img):
            ax.imshow(img.numpy())
        else:
            ax.imshow(img)
        ax.axes.get_xaxis().set_visible(False)
        ax.axes.get_yaxis().set_visible(False)
        if (titles):
            ax.set_title(titles[i])
    return axes


batch_size = 256


def get_dataloader_workers():
    return 4


timer = d2l.Timer()

train_iter = data.DataLoader(minist_train, batch_size=batch_size,
                             shuffle=True, num_workers=get_dataloader_workers())

X, y = next(iter(data.DataLoader(dataset=minist_train, batch_size=18)))
show_images(X.reshape(18, 28, 28), 2, 9, titles=get_fashion_minist_lables(y))

timer = d2l.Timer()
for X, y in train_iter:
    continue
print(f'{timer.stop():.2f} sec')


def load_data_fashion_minist(batch_size, resize=None):
    trans = [transforms.ToTensor()]
    if resize:
        trans.insert(0, transforms.Resize(resize))
    trans = transforms.Compose(trans)
    minist_train = torchvision.datasets.FashionMNIST(
        root='/home/talps/gitrepo/d2l-zh/pytorch/data', train=True, transform=trans, download=False)
    minist_test = torchvision.datasets.FashionMNIST(
        root='/home/talps/gitrepo/d2l-zh/pytorch/data', train=False, transform=trans, download=False)
    return (data.DataLoader(dataset=minist_train, batch_size=batch_size, shuffle=True, num_workers=get_dataloader_workers()), data.DataLoader(dataset=minist_test, batch_size=batch_size, shuffle=True, num_workers=get_dataloader_workers()))


train_iter, test_iter = load_data_fashion_minist(32, resize=64)
for X, y in train_iter:
    print(X.shape, X.dtype, y.shape, y.dtype)
    break
