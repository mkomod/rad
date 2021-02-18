library(Rcpp)

# Example 1
# AD of f(x) = 3x^2 + 2x + 2
Rcpp::sourceCpp("../src/example_1.cpp", rebuild=T, verbose=T)
example_1()
