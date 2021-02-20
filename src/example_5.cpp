#include <vector>
#include "rad.h"


typedef CppAD::AD<double> a_double;
typedef CPPAD_TESTVECTOR(CppAD::AD<double>) av_double;
typedef CppAD::ADFun<double> a_fun;

a_double fn(const av_double x)
{
    return exp(x[0] * x[1]);
}

a_fun gr;

// [[Rcpp::export]]
Rcpp::List example_5(std::vector<double> x)
{
    av_double ax;
    ax.push_vector(x);
    CppAD::Independent(ax);

    av_double ay(1);
    ay[0] = fn(ax);
    gr.Dependent(ax, ay);
  
    std::vector<double> jacobian = gr.Jacobian(x);
    std::vector<double> hessian = gr.Hessian(x, x);

    return Rcpp::List::create(
	Rcpp::_["J"] = jacobian,
	Rcpp::_["H"] = hessian
    );
}


