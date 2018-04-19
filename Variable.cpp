#include "Variable.h"
#include <Python.h>
#include "master.h"
#include "Helpers.h"
#include "helpers.h"
using namespace calc;


Eigen::MatrixXd differentiate(Variable *a, Variable *b, const Eigen::Map<Eigen::MatrixXd> &in,int it){
	// h = 0.00000001
    // npistF = np.copy(npist)
    // for (x,y), value in np.ndenumerate(npist):
    //     npist2 = np.copy(npist)
    //     npist2[x][y] += h
    //     npistF[x][y]= (sum(vara.value(dict([(varb,npist)]))) - sum(vara.value(dict([(varb,npist2)]))))/h
    // return npistF

	double h = 0.00000001;
	int bID = b->getID();
	Eigen::MatrixXd inF = in.replicate(1,1);
	a->setValue(bID, in);
	double bf = a->getValue(it).sum();
	for (int x=0; x<in.rows();x++){
		for (int y=0; y<in.cols();y++){
			Eigen::MatrixXd in2 = in.replicate(1,1);
			in2(x,y) += h;
			//float *p = &in2(0,0);  // get the address storing the data for m2
			Eigen::Map<Eigen::MatrixXd> in22(in2.data(),in2.rows(),in2.cols());
			a->setValue(bID, in22);
			inF(x,y) = (bf-a->getValue(it).sum())/h;
			
	}
	}

	return inF;
}

//this shouldn't be necessary, figure it out!!!
//square? - duh -
long double sq(long double x)
{
    return x*x;
}
//ln
long double ln(long double x)
{
    return std::log(x);
}
//1/(1+e^-x)
long double sigmoid(long double x)
{
    return (1/(1+std::exp(-x)));
}
//derivative of the sigmoid in terms of y
long double sigmoid_ry(long double y)
{
    return (y* (1- y));
}
//derivative of the sigmoid in terms of x
long double sigmoid_rx(long double x)
{
    x = sigmoid(x);
    return (x* (1- x));
}


//sets ID
void Variable::setID(int bId){
	id = bId;
}

