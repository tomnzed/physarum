from numpy import genfromtxt
import matplotlib
import matplotlib.pyplot as plt

if __name__ == "__main__":

    filename = '../bin/data/agents.csv'

    my_data = genfromtxt(filename, delimiter=',')

    n, bins, patches = plt.hist(my_data[:, 2], bins=160)
    plt.show()