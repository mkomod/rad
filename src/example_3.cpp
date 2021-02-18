#include "rad.h"
#include <vector>

template <class T> T f(const T &x, const CPPAD_TESTVECTOR(double) a)
{
    // exp(3x_1 + 4 + x_2^2)
    return (1.0 / CppAD::sqrt(2 * PI * a[1])) *
	CppAD::exp(- 1/(2*a[1]*a[1]) * (x-a[0]) * (x-a[0]));
}

// [[Rcpp::export]]
void example_3(double xp, double mu=0, double sigma=1)
{
    CPPAD_TESTVECTOR(double) a(2);
    a[0] = mu; a[1] = sigma;

    std::vector<CppAD::AD<double>> ax(1);
    ax[0] = xp;
    CppAD::Independent(ax);
    
    std::vector<CppAD::AD<double>> ay(1);
    ay[0] = f(ax[0], a);
    CppAD::ADFun<double> f_ad(ax, ay);
    
    std::vector<double> x(1);
    x[0] = xp;
    std::vector<double> res = f_ad.Jacobian(x);

    Rcpp::Rcout << "f/dx(" << xp << ") = " << res[0] << "\n";
}
