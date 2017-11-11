#include "Variable.h"
#include <Python.h>
#include "variable.h"
using namespace calc;


void Variable::setID(int bId){
	id = bId;
}

Variable::Variable(const Eigen::MatrixXd &mat){
	type = constant;
	deps_count = 0;
	val = mat.replicate(1,1);
	rows = val.rows();
	columns = val.cols();
	show();
}
void Variable::show(){
}

Variable::Variable(int &r, int &c){
	type = independent;
	deps_count = 1;
	rows = r;
	columns = c;
}

Variable::Variable(Variable *a, Variable *b, std::string *o, bool getDerivs){
	type = function;
	left = a;
	right = b;
	op = *o;
	if (a->type == function or b->type==function){
		if (a->type == constant){
			deps_count = b->deps_count;
			deps_list = b->deps_list;
		}
		else if (b->type == constant){
			deps_count = a->deps_count;
			deps_list = a->deps_list;
		}
			else{			
			int *l_ordered;
			int l_l ;	
			int *r_ordered;
			int r_l ;	
			if (a->type == function){
				l_ordered = a->deps_list;
				l_l = a->deps_count;
			}
			else if (a->type == independent){
				l_ordered = new int[1];
				l_ordered[0] = {a->id};
				l_l = 1;
			}
			if (b->type == function){
				r_ordered = b->deps_list;
				r_l = b->deps_count;
			}
			else if (b->type == independent){
				r_ordered = new int[1];
				r_ordered[0] = {b->id};
				r_l = 1;
			}
			int uniques = r_l + l_l;
			for (int i = 0; i< r_l; i++){
				for (int ii = 0; ii< l_l; ii++){
					if (r_ordered[i] == l_ordered[ii])
						uniques--;
				}
			}
			deps_count = uniques;
			deps_list = new int[uniques];
			int ri = 0;
			int li = 0;
			for (int i = 0; i<uniques; i++){
				if (ri < r_l and li < l_l){
					if (r_ordered[ri]<l_ordered[li]){
						deps_list[i] = r_ordered[ri];
						ri++;
					}
					else if (r_ordered[ri]>l_ordered[li]){
						deps_list[i] = l_ordered[li];
						li++;
					}
					else if (r_ordered[ri]<l_ordered[li]){
						deps_list[i] = r_ordered[ri];
						ri++;
						li++;
					}
				}
				else if (ri < r_l and li >= l_l){
					deps_list[i] = r_ordered[ri];
					ri++;
				}
				else if (ri >= r_l and li < l_l){
					deps_list[i] = l_ordered[li];
					li++;
				}
			}
		}
	}
	else{
		if ( a->id == b->id){
			deps_count = 1;
			deps_list = new int[1];
			deps_list[0] = a->id;
		}
		else if ( a->type == independent and b->type == independent){
			deps_count = 2;
			deps_list = new int[2];
			deps_list[0] = std::min(a->id, b->id);
			deps_list[1] = std::max(a->id, b->id);
		}
		else if (a->type == independent and b->type == constant){
			deps_count = 1;
			deps_list = new int[1];
			deps_list[0] = a->id;
		}
		else if (a->type == constant and b->type == independent){
			deps_count = 1;
			deps_list = new int[1];
			deps_list[0] = b->id;
		}
		
	}
}

Variable::Variable(Variable *a, std::string *o, bool getDerivs){
	type = function;
	left = a;
	op = *o;
	deps_count = a->deps_count;
	deps_list = a->deps_list;
}
Variable::Variable(Variable *a, std::string *op, int p, bool getDerivs){
	type = function;
}
float sigmoid_r(float x) // the functor we want to apply
{
    return (1/(1+std::exp(-x))) * (1- (1/(1+std::exp(-x))));
}
float sigmoid(float x) // the functor we want to apply
{
    return (1/(1+std::exp(-x)));
}
float square(float x) // the functor we want to apply
{
    return x*x;
}
Eigen::MatrixXd Variable::getValue(const Eigen::MatrixXd &in){
	if (type == independent) return in;
	if (type == constant) return val;
	
	Eigen::MatrixXd l = left->getValue(in);
	if (op == "sigmoid_r") return l.unaryExpr(&sigmoid_r);
	if (op == "sigmoid") return l.unaryExpr(&sigmoid);
	if (op == "square") return l.unaryExpr(&square);
	if (op == "rowsum") return l.rowwise().sum();

	 //only one input -> 'update' constants
	Eigen::MatrixXd r = right->getValue(in);

	Eigen::MatrixXd m(1,deps_count);
	for (int i = 0; i<deps_count; i++)
		m(0,i) = deps_list[i];
	
	
	if (op == "colwise"){
		for (int i = 0; i<l.cols(); i++){
			for (int ii = 0; ii<l.rows(); ii++){
				l(ii,i) *= r(ii,0);

			}
		}
		return l;
	}
	if (op == "+")return (l + r).transpose();
	if (op == "-")return (l - r).transpose();
	if (op == "*")return (l.transpose() * r).transpose(); //figure out which kind of multiplication (matrix vs scalar)
	if (op == "/")return (l / r(0,0)).transpose();
}
