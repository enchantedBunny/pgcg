#include "Variable.h"
#include <Python.h>
#include "variable.h"
using namespace calc;

//sets ID
void Variable::setID(int bId){
	id = bId;
}
//init for constants
Variable::Variable(const Eigen::MatrixXd &mat){
	type = constant;
	deps_count = 0;
	val = mat.transpose();
	rows = val.rows();
	columns = val.cols();
}
void Variable::show(){
	cPrint("My ID is " + std::to_string(id) + " and myoutput will be " + std::to_string(rows) + " x " + std::to_string(columns));
}
//init for independent
Variable::Variable(int &r, int &c){
	type = independent;
	deps_count = 1;
	rows = r;
	columns = c;
	cPrint("rows " + std::to_string(rows) + " colss " + std::to_string(columns));

}
// init for fns with 2 inputs
Variable::Variable(Variable *a, Variable *b, std::string *o, bool getDerivs){
	type = function;
	left = a;
	right = b;
	rows = left->rows;
	columns = right->columns;
	op = *o;
	if (a->type == function or b->type==function){
		if (a->type == constant){
			deps_count = b->deps_count;
			deps_list = b->deps_list;
			l_l = 0;
			r_l = deps_count;
			r_ordered =b->deps_list;
		}
		else if (b->type == constant){
			deps_count = a->deps_count;
			deps_list = a->deps_list;
			r_l = 0;
			l_l = deps_count;
			l_ordered = deps_list;
		}
		else{	
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
					else{
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
			if (a->type == independent){
				l_l = 1;
				r_l = 1;
				l_ordered = new int[1];
				l_ordered[0] = {b->id};
				r_ordered = new int[1];
				r_ordered[0] = {b->id};
			}
			else{
				l_l = 0;
				r_l = 0;
			}

		}
		else if ( a->type == independent and b->type == independent){
			deps_count = 2;
			deps_list = new int[2];
			deps_list[0] = std::min(a->id, b->id);
			deps_list[1] = std::max(a->id, b->id);
			l_l = 1;
			r_l = 1;
			l_ordered = new int[1];
			l_ordered[0] = {a->id};
			r_ordered = new int[1];
			r_ordered[0] = {b->id};
		}
		else if (a->type == independent and b->type == constant){
			deps_count = 1;
			deps_list = new int[1];
			deps_list[0] = a->id;
			l_l = 1;
			r_l = 0;
			l_ordered = new int[1];
			l_ordered[0] = {a->id};
		}
		else if (a->type == constant and b->type == independent){
			deps_count = 1;
			deps_list = new int[1];
			deps_list[0] = b->id;
			l_l = 0;
			r_l = 1;
			r_ordered = new int[1];
			r_ordered[0] = {b->id};
		}
		
	}
	
	if (op == "*" && left->columns == right->rows){
		cPrint("valid");
	}
	else if (op == "+" && left->columns == right->columns && left->rows == right->rows){
		cPrint("valid");
		rows = left->rows;
		columns = left->columns;
	}
	else if (op == "-" && left->columns == right->columns && left->rows == right->rows){
		cPrint("valid");
		rows = left->rows;
		columns = left->columns;
	}
	else{
		err = 2;
		// error value 2  => operation invalid
		cError("invalid operation");
		cError("left is a " + std::to_string(left->rows) +"x" + std::to_string(left->columns) + " matrix");
		cError("but right is a " + std::to_string(right->columns) +"x" + std::to_string(right->columns) + " matrix");

	}
	

	cPrint("output will be " + std::to_string(rows) + " x " + std::to_string(columns));
}
//init for fns with a single input
Variable::Variable(Variable *a, std::string *o, bool getDerivs){
	type = function;
	ftype = one;
	left = a;
	op = *o;
	if (op == "transpose") {
		rows = left->columns;
		columns = left->rows;
		cPrint("trans");
	}
	else{
		rows = left->rows;
	columns = left->columns;
	}
	deps_count = a->deps_count;
	deps_list = a->deps_list;
	l_l = deps_count;
	l_ordered = deps_list;
	r_l = 0;

	cPrint("output will be " + std::to_string(rows) + " x " + std::to_string(columns));
}
void Variable::setOpperand(int p){
	opperand = p;
};
long double sigmoid_r(long double x) // the functor we want to apply
{
    return (x* (1- x));
}
long double sigmoid(long double x) // the functor we want to apply
{
    return (1/(1+std::exp(-x)));
}
float square(float x) // the functor we want to apply
{
    return x*x;
}
//setter function fills the tree, used before getValue()
void Variable::setValue(int targetID, const Eigen::MatrixXd &in){
	if (id == targetID){
		if (rows == in.rows() && columns == in.cols())
			val = in;
		else if(columns == in.rows() && rows == in.cols())
			val = in.transpose();
		else {
			val = in;
			err = 1;
			// error value 1  => value doesn't fit
			cError("this value doesn't fit");
			cError("expected a " + std::to_string(rows) +"x" + std::to_string(columns) + " matrix");
			cError("but received a " + std::to_string(in.rows()) +"x" + std::to_string(in.cols()) + " matrix");
		}

	}
	else if(type==function){
		if (left->id == targetID){
			left->setValue(targetID, in);}
		else if (l_l == deps_count){
			left->setValue(targetID, in);
		}
		else if (right->id == targetID){
			right->setValue(targetID, in);}
		else{
			for (int i = 0; i<l_l; i++){
				if (l_ordered[i] == targetID){
					left->setValue(targetID, in);
				}
			}
			for (int i = 0; i<r_l; i++){
				if (r_ordered[i] == targetID){
					right->setValue(targetID, in);
				}
			}
		}
	}
	
}
// getter function which finds and spits out value- will merge this with the setter so that value is always calculated as the numbers are filled in
Eigen::MatrixXd Variable::getValue(){
	
	
	if (type == independent) return val;
	if (type == constant) return val;

	
	Eigen::MatrixXd l = left->getValue();
	if (ftype == one){
		if (op == "sigmoid_r") lastVal = l.unaryExpr(&sigmoid_r);
		if (op == "sigmoid") lastVal =  l.unaryExpr(&sigmoid);
		if (op == "unary_square") lastVal =  l.unaryExpr(&square);
		if (op == "unary_pow") lastVal =  l.array().pow(opperand);

		if (op == "rowsum") lastVal =  l.rowwise().sum();
		if (op == "colsum") lastVal =  l.colwise().sum();

		if (op == "transpose") lastVal =  l.transpose();
		if (op== "stochastic") 
		{
		lastVal = l;
		}
		return lastVal;
	}
	
	// cPrint("l");
	// cPrintM(l);

	if (right == nullptr)return l;
	 //getvalue has no inputs -> 'update' variables before using
	Eigen::MatrixXd r = right->getValue();

	
	// cPrint("r");
	// cPrintM(r);
	
	// if (op == "rowwise*") {
	// 	if (r.cols()==1){
	// 		r = r.transpose();
	// 	}
	// 	Eigen::VectorXd v(r.cols());
	// 	for (int i = 0; i< r.cols(); i++)
	// 		v(i)=r(0,i);
	// 	l.array().rowwise() *= v.array().transpose();
	// 	return l;}
	if (ftype != one){
		if (op == "rowwise*") {
			if (r.cols()==1){
				r = r.transpose();
			}
			cPrintM(r);
			int hh = r.rows();
			bool isr = true;
			if (hh ==1){
				isr = false;
				hh =l.rows();
			}
			for (int g = 0; g<hh; g++)
				for (int i = 0; i< r.cols(); i++)
					l(g,i) *= r(isr ? g : 0,i);
				
			lastVal = l;
		}
		if (op == "rowwise-") {
			if (r.cols()==1){
				r = r.transpose();
			}
			Eigen::VectorXd v(r.cols());
			for (int i = 0; i< r.cols(); i++)
				v(i)=r(0,i);
			l.array().rowwise() -= v.array().transpose();
			lastVal = l;}


		if (op == "+")lastVal = (l + r);
		if (op == "-")lastVal = (l - r);
		if (op == "*")lastVal = (l * r); //figure out which kind of multiplication (matrix vs scalar)
		if (op == "/")lastVal = (l / r(0,0));

		return lastVal;
	}
}
Eigen::MatrixXd Variable::getLastValue(){
	return lastVal;
}
int Variable::getErr(){
	return err;
};
void Variable::resetErr(){
	err = -199;
};