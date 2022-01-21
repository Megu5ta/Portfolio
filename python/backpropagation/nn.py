import numpy as np


np.random.seed(42)


class Layer:
    """
    This is just a dummy class that is supposed to represent the general
    functionality of a neural network layer. Each layer can do two things:
     - forward pass - prediction
     - backward pass - training
    """

    def __init__(self):
        pass

    def forward(self, inp):
        # a dummy layer returns the input
        return inp

    def backward(self, inp, grad_outp):
        pass


class Sigmoid(Layer):
    def __init__(self):
        pass

    def forward(self, inp):
        return 1 / (1 + np.exp(-inp))

    def backward(self, inp, grad_outp):
        outp = inp[0] * (1 - inp[0])
        return grad_outp * outp, None


class Dense(Layer):
    def __init__(self, inp_units, outp_units, learning_rate=0.001):
        self.learning_rate = learning_rate
        self.weights = np.random.normal(
            loc=0.0, scale=np.sqrt(2 / (inp_units + outp_units)),
            size=(inp_units, outp_units)
        )
        # self.biases = np.zeros(outp_units) + np.random.normal(loc=0.0, scale=0.5)
        self.biases = np.random.normal(loc=0.0, scale=0.5, size=(1, outp_units))[0]

        self.neuron_count = outp_units

    def forward(self, inp):
        return np.dot(inp, self.weights) + self.biases

    def backward(self, inp, grad_outp):
        outp = grad_outp * self.weights
        return sum(outp.T), grad_outp * np.array([inp[1]]).T

    def update_weights(self, delta):
        self.weights -= self.learning_rate * delta


class MLP():
    def __init__(self):
        self.layers = []

    def add_layer(self, neuron_count, inp_shape=None, activation='sigmoid', learning_rate=0.001):
        if len(self.layers) == 0 and inp_shape is None:
            raise ValueError("Must define number of inputs for first layer")

        if inp_shape is None:
            inp_shape = self.layers[-2].neuron_count

        self.layers.append(Dense(inp_shape, neuron_count, learning_rate=learning_rate))
        if activation == 'sigmoid':
            self.layers.append(Sigmoid())
        else:
            raise ValueError("Unknown activation {}".format(activation))

    def forward(self, X):
        activations = []

        layer_input = X
        for layer in self.layers:
            layer_output = layer.forward(layer_input)
            activations.append(layer_output)
            layer_input = layer_output

        return activations

    def predict(self, X):
        activations = self.forward(X)
        output = activations[-1]
        return output

    def fit(self, X, y):
        deltas = []
        activations = self.forward(X)
        activations.insert(0, X)
        grad_outp = activations[-1] - y
        for index, layer in enumerate(reversed(self.layers)):
            grad_outp, delta = layer.backward([activations[-1 - index], activations[-2 - index]], grad_outp)
            if delta is not None:
                deltas.insert(0, delta)

        # print('-'*20)
        for i in range(0, len(deltas)):
            self.layers[i*2].update_weights(deltas[i])

    def train(self, X, y, epochs=10):
        for i in range(0, epochs):
            for j in range(0, len(y)):
                self.fit(X[j], y[j])


if __name__ == '__main__':
    test = [[300, 400, 500], [2, 0, 1]]
    test = np.array(test)

    network = MLP()

    X = [0.05, 0.1]
    y = [0.01, 0.99]


    # TODO: add layers to the network
    network.add_layer(2, 2, activation='sigmoid')
    # network.layers[0].weights = np.array([[0.15, 0.25], [0.2, 0.3]])
    # network.layers[0].biases = np.array([0.35, 0.35])
    network.add_layer(2, activation='sigmoid')
    network.add_layer(2, activation='sigmoid')
    # network.layers[2].weights = np.array([[0.4, 0.5], [0.45, 0.55]])
    # network.layers[2].biases = np.array([0.6, 0.6])

    print(network.predict(X))
    for _ in range(0, 10002):
        network.fit(X, y)
    # print(network.layers[0].weights)
    # print()
    # print(network.layers[2].weights)
    print(network.predict(X))
