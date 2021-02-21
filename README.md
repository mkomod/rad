# rad

Some C++ autodiff (AD) examples with binding to R through Rcpp. 

Different examples have been put together showcasing AD. 

## Example 1

Computes the derivative of

```
f(x) = 3x^2 + 2x + 2
```

[Source](./src/example_1.cpp)


## Example 2

Computes the derivative of a function with two input variables

```
f(x_1, x_2) = exp(3x_1 + 4 + x_2^2)
```

[Source](./src/example_2.cpp)

## Example 3

Computes the derivative of the normal density, and compares the performance to the analytical solution. AD generated derivatives are about 10x slower

[Source](./src/example_3.cpp)

## Example 4

We introduce the Adept AD library and compare it to CppAD. CppAD is about twice as fast.

[Source](./src/example_4.cpp)


## Example 5 

We compute the Jacobian and Hessian of a function taking a vector as input.

[Source](./src/example_5.cpp)


## Example 6

We provide a general class for defining functions and computing their derivatives. We showcase how the class can be used with R's optimisers to find the minimum of a function.

[Source](./src/example_6.cpp)

