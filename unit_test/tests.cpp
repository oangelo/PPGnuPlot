#include "gtest/gtest.h"
#include "../src/ppgnuplot.h"
#include <vector>

TEST(PPGnuPlot, Equation){
 PPGnuPlot plot;
 plot.Equation("sin(x)", "Sin(x)");
// plot.Wait();
} 

TEST(PPGnuPlot, Vector){
 PPGnuPlot plot;
 std::vector<double> vec({6, 5, 4, 3, 2, 1});
 plot.SetXLabel("x label test");
 plot.SetYLabel("y label test");
 plot.Single(vec, "Vector");
 plot.Wait();
}

TEST(PPGnuPlot, matrix){
 PPGnuPlot plot;
 std::vector<std::vector<double>> vec;
 vec.push_back({1, 1});
 vec.push_back({2, 2});
 vec.push_back({3, 4});
 vec.push_back({4, 16});
 vec.push_back({5, 256});
 plot.SetStyle("boxes");
 plot.Pairs(vec, "Vector");
 plot.Wait();
}

int main(int argc, char *argv[])
{  
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
