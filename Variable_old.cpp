#include "Variable_old.h"
#include <Python.h>
#include "master.h"
using namespace calc_old;
Variable_old::Variable_old()
{
	
}
void Variable_old::setID(int bId)
{
	id = bId;
}
void Variable_old::i()
{
	type = independent_old;
}
void Variable_old::c(float v)
{
	type = constant_old;	
	value = v;
}

void Variable_old::f(Variable_old *a, Variable_old *b, std::string *o, bool getDerivs)
{
	type = function_old;
	left = a;
	right = b;
	op = *o;
	cPrint("here2definition00     " + std::to_string(a->countOrderedDeps));
	if (a->type == function_old || a->type == special_old){
		l_l = a->countOrderedDeps;
		l_ordered = a->allOrderedDeps;
	}
	else if (a->type == constant_old){
		l_l = 0;
	}
	else if (a->type == independent_old){
		l_l = 1;
		l_ordered = new unsigned short [1];
		l_ordered[0] = a->id;
	}
	cPrint("here2definition01     " + std::to_string(l_l));
	if (b->type == function_old || b->type == special_old){
		r_l = b->countOrderedDeps;
		r_ordered = b->allOrderedDeps;
	}
	else if (b->type == constant_old){
		r_l = 0;
	}
	else if (b->type == independent_old){
		r_l = 1;
		r_ordered = new unsigned short [1];
		r_ordered[0] = b->id;
	}

	if (a->type == constant_old && (b->type == function_old || b->type == special_old)){
		allOrderedDeps = b->allOrderedDeps;
		countOrderedDeps = b->countOrderedDeps;
		cPrint("here2definition02     " + std::to_string(countOrderedDeps));
	}
	else if ((a->type == function_old || a->type == special_old) && b->type == constant_old){
		allOrderedDeps = a->allOrderedDeps;
		countOrderedDeps = a->countOrderedDeps;
		cPrint("here2definition03     " + std::to_string(countOrderedDeps));
	}
	else if (a->type == independent_old && b->type == constant_old){
		allOrderedDeps = new unsigned short[1];
		allOrderedDeps[0] = a->id;
		countOrderedDeps = 1;
	}
	else if (a->type == constant_old && b->type == independent_old){
		allOrderedDeps = new unsigned short[1];
		allOrderedDeps[0] = b->id;
		countOrderedDeps = 1;
	}
	else if (a->type == constant_old && b->type == constant_old){
		countOrderedDeps = 0;
	}
	else if (a->type == independent_old && b->type == independent_old){
		if (a->id == b->id){
			countOrderedDeps = 1;
			allOrderedDeps = new unsigned short [1];
			allOrderedDeps[0] = a->id;
		}
		countOrderedDeps = 2;
		allOrderedDeps = new unsigned short [2];
		allOrderedDeps[0] = std::min(a->id, b->id);
		allOrderedDeps[1] = std::max(a->id, b->id);
	}
	else if (a->type == independent_old && (b->type == function_old || b->type == special_old)){
		bool in = false;
		for (int i = 0; i++; i<r_l){
			if (r_ordered[i]==a->id){
				in = true;
			}
		}
		if(in){
			allOrderedDeps = r_ordered;
			countOrderedDeps = r_l;
		}
		else{
			countOrderedDeps = r_l + 1;
			allOrderedDeps = new unsigned short [countOrderedDeps];
			int ri = 0;
			bool done=  false;
			for (int i = 0; i++; i<countOrderedDeps){
				if (r_ordered[ri]<a->id || done){
					allOrderedDeps[i] = r_ordered[ri];
					ri++;
				}
				else{
					allOrderedDeps[i] = a->id;
					done = true;
				}
			}
		}
	}
	else if ((a->type == function_old || a->type == special_old) && b->type == independent_old){
		bool in = false;
		for (int i = 0; i++; i<l_l){
			if (l_ordered[i]==b->id){
				in = true;
			}
		}
		if(in){
			allOrderedDeps = l_ordered;
			countOrderedDeps = l_l;
		}
		else{
			countOrderedDeps = l_l + 1;
			allOrderedDeps = new unsigned short [countOrderedDeps];
			int li = 0;
			bool done=  false;
			for (int i = 0; i++; i<countOrderedDeps){
				if (l_ordered[li]<b->id || done){
					allOrderedDeps[i] = l_ordered[li];
					li++;
				}
				else{
					allOrderedDeps[i] = b->id;
					done = true;
				}
			}
		}
	}
	else if ((a->type == function_old || a->type == special_old) && (b->type == function_old || b->type == special_old) ){
		int uniques = r_l + l_l;
			for (int i = 0; i< r_l; i++){
				for (int ii = 0; ii< l_l; ii++){
					if (r_ordered[i] == l_ordered[ii])
						uniques--;
				}
			}
			countOrderedDeps = uniques;
			allOrderedDeps = new unsigned short[uniques];
			int ri = 0;
			int li = 0;
			for (int i = 0; i<uniques; i++){
				if (ri < r_l and li < l_l){
					if (r_ordered[ri]<l_ordered[li]){
						allOrderedDeps[i] = r_ordered[ri];
						ri++;
					}
					else if (r_ordered[ri]>l_ordered[li]){
						allOrderedDeps[i] = l_ordered[li];
						li++;
					}
					else{
						allOrderedDeps[i] = r_ordered[ri];
						ri++;
						li++;
					}
				}
				else if (ri < r_l and li >= l_l){
					allOrderedDeps[i] = r_ordered[ri];
					ri++;
				}
				else if (ri >= r_l and li < l_l){
					allOrderedDeps[i] = l_ordered[li];
					li++;
				}
			
			}
	}


	if (getDerivs){
		std::string* add = new std::string;
		*add = "+";
		std::string* mult = new std::string;
		*mult = "*";
		std::string* di = new std::string;
		*di = "/";
		if (op == "*")
		{	
			if (right->id == left->id){
				Variable_old* nlob = new Variable_old;
				Variable_old* fblob = new Variable_old;
				fblob->c(2);
				nlob->f(fblob,a, mult, false);
				directDerivs[0]= nlob;
			}
			else{
				directDerivs[0]=b;
				directDerivs[1]=a;
			}
		}
		Variable_old* one = new Variable_old;
		Variable_old* mone = new Variable_old;
		one->c(1);
		mone->c(-1);
		if (op == "+"){
			directDerivs[0]= one;
			directDerivs[1]= one;
			cPrint("here");
		}
		else if (op == "-"){
			directDerivs[0]= one;
			directDerivs[1]= mone;
		}
		else if (op == "/"){ //  f(x)/g(x)
			Variable_old* flob = new Variable_old;
			flob->f(one,b, di, false);	// 1/g(x)
			directDerivs[0]= flob;
			Variable_old* fglob = new Variable_old;
			fglob->f(mone,a, mult, false);	 //-f(x)
			Variable_old* klob = new Variable_old;
			klob->f(b,b, mult, false);     //g(x)**2
			Variable_old* tlob = new Variable_old;
			tlob->f(fglob,klob, di, false); //-f(x)/g(x)**2
			directDerivs[1]= tlob;
		}
		if (op == "+"){
				cPrint("here2    " + std::to_string(countOrderedDeps));
			}
		for (int gg=0; gg < countOrderedDeps; gg++){
			//bool inleft = std::find(std::begin(left->allOrderedDeps), std::end(left->allOrderedDeps), allOrderedDeps[gg]) != std::end(left->allOrderedDeps);
			//bool inright = std::find(std::begin(right->allOrderedDeps), std::end(right->allOrderedDeps), allOrderedDeps[gg]) != std::end(right->allOrderedDeps);
			bool inleft = false;
			bool inright = false;
			if (op == "+"){
				cPrint("here");
			}

			if (l_l>0){
			for (int i = 0; i<l_l; i++){
				if (allOrderedDeps[gg] == l_ordered[i]){
					inleft = true;
					break;
				}
			}
			}
			if (r_l>0){
			for (int i = 0; i<r_l; i++){
				if (allOrderedDeps[gg] == r_ordered[i]){
					inright = true;
					break;
				}
			}
			}
			if (op == "+"){
				cPrint("here");
			}
			// cPrint("bef");
			cPrint(inleft ? "inleft: true":"inleft: false");
			cPrint(inright ? "inright: true":"inright: false");
			// if (left->id == allOrderedDeps[gg] || left->countOrderedDeps == 1)inleft = true;
			// else if (left->type ==independent_old || left->type ==constant_old) inleft = false;
			// if (right->id == allOrderedDeps[gg] || right->countOrderedDeps == 1)inright = true;
			// else if (right->type ==independent_old || right->type ==constant_old) inright = false;
			// cPrint(inleft ? "inleft: true":"inleft: false");
			// cPrint(inright ? "inright: true":"inright: false");
			Variable_old* wip = new Variable_old;
			if (right->id == allOrderedDeps[gg] && left->id == allOrderedDeps[gg] ){  //a a 
				derivs[gg] = directDerivs[0]; //which equalts directDerivs[1]
			}
			else if (!inright && left->id == allOrderedDeps[gg] ){   //a b
				derivs[gg] = directDerivs[0];
			}
			else if (!inleft && right->id == allOrderedDeps[gg]){ //b a
				derivs[gg] = directDerivs[1];
			}
			else if (inright && left->id == allOrderedDeps[gg]){   //a f(a)
				int c = 0;
				while (c<left->countOrderedDeps){
					if (left->allOrderedDeps[c] == allOrderedDeps[gg]){
						break;
					}
					c++;
				}
				wip->f(directDerivs[0], left->derivs[c], mult, false);
				Variable_old* wip2 = new Variable_old;
				wip2->f(directDerivs[1], wip, add, false);
				derivs[gg] = wip2;
			}
			else if (inleft && right->id == allOrderedDeps[gg]){   //f(a) a
				int c = 0;
				while (c<left->countOrderedDeps){
					if (left->allOrderedDeps[c] == allOrderedDeps[gg]){
						break;
					}
					c++;
				}
				wip->f(directDerivs[0], left->derivs[c], mult, false);
				Variable_old* wip2 = new Variable_old;
				wip2->f(directDerivs[1], wip, add, false);
				derivs[gg] = wip2;
			}
			else if (inleft && inright){   //f(a) f(a)
				int c = 0;
				while (c<left->countOrderedDeps){
					if (left->allOrderedDeps[c] == allOrderedDeps[gg]){
						break;
					}
					c++;
				}
				wip->f(directDerivs[0], left->derivs[c], mult, false);//left
				c = 0;
				while (c<right->countOrderedDeps){
					if (right->allOrderedDeps[c] == allOrderedDeps[gg]){
						break;
					}
					c++;
				}
				Variable_old* wip2 = new Variable_old;
				Variable_old* wip3 = new Variable_old;
				wip2->f(directDerivs[1], right->derivs[c], mult, false);//right
				wip3->f(wip, wip2, add, false);
				derivs[gg] = wip3;
			}	
			else if (inleft && !inright){   //f(a) b
				cPrint("here i guess");
				int c = 0;
				while (c<left->countOrderedDeps){
					if (left->allOrderedDeps[c] == allOrderedDeps[gg]){
						break;
					}
					c++;
				}
				wip->f(directDerivs[0], left->derivs[0], mult, false);
				derivs[gg] = wip;
			}	
			else if (!inleft && inright){   //b f(a)
				int c = 0;
				while (c<right->countOrderedDeps){
					if (right->allOrderedDeps[c] == allOrderedDeps[gg]){

					cPrint("it's " + std::to_string(c));
					break;
					}
					c++;
				}
				//LAZY  

				float v[1] = {0};  
				wip->f(directDerivs[1], right->derivs[c], mult, false);
				derivs[gg] = wip;

				cPrint(std::to_string(directDerivs[1]->getValue(v)));
				cPrint(std::to_string(right->derivs[c]->getValue(v)));
				cPrint("wip " + std::to_string(wip->getValue(v)));
				std::string* multut = new std::string;
				*multut = "*";
				Variable_old* wip22 = new Variable_old;
				wip22->f(directDerivs[1], right->derivs[c], multut, false);
				

				cPrint("wip22 " + std::to_string(wip22->getValue(v)));

				derivs[gg] = wip22;

				cPrint("derv " + std::to_string(derivs[gg]->getValue(v)));
				
			}	
			//else cPrint("probably error :D");
			
		}	
	}
	
}
void Variable_old::f(Variable_old *a, std::string* o, bool getDerivs)
{
	type = special_old;
	left = a;
	std::string gop = static_cast<std::string>(*o);
	op = gop;
	std::string* e = new std::string;
	*e = "exp";
	std::string* sigmoid = new std::string;
	*sigmoid = "sig";
	std::string* mult = new std::string;
	*mult = "*";
	std::string* minus = new std::string;
	*minus = "-";
	if (op == "exp")
	{
		if (left->type ==function_old){
			countOrderedDeps = left->countOrderedDeps;
			allOrderedDeps = left->allOrderedDeps;
			cPrint("hereexp     " + std::to_string(countOrderedDeps));
			if (getDerivs){
				Variable_old* wip2 = new Variable_old;
				wip2->f(left, e, false);
				for (int g = 0; g < countOrderedDeps; g++){
						Variable_old* wip = new Variable_old;
						wip->f(left->derivs[g], wip2, mult, false);
						derivs[g] = wip;
				}
			}
			

		}
		if (left->type == independent_old){
			countOrderedDeps = 1;
			allOrderedDeps = new unsigned short[1];
			allOrderedDeps[0] = left->id;
			if (getDerivs){
				Variable_old* wip2 = new Variable_old;
				wip2->f(left, e, false);
				derivs[0] = wip2;
			}
		}
	
	}
	if (op == "sig")
	{
		if (left->type ==function_old){
			countOrderedDeps = left->countOrderedDeps;
			allOrderedDeps = left->allOrderedDeps;
			if (getDerivs){
				Variable_old* wip2 = new Variable_old;
				Variable_old* wip3 = new Variable_old;
				Variable_old* wip4 = new Variable_old;
				Variable_old* wip5 = new Variable_old;
				wip2->f(left, sigmoid, false);
				wip3->c(1);
				wip4->f(wip3, wip2, minus,false);
				wip5->f(wip4, wip2, mult,false);
				for (int g = 0; g < countOrderedDeps; g++){
						Variable_old* wip = new Variable_old;
						wip->f(left->derivs[g], wip5, mult, false);
						derivs[g] = wip;
				}
			}
			

		}
		if (left->type == independent_old){
			countOrderedDeps = 1;
			allOrderedDeps = new unsigned short[1];
			if (getDerivs){
				Variable_old* wip2 = new Variable_old;
				Variable_old* wip3 = new Variable_old;
				Variable_old* wip4 = new Variable_old;
				Variable_old* wip5 = new Variable_old;
				wip2->f(left, sigmoid, false);
				wip3->c(1);
				wip4->f(wip3, wip2, minus,false);
				wip5->f(wip4, wip2, mult,false);
				derivs[0] = wip5;
			}
		}
	
	}
	if (getDerivs){
		float * h = new float[1];
		h[0] = 0;
		
	}
}
void Variable_old::f(Variable_old *a, std::string *o, int p, bool getDerivs){
	type = special_old;
	left = a;
	std::string gop = static_cast<std::string>(*o);
	op = gop;
	pw = p;
	std::string* pwt = new std::string;
	*pwt = "pow";
	std::string* mult = new std::string;
	*mult = "*";
	if (op == "pow")
	{
		if (left->type ==function_old){
			countOrderedDeps = left->countOrderedDeps;
			for (int g = 0; g < countOrderedDeps; g++){
				allOrderedDeps[g] = left->allOrderedDeps[g];
			}
			if (getDerivs){
					Variable_old* c1 = new Variable_old;
					c1->c(pw);  //3
					Variable_old* wip2 = new Variable_old;
					Variable_old* wip3 = new Variable_old;
					if (pw == 2){
						wip3->f(left, c1, mult, false);//2x
					}
					else{
						wip2->f(left, pwt, pw-1, false);//x^2
						wip3->f(wip2, c1, mult, false);//3x^2
					}
					for (int g = 0; g < countOrderedDeps; g++){
						if (getDerivs){
							Variable_old* wip = new Variable_old;
							wip->f(left->derivs[g], wip3, mult, false);
							derivs[g] = wip;
						}
					
				}
			}
			

		}
		if (left->type == independent_old){
			countOrderedDeps = 1;
			allOrderedDeps[0] = left->id;
			if (getDerivs){
				Variable_old* c1 = new Variable_old;
				c1->c(pw);
				Variable_old* wip2 = new Variable_old;
				wip2->f(left, pwt, pw-1, false); //x^n-1
				Variable_old* wip3 = new Variable_old;
				wip3->f(wip2, c1, mult, false);
				derivs[0] = wip3;
			}
		}
	
	}
	if (getDerivs){
		float * h = new float[1];
		h[0] = 0;
	}
}