int Variable::getID(){
	return id;
}
//init for constants
Variable::Variable(const Eigen::Map<Eigen::MatrixXd> &mat){
	type = constant;
	deps_count = 0;
	val = mat.transpose();
	rows = val.rows();
	columns = val.cols();
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
		if (a->type == constant and b->type==function){
			deps_count = b->deps_count;
			deps_list = b->deps_list;
			l_l = 0;
			r_l = deps_count;
			r_ordered =b->deps_list;
		}
		else if (a->type == function and b->type == constant){
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
	else if (op == "rowwise*" && left->columns == right->columns && left->rows == right->rows){
		cPrint("valid");
		rows = left->rows;
		columns = left->columns;
	}
	else{
		err = 2;
		// error value 2  => operation invalid
		cError("<invalid operation: " + op + " >left is a " + std::to_string(left->rows) +"x" + std::to_string(left->columns) + " matrix but right is a " + std::to_string(right->rows) +"x" + std::to_string(right->columns) + " matrix");

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
//debug fn that shows info about variable
void Variable::show(){
	cPrint("My ID is " + std::to_string(id) + " and myoutput will be " + std::to_string(rows) + " x " + std::to_string(columns));
}
//setter for additional opperand some functions use (e.g. pow)
void Variable::setOpperand(int p){
	opperand = p;
};
//setter function fills the tree, used before getValue()
void Variable::setValue(int targetID, const Eigen::Map<Eigen::MatrixXd> &in){
	if (id == targetID){
		if (rows == in.rows() && columns == in.cols()){
			val = in;
		}
		else if(columns == in.rows() && rows == in.cols()){
			val = in.transpose();
		}
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
// setter fn for iteration ID - REDUNDANT -> IT'S DONE BY getValue, adding this was a stupid mistake
void Variable::setItID(int it){
	itid = it;
	if (l_l > 0){
		left->setItID(it);
	}
	if (r_l > 0){
		right->setItID(it);
	} 
}
// getter function which finds and spits out value- will merge this with the setter so that value is always calculated as the numbers are filled in
Eigen::MatrixXd Variable::getValue(int it){

	if (type == independent) return val;
	if (type == constant) return val;
	

	if (itid == it){ //this fn for this var has already been executed, ergo we can use lastValue
		return lastVal;
	}itid = it;
	
	
	Eigen::MatrixXd l = left->getValue(it);
	if (ftype == one){
		if (op == "sigmoid_r") lastVal = l.unaryExpr(&sigmoid_ry);
		if (op == "sigmoid") lastVal =  l.unaryExpr(&sigmoid);
		if (op == "ln") lastVal =  l.unaryExpr(&ln);
		if (op == "softmax") {
			if (l.rows()==1){
				l = l.transpose();
			}
			for (int j =0; j<l.rows(); j++){
				float s = 0;
				for (int i =0; i<l.cols(); i++){
					s += std::exp(l(j,i));
				}
				for (int i =0; i<l.cols(); i++){
					l(j,i) = std::exp(l(j,i))/s;
				}
			}
			lastVal =  l;
		}
		if (op == "softmax_r") {
			if (l.rows()==1){
				l = l.transpose();
			}
			for (int j =0; j<l.rows(); j++){
				float s = 0;
				for (int i =0; i<l.cols(); i++){
					s += std::exp(l(j,i));
				}
				for (int i =0; i<l.cols(); i++){
					int a  = std::exp(l(j,i));
					l(j,i) = ( a * (s - a)   ) /  (s*s);
				}
			}
			lastVal =  l;
		}
		if (op == "unary_square") lastVal =  l.unaryExpr(&sq);
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
	if (right == nullptr)return l;
	Eigen::MatrixXd r = right->getValue(it);

	
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
		if (op == "colwise*") { //I think this is colwise* .. oh well
			if (r.rows()==1){
				r = r.transpose();
			}
			if (l.rows()==1){
				l = l.transpose();
			}
			if (l.rows() != r.rows()){
				cError("cannot do colwsie*, size mismatch, left is " + std::to_string(l.rows()) + " while right is" +std::to_string(r.rows()) );
				return r;
			}
			else if (l.cols()!=1 && r.cols()!=1){
				cError("cannot do rowwsie*, neither is a column vector");
				return r;
			}
			bool right = false;
			int cols = r.cols();
			int rows = l.rows();
			if (cols==1){
				right = true;
				cols = l.cols();
			}
			for (int g = 0; g<rows; g++){
				for (int i = 0; i< cols; i++)
				{
					if (right)
						l(g,i) *= r(g,0);
					else
						r(g,i) *= l(g,0);
				}
			}
			if (right)	
				lastVal = l;
			else
				lastVal = r;
		}
		if (op == "elementwise*") {
			if (l.cols() != r.cols()){
				cError("cannot do elementwise*, size mismatch, left is " + std::to_string(l.cols()) + " while right is" +std::to_string(r.cols()) );
				return r;
			}
			if (l.rows() != r.rows()){
				cError("cannot do elementwise*, size mismatch, left is " + std::to_string(l.rows()) + " while right is" +std::to_string(r.rows()) );
				return r;
			}
			for (int g = 0; g<l.rows(); g++){
				for (int i = 0; i< l.cols(); i++)
				{
					l(g,i) *= r(g,i);
				}
			}
			lastVal = l;
			
		}
		if (op == "elementwise/") {
			if (l.cols() != r.cols()){
				cError("cannot do elementwise/, size mismatch, left is " + std::to_string(l.cols()) + " while right is" +std::to_string(r.cols()) );
				return r;
			}
			if (l.rows() != r.rows()){
				cError("cannot do elementwise/, size mismatch, left is " + std::to_string(l.rows()) + " while right is" +std::to_string(r.rows()) );
				return r;
			}
			for (int g = 0; g<l.rows(); g++){
				for (int i = 0; i< l.cols(); i++)
				{
					l(g,i) /= r(g,i);
				}
			}
			lastVal = l;
			
		}
		if (op == "rowwise*") {
			if (r.cols()==1){
				r = r.transpose();
			}
			if (l.cols()==1){
				l = l.transpose();
			}
			if (l.cols() != r.cols()){
				cError("cannot do rowwsie*, size mismatch, left is " + std::to_string(l.cols()) + " while right is" +std::to_string(r.cols()) );
				return r;
			}
			else if (l.rows()!=1 && r.rows()!=1){
				cError("cannot do rowwsie*, neither is a row vector");
				return r;
			}
			bool right = false; //right is not a row vector
			int cols = l.cols();
			int rows = r.rows();
			if (rows==1){
				right = true;
				rows = l.rows();
			}
			for (int g = 0; g<rows; g++){
				for (int i = 0; i< cols; i++)
				{
					if (right)
						l(g,i) *= r(0,i);
					else
						r(g,i) *= l(0,i);
				}
			}
			if (right)	
				lastVal = l;
			else
				lastVal = r;
		}
		if (op == "rowwise*OLD") {
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

		if (op == "+")lastVal = l + r;
		if (op == "-")lastVal = l - r;
		if (op == "*") lastVal = l * r; //figure out which kind of multiplication (matrix vs scalar)
		if (op == "/")lastVal = l / r(0,0);
		return lastVal;
	}
}
// getter that doesn't compute the value again
Eigen::MatrixXd Variable::getLastValue(){
	return lastVal;
}
// getter that prints returns current error code
int Variable::getErr(){
	return err;
};
// setter that resets err
void Variable::resetErr(){
	err = -199;
};