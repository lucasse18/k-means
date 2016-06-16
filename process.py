#!/usr/bin/env python
import subprocess as sp
import argparse
import random
import errno
import os

from sklearn.metrics.cluster import adjusted_rand_score

def remove_file(filename):
    try:
        os.remove(filename)
    except OSError as e:
        # errno.ENOENT = no such file or directory
        if e.errno != errno.ENOENT:
            raise  # re-raise exception if a different error occured


def float_range(start, stop=None, step=1.0, error=0.00000001):
    if stop is None:
        stop = start
        start = 0.0
    if step <= 0:
        while start < (stop + error):
            yield start
            start += step
    else:
        while start < (stop - error):
            yield start
            start += step

parser = argparse.ArgumentParser()
parser.add_argument('-d', action='store', dest='data',
                    required=True, help='dataset name')
parser.add_argument('-k', action='store', dest='clusters',
                    required=True, help='number of clusters')
parser.add_argument('-m', action='store', dest='mult',
                    required=True, help='sample size')
parser.add_argument('-n', action='store', dest='iterations',
                    default='10',  help='sample size')
parser.add_argument('-o', action='store', dest='outfile',
                    default='processed',  help='outfile name')
args = parser.parse_args()

random.seed()

data_folder = "datasets/" + args.data + "/"
data = data_folder + args.data + ".dat"
cmd = "sudo nice -n -3 ./kmeans " + data + " -q -a yy -k " + args.clusters + " -s "

outfile = data_folder + args.outfile + ".dat"
remove_file(outfile)

# for mult in [m for m in float_range(1.1, 2.1, 0.1)]:
for i in range(int(args.iterations)):
    res2 = ([], [], [])
    res = ([], [], [])
    seed = random.getrandbits(64)

    yy = sp.getoutput("taskset 4 " + cmd + str(seed))
    yy_m = sp.getoutput("taskset 8 " + cmd + str(seed) + " -m " + str(args.mult))

    yy = str.splitlines(yy)
    yy_m = str.splitlines(yy_m)

    # computa a diferenca no tempo de execucao
    res[0].append(float(yy[0]) - float(yy_m[0]))
    # compara os agrupamentos
    res[1].append(adjusted_rand_score(list(yy[1]),list(yy_m[1])))
    # multiplicador utilizado
    res[2].append(args.mult)

    res = zip(res[0], res[1], res[2])
    with open(outfile, 'a') as f:
        for col in res:
            f.write("{0},{1},{2}\n".format(*col))
