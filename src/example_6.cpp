#include <vector>
#include <R/R_ext/Applic.h>
#include "rad.h"


typedef CppAD::AD<double> a_double;
typedef CPPAD_TESTVECTOR(CppAD::AD<double>) av_double;
typedef CppAD::ADFun<double> a_fun;

class Function;
class F;

struct opt_struct
{
    double* ndeps;   /* tolerances for numerical derivatives */
    double  fnscale;  /* scaling for objective */
    double* parscale;/* scaling for parameters */
    int     usebounds;
    double* lower, *upper;
};


class Function {
    private:
	a_fun gr_;
    
    public:
	virtual a_double fn(const av_double &x) {
	    return x[0];
	}
	
	template <class T>
	T gr(const T &x) {
	    return gr_.Jacobian(x);
	}

	void set_grad(const std::vector<double> &x) {
	    av_double ax;
	    ax.push_vector(x);
	    CppAD::Independent(ax);
	    av_double ay(1);
	    ay[0] = fn(ax);
	    gr_.Dependent(ax, ay);
	    gr_.optimize();
	}
	
	opt_struct args;
};


double minfn(int n, double *x, void *ex) 
{
    opt_struct args(static_cast<Function *>(ex)->args);
    std::vector<double> x_(n, 0);

    x_.assign(x, x+n);
    av_double ax_;
    ax_.push_vector(x_);
    
    return CppAD::Value(static_cast<Function *>(ex)->fn(ax_));
}


void mingr(int n, double *x, double *gr, void *ex)
{
    opt_struct args(static_cast<Function *>(ex)->args);
    std::vector<double> x_(n, 0);
    x_.assign(x, x+n);

    std::vector<double> xgr = static_cast<Function *>(ex)->gr(x_);
    for (int i = 0; i < n; ++i) {
	// Rcpp::Rcout << xgr.at(i) << "\n";
	gr[i] = xgr.at(i); // * (args.parscale[i] / args.fnscale);
    }
}


void minimise(F &func, std::vector<double> x, double *x_out) {
    int n = x.size();
    double x_init[n];
    double Fmin;
    int fail;
    int type = 1;
    int trace = 0;
    int fncount = 0;
    int grcount = 0;
    int maxit = 10000;

    for (int i = 0; i < n; ++i) {
	x_init[i] = x.at(i);
	x_out[i] = 0;
    }

    cgmin(n, x_init, x_out, &Fmin, &minfn, &mingr, &fail, R_NegInf, 1e-32, 
	    &func, type, trace, &fncount, &grcount, maxit);
}


class F : public Function {
    a_double fn(const av_double &x) {
	// change what's being returned to test out different functions
	return 100 * pow(x[1] - x[0]*x[0], 2) + pow(1-x[0], 2);
    }
};


//[[Rcpp::export]]
std::vector<double> example_6(std::vector<double> x) 
{
    F func;
    func.set_grad(x);

    double x_out[x.size()];
    minimise(func, x, x_out);

    std::vector<double> min;
    min.assign(x_out, x_out + x.size());
    return min;
}

