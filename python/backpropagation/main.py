from sklearn import datasets
from sklearn.utils import shuffle
import nn
import data_processor as dp


def print_table(tp, fp, fn, tn, accuracy, precision, recall):
    table_template = "|{:<14}|{:>9}|{:>9}|"
    vertical_line = "+{}+{line}+{line}+".format('-'*14, line=('-'*9))
    print(vertical_line)
    print(table_template.format("Predict\\Actual", "Positive", "Negative"))
    print(vertical_line)
    print(table_template.format("Positive", tp, fp))
    print(vertical_line)
    print(table_template.format("Negative", fn, tn))
    print(vertical_line)
    table_template = "|{:<9}|{:>20}|"
    vertical_line = "+{}+{}+".format('-'*9, '-'*20)
    print(vertical_line)
    print(table_template.format("Accuracy", accuracy))
    print(vertical_line)
    if precision is None:
        print(table_template.format("Precision", "None"))
    else:
        print(table_template.format("Precision", precision))
    print(vertical_line)
    if recall is None:
        print(table_template.format("Recall", "None"))
    else:
        print(table_template.format("Recall", recall))
    print(vertical_line)


def check(X, y):
    tp = 0
    fp = 0
    fn = 0
    tn = 0
    average_error = 0
    for i in range(0, len(X)):
        got = network.predict(X[i])
        out = nn.np.array(transform_output(y[i]))
        average_error += 0.5 * ((out - got) ** 2)
        got = nn.np.where(got == max(got))[0][0]
        if got == 1 and y[i] == 1:
            tp += 1
        elif got == 1 and y[i] == 0:
            fp += 1
        elif got == 0 and y[i] == 0:
            tn += 1
        elif got == 0 and y[i] == 1:
            fn += 1

    average_error /= len(X)
    average_error = sum(average_error)
    print("-="*2 + "AVERAGE ERROR" + "=-"*2)
    print("", average_error)
    print("-=-"*7)

    accuracy = (tp + tn) / (tp + tn + fp + fn)
    try:
        precision = tp / (tp + fp)
        recall = tp / (tp + fn)
    except ZeroDivisionError:
        precision = None
        recall = None

    print_table(tp, fp, fn, tn, accuracy, precision, recall)
    return None


def check_train_process(iterations, epochs_per_iteration):
    if iterations < 1 or epochs_per_iteration < 1:
        raise ValueError("Input parameters must be greater than 0")
    for iteration in range(0, iterations):
        network.train(train_X, train_y, epochs=epochs_per_iteration)
        print()
        print("Epochs {}".format((iteration+1) * epochs_per_iteration))
        check(test_X, test_y)


if __name__ == '__main__':
    # Create network--------------------------------------
    network = nn.MLP()
    network.add_layer(15, 13, activation='sigmoid', learning_rate=0.001)
    network.add_layer(8, activation='sigmoid', learning_rate=0.001)
    network.add_layer(2, activation='sigmoid', learning_rate=0.001)
    # ----------------------------------------------------

    # Load data and prepare data--------------------------
    proc = dp.DataProcessor()
    proc.load_csv_dataset("archive/heart.csv", skip_first=True)

    train, test = proc.get_dataset(shuffle_dataset=True, percentage=0.8, convert_to_float=True)

    train_X, train_y = proc.split_dataset_to_data_target(train)
    test_X, test_y = proc.split_dataset_to_data_target(test)

    transform_output = lambda x: [1, 0] if x == 0 else [0, 1] if x == 1 else None
    train_y = [transform_output(elem) for elem in train_y]
    # ----------------------------------------------------

    # train network and view the learning process---------
    check_train_process(20, 100)    # average precision is around 0.7; accuracy - around 0.65 (But sometimes they were both over 85. Perhaps it was necessary to save the weights to a file so that later you could return to them).   Pretty bad :( It might be worth trying normalization, but...                                                                                                                                                                       to be continued...
    # ----------------------------------------------------
