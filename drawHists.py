#!/usr/bin/env python
import sys
from pprint import pprint
import pandas as pd
import numpy as np
import matplotlib.mlab as mlab
import matplotlib.pyplot as plt

def showHist(filename, xlabel = "values", title = "Histogram", num_bins_ = 100, min_ = 30, max_ = None):
    with open(filename, 'r') as f:
        x = [int(line.rstrip('\n')) for line in f]
    num_bins = num_bins_
    # the histogram of the data
    if max_ == None:
        max_ = max(x)
    if max_ < min_:
        min_ = 0
    
    n, bins, patches = plt.hist(x, num_bins, normed=1, facecolor='green', alpha=0.5, range = (min_, max_), histtype="bar")
    plt.xlabel(xlabel)
    plt.ylabel('Probability')
    plt.title(title)
   
    plt.show()


def showDiscrHist(filename, xlabel = "values", title = "Histogram", num_bins_ = 100, min_ = 30, max_ = None):
    with open(filename, 'r') as f:
        data = [int(line.rstrip('\n')) for line in f]
        
    data = pd.Series(data)
    num_bins = num_bins_
    #print data
    
    vc = data.value_counts().sort_index()
    print "pages distances:"
    print vc

    ax = vc.plot(kind='bar')
    fig = ax.get_figure()
    fig.autofmt_xdate()
    fig.savefig('pageDistances.png')
    

def main():
    if len(sys.argv) >= 4:
        showHist(sys.argv[1], "Bytes", "Histogram of pages' sizes")
        showHist(sys.argv[2], "Links", "Histogram of incoming links for pages", 50)
        showHist(sys.argv[3], "Links", "Histogram of outgoing links for pages")
        if len(sys.argv) >= 5:
            showDiscrHist(sys.argv[4], "Distance in clicks", "Histogram of distances from main page", 10)

    else:
        print "Usage: %s  sizes_file  in_links_file  out_links_file" % sys.argv[0]

if __name__ == '__main__':
    main()
