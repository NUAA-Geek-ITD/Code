def test_correct_rate(test_epoch=1000, ifprint=False):
    correct = 0
    for i in range(test_epoch):
        test = torch.randint(0, array_upper_limit, (1, array_length))
        output = net(test)
        if ifprint:
            print(f'''Test:   {','.join([f'{i:6d}' for i in list(test.numpy())[0]])}
    Output: {','.join([f'{i:6.2f}' for i in list(output.detach().numpy())[0]])}
    Max:     {test.max()} (Index = {test.argmax()}, Output = {output[0][test.argmax()]:6.2f})
    Predict: {test[0][output.argmax()]} (Index = {output.argmax()}, Output = {output.max():6.2f})
    ''')
        if test.max() == test[0][output.argmax()]: correct += 1
    print(f"Correct rate: {correct / test_epoch:.2%} ({correct} / {test_epoch})")

test_correct_rate()
