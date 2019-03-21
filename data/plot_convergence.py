import numpy as np
import matplotlib.pyplot as plt
import scipy.interpolate as si
import scipy.linalg as sl
import scipy.sparse as sp
import scipy.sparse.linalg as spla
import scipy.integrate as sig
from mpltools import annotation  # plot triangles indicating convergence order
from mpl_toolkits.mplot3d import Axes3D  # 3D plotting
from matplotlib import cm  # colormaps
from pprint import pprint

plt.rcParams['font.size'] = 16
plt.rcParams['font.family'] = 'sans-serif'
plt.rcParams['font.sans-serif'] = ['Arial', 'Dejavu Sans']


def remove_empty_lines(filename):
    """Overwrite the file, removing empty lines and lines that contain only whitespace."""
    with open(filename) as in_file, open(filename, 'r+') as out_file:
        out_file.writelines(line for line in in_file if line.strip())
        out_file.truncate()
    return None

def read():
    parameters = np.zeros((7, 10, 10))
    performances = np.zeros((7, 10, 10))
    steps = np.zeros((7, 10, 10))

    avg_performances = np.zeros((7, 10))
    avg_steps = np.zeros((7, 10))

    default_steps = np.zeros((10))
    default_performance = np.zeros((10))

    avg_default_steps = 0
    avg_default_performance = 0

    filenames = ['original_convergence.txt', 'num_parents_convergence.txt', 'gene_change_rate_convergence.txt',
                 'crossover_rate_convergence.txt', 'cost_convergence.txt', 'profit_convergence.txt', 'conc_convergence', 'waste_convergence']
    
    for no, filename in enumerate(filenames):
        remove_empty_lines(filename)
        f = open(filename, 'r')
        if no == 0:
            for i in range(5):
                f.readline()
            for run, line in enumerate(f):
                if not line.strip():
                    words = f.readline().split(' ')
                    if len(words) >= 6:
                        step_str = words[3]
                        default_steps[run] = step_str[:-1]
                        perf_str = words[6]
                        default_performance[run] = perf_str
                        for i in range(2):
                            f.readline()
            avg_default_steps = sum(default_steps[:]) / run
            avg_default_performance = sum(default_performance[:]) / run
        else:
            for i in range(6):
                f.readline()
            for var, line in enumerate(f):
                run = 0
                if not line.strip():
                    words = f.readline().split(' ')
                    if len(words) >= 4:
                        parameter_str = words[3]
                        parameters[no, var, run] = parameter_str[:-1]
                        
                    words = f.readline().split(' ')
                    if len(words) >= 7:
                        step_str = words[3]
                        steps[no, var, run] = step_str[:-1]
                        performance_str = words[6]
                        performances[no, var, run] = performance_str[:-1]
                        run += 1
                    f.readline()
                    f.readline()
                    #f.readline()
                    f.readlines()
                    f.readlines()
    
            for i in range(7):
                for var in range(10):
                    avg_performances[i, var] = sum(performances[i, var, :]) / run
                    avg_steps[i, var] = sum(performances[i, var, :]) / run
        f.close()
        
        for i in range(7):
            for var in range(10):
                for run in range(10):
                    print(steps[i,var,run])
        
    return parameters, avg_performances, avg_steps, avg_default_performance, avg_default_steps


def plot():

    parameters, avg_performances, avg_steps, avg_default_performance, avg_default_steps = read()
    default_parameters = [100, 0.01, 0.8, 10, 100, 100.0, 500.0]
    names = ['num_parents', 'gene_change_rate', 'crossover_rate', 'conc', 'waste', 'profit', 'cost']

    #colors = ['b.', 'g.', 'k.', 'r.', 'm.', 'c.', 'y.']
    colors = ['bo', 'go', 'ko', 'ro', 'mo', 'co', 'yo']
    
    ########################################################################
    # plot steps
    ########################################################################

    fig2 = plt.figure(figsize=(8, 6))
    plt.title = 'Influence of different parameters on the number of steps'
    plt.xlabel('parameter [%]', fontsize=16)
    plt.ylabel('avg number of steps [ - ]', fontsize=16)

    plt.loglog(100, avg_default_steps, 'o')#, label='default')

    for i in range(1):
        for j in range(10):
            if j==0:
                plt.loglog(parameters[i,j] / default_parameters[i] * 100, avg_steps[i], colors[i], label = names[i]);
            else:
                plt.loglog(parameters[i,j] / default_parameters[i] * 100, avg_steps[i], colors[i]);
    
    plt.legend(loc='best', fontsize=16)
    fig2.savefig('steps.png')
    
    
    
    #######################################################################
    # plot performances
    ########################################################################

    fig2 = plt.figure(figsize=(8, 6))
    plt.title = 'Influence of different parameters on the performance'
    plt.xlabel('parameter [%]', fontsize=16)
    plt.ylabel('performance [GBP]', fontsize=16)

    
    plt.loglog(100, avg_default_performance, 'o')#, label='default')

    for i in range(7):
        for j in range(10):
            if j==0:
                plt.loglog(parameters[i,j] / default_parameters[i] * 100, avg_performances[i] / avg_default_performance, colors[i], label = names[i]);
            else:
                plt.loglog(parameters[i,j] / default_parameters[i] * 100, avg_performances[i] / avg_default_performance, colors[i]);
    
    plt.legend(loc='best', fontsize=16)
    fig2.savefig('performance.png')

    plt.show()
    return None


def main():
    read()
    plot()
    return None


main()
