#include "rad.h"
#include <vector>

template <class T> T f(const T &x_1, const T &x_2)
{
    // exp(3x_1 + 4 + x_2^2)
    return CppAD::exp(3.0 * x_1 + CppAD::pow(x_2, 2));
}

// [[Rcpp::export]]
void example_2()
{
    std::vector<CppAD::AD<double>> ax(2);
    ax[0] = 3;
    ax[1] = -1;
    CppAD::Independent(ax);
    
    std::vector<CppAD::AD<double>> ay(1);
    ay[0] = f(ax[0], ax[1]);
    CppAD::ADFun<double> f_ad(ax, ay);
    
    std::vector<double> x(2);
    x[0] = .2; x[1] = -0.5;
    std::vector<double> res = f_ad.Jacobian(x);

    Rcpp::Rcout << "f/dx_1(0.2) = " << res[0] << "\n";
    Rcpp::Rcout << "f/dx_2(-0.5) = " << res[1] << "\n";
}
