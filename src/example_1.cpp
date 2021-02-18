#include "rad.h"
#include <vector>

template <class T> T f(T x)
{
   return 3 * x * x + 2 * x + 4; 
}

// [[Rcpp::export]]
void example_1()
{
    std::vector<CppAD::AD<double>> ax(1);
    ax[0] = 3;
    CppAD::Independent(ax);
    
    std::vector<CppAD::AD<double>> ay(1);
    ay[0] = f(ax[0]);
    CppAD::ADFun<double> f_ad(ax, ay);
    
    std::vector<double> x(1);
    x[0] = 3;
    std::vector<double> res = f_ad.Jacobian(x);

    Rcpp::Rcout << "Gradient of f(x) at x=3: f'(3) = " << res[0] << "\n";
}
