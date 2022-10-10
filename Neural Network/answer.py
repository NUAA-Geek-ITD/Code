import numpy as np
import torch
import torch.nn as nn
import torch.utils.data as Data

# length:5, no bigger than10. Datasets:
array_length = 5
array_upper_limit = 10
num_examples = 1000

features = torch.randint(0, array_upper_limit, (num_examples, array_length))
labels = torch.tensor([[0 if j != torch.max(i) else 1 for j in i] for i in features])

# Read the logs:
batch_size = 1
dataset = Data.TensorDataset(features, labels)
data_iter = Data.DataLoader(dataset, batch_size, shuffle=True)

# DEFINE
class Net(nn.Module):
    def __init__(self):
        super(Net, self).__init__()
        
        self.conv1 = nn.Conv1d(1, 6, 2)
        
        self.conv2 = nn.Conv1d(6, 10, 2)
        
        self.linear2 = nn.Linear(10*(array_length-2), array_length)
    def forward(self, x):
        x = x.float()
        x = self.conv1(x)
        x = nn.functional.relu(x)
        x = self.conv2(x)
        x = nn.functional.relu(x)
        x = self.linear2(x.view(-1,10*(array_length-2)))
        return x

net = Net()

loss = nn.CrossEntropyLoss()

# learn_rate = 0.001
optimizer = torch.optim.SGD(net.parameters(), lr=0.001)

# Train!
num_epochs = 10
for epoch in range(num_epochs):
    train_l_sum, train_acc_sum, n = 0.0, 0.0, 0
    for X, y in data_iter:

        optimizer.zero_grad()

        y_hat = net(X)
        
        l = loss(y_hat, y.float()).sum()
        l.backward()
        optimizer.step()
        
        train_l_sum += l.item()
        train_acc_sum += (y_hat.argmax() == y).sum().item()
        n += y.shape[0]
    print(f"epoch {epoch + 1}, loss {train_l_sum:f}")

# FUNCTION
def torch_maximum(array):
    return array[net(torch.tensor(array).view(-1, 5)).argmax()]

# TEST!
print(torch_maximum([2, 3, 9, 1, 0]))
