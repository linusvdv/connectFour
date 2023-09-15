import torch
import json
from torch import nn
from torch.utils.data import DataLoader
from torch.utils.data import Dataset
from collections import OrderedDict


learning_rate = 1e-3
batch_size = 64
epochs = 100


# get device
device = (
    "cuda"
    if torch.cuda.is_available()
    else "mps"
    if torch.backends.mps.is_available()
    else "cpu"
)
print(f"Using {device} device")


class CustomDataset(Dataset):
    def __init__(self):
        with open("training_data.json", "r", encoding="utf-8") as file:
            self.data = json.load(file)

    def __len__(self):
        return len(self.data)

    def __getitem__(self, idx):
        position_args = torch.Tensor(self.data[idx][0]).to(device)
        game_res = torch.Tensor(self.data[idx][1]).to(device)
        return (position_args, game_res)


class NeuralNetwork(nn.Module):
    def __init__(self):
        super().__init__()
        self.linear_relu_stack = nn.Sequential(
            nn.Linear(552, 64),
            nn.ReLU(),
            nn.Linear(64, 16),
            nn.ReLU(),
            nn.Linear(16, 16),
            nn.ReLU(),
            nn.Linear(16, 3),
            nn.Softmax(dim=1),
        )

    def forward(self, x):
        logits = self.linear_relu_stack(x)
        return logits


def train_loop(dataloader, model, loss_fn, optimizer):
    size = len(dataloader.dataset)
    # Set the model to training mode - important for batch normalization and dropout layers
    # Unnecessary in this situation but added for best practices
    model.train()
    for batch, (X, y) in enumerate(dataloader):
        # Compute prediction and loss
        pred = model(X)
        loss = loss_fn(pred, y)

        # Backpropagation
        loss.backward()
        optimizer.step()
        optimizer.zero_grad()

        if batch % 10 == 0:
            loss, current = loss.item(), (batch + 1) * len(X)
            print(f"loss: {loss:>7f}  [{current:>5d}/{size:>5d}]")


if __name__ == "__main__":
    trainset = CustomDataset()
    train_dataloader = DataLoader(trainset, batch_size=batch_size, shuffle=True)

    model = NeuralNetwork().to(device)
    print(model)

    # train
    loss_fn = nn.CrossEntropyLoss()
    optimizer = torch.optim.SGD(model.parameters(), lr=learning_rate)

    for i in range(epochs):
        print(f"Epoch {i+1}\n-------------------------------")
        for g in optimizer.param_groups:
            g['lr'] = learning_rate / ((i+10) / 10)
        train_loop(train_dataloader, model, loss_fn, optimizer)
    print("Done!")

    with open("NeuralNetwork.json", "w", encoding="utf-8") as file:
        od1 = model.state_dict()
        od1 = OrderedDict({k: od1[k].detach().cpu().tolist() for k in od1})
        json.dump(od1, file, ensure_ascii=False, indent=4)
    print("Saved PyTorch Model State to NeuralNetwork.json")
