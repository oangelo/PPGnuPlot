#include "gtest/gtest.h"
#include "../src/ppgnuplot.h"
#include <vector>

TEST(PPGnuPlot, plot){
 PPGnuPlot plot;
 plot.gnuplot_plot_equation("x","my graph");
} 

int main(int argc, char *argv[])
{  
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
