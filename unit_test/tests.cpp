#include "gtest/gtest.h"
#include "../src/ppgnuplot.h"
#include <vector>

TEST(PPGnuPlot, plot){
 PPGnuPlot plot;
 plot.Equation("sin(x)","Sin(x)");
 plot.Wait();
} 

int main(int argc, char *argv[])
{  
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