float Variable_old::getValue(float *v)
{
   	if (type == constant_old)return value;
	if (type == independent_old)return v[0];
	if (type == special_old){
		if (op == "exp"){
			float ev = exp(left->getValue(v));
			return ev;
		}
		if (op == "sig"){
			float ev = 1/(1+exp(-left->getValue(v)));
			return ev;
		}
		if (op == "pow"){
			float ev = std::pow(left->getValue(v), pw);
			return ev;
		}
	}
	if(left->id == right->id && op == "*"){
		float lv = left->getValue(v);
		return lv*lv;
	}
	float lv;
	if (l_l > 0){
		float lout [l_l];
		for (unsigned short i = 0; i<countOrderedDeps; i++){
			for (unsigned short hh = 0; hh<l_l;hh++){
				if (l_ordered[hh] == allOrderedDeps[i]){
					lout[hh] = v[i];
				}
			}
		}
		lv = left->getValue(lout);
	}
	else lv = left->getValue(v);
	float rv;
	if (r_l > 0){
		float rout [r_l];
		for (unsigned short i = 0; i<countOrderedDeps; i++){
			for (unsigned short hh = 0; hh<r_l;hh++){
				if (r_ordered[hh] == allOrderedDeps[i]){
					rout[hh] = v[i];
				}
			}
		}
		rv = right->getValue(rout);
	}
	else rv = right->getValue(v);
	cPrint("left value: "  + std::to_string(lv));
	cPrint("right value: " +std::to_string(rv));
	if (op == "+")return  lv + rv;
	if (op == "-")return  lv - rv;
	if (op == "*")return  lv * rv;
	if (op == "/")return  lv / rv;
	return -42;
}

float Variable_old::getDerivValue(int g,float *v){
	return derivs[g]->getValue(v);
	}