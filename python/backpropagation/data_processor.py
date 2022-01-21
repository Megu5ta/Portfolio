from csv import reader
from random import shuffle


class DataProcessor:
    def __init__(self):
        self.dataset = None
        self.train = None
        self.test = None

    def load_csv_dataset(self, file_path, skip_first=False):
        with open(file_path, "r") as f:
            if skip_first:
                self.dataset = list(reader(f))[1:]
            else:
                self.dataset = list(reader(f))
        return self.dataset

    def split_dataset_to_data_target(self, dataset):
        X = []
        y = []
        for line in dataset:
            X.append(line[:-1])
            y.append(line[-1])
        return X, y

    def get_dataset(self, shuffle_dataset=False, percentage=None, convert_to_float=False, dataset=None):
        if self.dataset is None and dataset is None:
            raise ValueError("You need to load dataset first.")

        if dataset is None:
            result = self.dataset
        else:
            result = dataset

        if shuffle_dataset:
            shuffle(result)

        if convert_to_float:
            result = [[float(elem) for elem in line] for line in result]

        if percentage is not None:
            if not isinstance(percentage, float):
                raise TypeError("percentage must be float")
            if 0 < percentage < 1:
                amount = len(self.dataset)
                self.train = result[0: int(amount * percentage)]
                self.test = result[int(amount * percentage):]
            else:
                raise ValueError("percentage must be greater than 0 and lower than 1")
            return self.train, self.test

        return result
