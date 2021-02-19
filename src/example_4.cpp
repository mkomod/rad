#include "rad.h"
#include "adept_source.h"
#include "adept.h"

adept::adouble f(const adept::adouble x, const double a[2]) {
    return (1.0/sqrt(2*PI*a[1]*a[1]))*exp(-1/(2*a[1]*a[1])*(x-a[0])*(x-a[0]));
}

// [[Rcpp::export]]
double
example_4(double x, double mu, double sigma)
{
    adept::Stack stack;
    double a[2] = {mu, sigma};
    adept::adouble x_ = x;
    
    stack.new_recording();
    adept::adouble y = f(x_, a);

    y.set_gradient(1.0);
    stack.compute_adjoint();
    double x_grad = x_.get_gradient();

    return x_grad;
}
