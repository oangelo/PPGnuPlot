++GnuPlot
=========

Another C++ API for GnuPlot.

Installing
----------

'''
$ make
$ sudo make install
'''

Usage example:
-------------

'''
//file: plot.cpp
#include <ppgnuplot.h>
#include <vector>

int main(){
  PPGnuPlot plot;
  std::vector<double> vec({6, 5, 4, 3, 2, 1});
  plot(vec);
  plot.Wait();
}
'''
Compile with:
'''
g++ plot.cpp  -lppgnuplot --std=c++11
'''
