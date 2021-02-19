library(Rcpp)

# Example 1
# f(x) = 3x^2 + 2x + 2
Rcpp::sourceCpp("../src/example_1.cpp", rebuild=T, verbose=T)
example_1()


# Example 2
# f(x_1, x_2) = exp(3x_1 + 4 + x_2^2)
Rcpp::sourceCpp("../src/example_2.cpp", rebuild=T, verbose=T)
example_2()


# Example 3
# f(x) = Norm(x; mu, sigma^2)
Rcpp::sourceCpp("../src/example_3.cpp", rebuild=T, verbose=T)
f.d <- function(x, mu, sigma) {
- x * exp(- x^2 / (2 * sigma^2) ) / (sigma^3 * sqrt(2 * pi))
}
example_3(xp=5, mu=0, sigma=1)
microbenchmark::microbenchmark(example_3(0,0,1), f.d(0, 0, 1), times=1e3)
# About 10 times slower than using analytical sol


# Example 4
# We use adept, a different library to compute derivatives.
Rcpp::sourceCpp("../src/example_4.cpp", rebuild=T, verbose=T)
microbenchmark::microbenchmark(
	example_3(x=5, mu=0.0, sigma=2), 
	example_4(x=5, mu=0.0, sigma=2), times=100)
# CppAD is about 2x faster

