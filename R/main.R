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
example_3(xp=5, mu=5, sigma=1)
