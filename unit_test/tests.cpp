#include "gtest/gtest.h"
#include "../src/ppgnuplot.h"
#include <vector>

TEST(PPGnuPlot, Equation){
 PPGnuPlot plot;
 plot.Equation("sin(x)", "Sin(x)");
 //plot.Wait();
} 

TEST(PPGnuPlot, Vector){
 PPGnuPlot plot;
 std::vector<double> vec({6, 5, 4, 3, 2, 1});
 plot(vec, "Sin(x)");
 plot.Wait();
}

int main(int argc, char *argv[])
{  
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
