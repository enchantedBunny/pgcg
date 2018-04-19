#include "Variable_old.h"
#include <Python.h>
#include "master.h"
using namespace calc_old;
// 	cPrint("preview:"); will print preview :)
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
std::string Variable_old::preview(){
	if (type == constant_old)return "c";
	if (type == independent_old)return "v" + std::to_string(id);
	if (type ==  function_old)return "( " + tree[0]  + " " + op + " " + tree[1] + " )";
	//if (op ==  "pow")cPrint2(std::to_string(pw));
	if (type ==  special_old)return "( " + op + tree[0] + " )";
	return "sum( " + tree[0] + " )";
}
std::string Variable_old::previewDeriv(){
	return derivs[0]->preview();

}
std::string Variable_old::build(){
	if (type == constant_old)return std::to_string(value);
	if (type == independent_old)return "v" + std::to_string(id);
	if (type ==  function_old)return "( " + left->build() + " " + op + " " + right->build() + " )";
	if (type ==  special_old)return "( " + op + left->build() + " )";
	return "sum( " + left->build() + " )";
}
void Variable_old::buildTree(){
	tree[0] = left->build();
	tree[1] = right->build();	
}
unsigned short getFirst(std::string &s){
	size_t l = s.find('v');
	s[l]='g';
	std::string out = "";
	if (s.length()>l+1)if ('0' <= s[l+1] && s[l+1] <= '9'){
		out += s[l+1];
		if (s.length()>l+2)if ('0' <= s[l+2] && s[l+2] <= '9'){
			out += s[l+2];
			if (s.length()>l+3)if ('0' <= s[l+3] && s[l+3] <= '9')
				out += s[l+3];
		}
	}
	return atoi(out.c_str());
}
std::string Variable_old::f(Variable_old *a, Variable_old *b, std::string *o, bool getDerivs)
{
	type = function_old;
	left = a;
	right = b;
	op = *o;
	buildTree();
	for (int i = 0;i<2; i++){
		deps[i] = (unsigned short)std::count(tree[i].begin(), tree[i].end(), 'v');
	}
	int i2 = 0;
	while (tree[0].find('v') != std::string::npos){depsList[0][i2] = getFirst(tree[0]);i2++;}
	i2 = 0;
	while (tree[1].find('v') != std::string::npos){depsList[1][i2] = getFirst(tree[1]);i2++;}
	int n = deps[0] + deps[1];
	unsigned short * allDepsWithDuplicates = new unsigned short[n];
	std::copy(depsList[0], depsList[0] + deps[0], allDepsWithDuplicates);
	std::copy(depsList[1], depsList[1] + deps[1], allDepsWithDuplicates + deps[0]);	
	unsigned short i, j, k;
	for(i=0;i<n;i++)
        for(j=i+1;j<n;)
        {
            if(allDepsWithDuplicates[i]==allDepsWithDuplicates[j])
            {
                for(k=j;k<n-1;++k)
                    allDepsWithDuplicates[k]=allDepsWithDuplicates[k+1];
                --n;
            }
            else
                ++j;
        }
    std::sort(allDepsWithDuplicates, allDepsWithDuplicates+n);
	std::string out = "";
	
	for(j=0;j<n;j++){
         allOrderedDeps[j] = allDepsWithDuplicates[j];
		 out += std::to_string(allOrderedDeps[j]) + " ";
	}
	countOrderedDeps = n;
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
		}
		if (op == "-"){
			directDerivs[0]= one;
			directDerivs[1]= mone;
		}
		if (op == "/"){ //  f(x)/g(x)
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
		for (int gg=0; gg < n; gg++){
			//cPrint("find deriv of id"+std::to_string(allOrderedDeps[gg]));
			//cPrint("right has deps: " + std::to_string(right->countOrderedDeps));
			//cPrint("right has a dep: " + std::to_string(right->allOrderedDeps[0]));
			bool inleft = std::find(std::begin(left->allOrderedDeps), std::end(left->allOrderedDeps), allOrderedDeps[gg]) != std::end(left->allOrderedDeps);
			bool inright = std::find(std::begin(right->allOrderedDeps), std::end(right->allOrderedDeps), allOrderedDeps[gg]) != std::end(right->allOrderedDeps);
			//cPrint("bef");
			//cPrint(inleft ? "inleft: true":"inleft: false");
			//cPrint(inright ? "inright: true":"inright: false");
			if (left->id == allOrderedDeps[gg] || left->countOrderedDeps == 1)inleft = true;
			else if (left->type ==independent_old || left->type ==constant_old) inleft = false;
			if (right->id == allOrderedDeps[gg] || right->countOrderedDeps == 1)inright = true;
			else if (right->type ==independent_old || right->type ==constant_old) inright = false;
			//cPrint(inleft ? "inleft: true":"inleft: false");
			//cPrint(inright ? "inright: true":"inright: false");
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
				int c = 0;
				while (c<left->countOrderedDeps){
					if (left->allOrderedDeps[c] == allOrderedDeps[gg]){
						break;
					}
					c++;
				}
				wip->f(directDerivs[0], left->derivs[c], mult, false);
				derivs[gg] = wip;
			}	
			else if (!inleft && inright){   //b f(a)
				int c = 0;
				while (c<right->countOrderedDeps){
					if (right->allOrderedDeps[c] == allOrderedDeps[gg]){

					//cPrint("it's " + std::to_string(c));
						break;
					}
					c++;
				}
				//LAZY  
				wip->f(directDerivs[1], right->derivs[0], mult, false);
				derivs[0] = wip;
				}	
			//else cPrint("probably error :D");
			
		}
		
	}
	if (getDerivs){
		float * h = new float[1];
		h[0] = 0;
		return "value at 0 of the 0th direct deriv is " + std::to_string(directDerivs[0]->getValue(h)) +"value at 0 of the 1st direct deriv is " + std::to_string(directDerivs[1]->getValue(h)) + "value at 0 of the deriv is " + std::to_string(derivs[0]->getValue(h)) + "the 0th deriv of this is" + derivs[0]->preview();
	
	}
	return "This function_old has " + std::to_string(countOrderedDeps) + " dependencies";

}
std::string Variable_old::f(Variable_old *a, std::string* o, bool getDerivs)
{
	type = special_old;
	left = a;
	std::string gop = static_cast<std::string>(*o);
	tree[0] = left->build();
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
			for (int g = 0; g < countOrderedDeps; g++){
				allOrderedDeps[g] = left->allOrderedDeps[g];
				}
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
			for (int g = 0; g < countOrderedDeps; g++){
				allOrderedDeps[g] = left->allOrderedDeps[g];
			}
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
			allOrderedDeps[0] = left->id;
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
		return "value at 0 of the deriv is " + std::to_string(derivs[0]->getValue(h)) + "the 0th deriv of this is" + derivs[0]->preview();
	
	}
	return "hope" ;

}
std::string Variable_old::f(Variable_old *a, std::string *o, int p, bool getDerivs){
	type = special_old;
	left = a;
	std::string gop = static_cast<std::string>(*o);
	tree[0] = left->build();
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
		return "value at 0 of the deriv is " + std::to_string(derivs[0]->getValue(h)) + "the 0th deriv of this is" + derivs[0]->preview();
	}
	return "hope" ;
}
void Variable_old::m(Variable_old *a)
{
	type = matrix_old;
	left = a;
	tree[0] = left->build();
}

Variable_old::~Variable_old()
{
}
float Variable_old::getValue(float **v, int rows){
	float* vals = left->feed(v, rows);
	float out=0;
	for (int i = 0; i<rows;i++){
		out += vals[i];
	}
	return out/rows;
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
	float lout [deps[0]];
	float rout [deps[1]];
	for (unsigned short i = 0; i<countOrderedDeps; i++){
		//int x = std::distance(depsList[0], std::find(depsList[0], depsList[0] + deps[0], allOrderedDeps[i]));
		for (unsigned short hh = 0; hh<deps[0];hh++){
			if (depsList[0][hh] == allOrderedDeps[i]){
				lout[hh] = v[i];
			}
		}
		for (unsigned short hh = 0; hh<deps[1];hh++){
			if (depsList[1][hh] == allOrderedDeps[i]){
				rout[hh] = v[i];
			}
		}
	}
	float lv = left->getValue(lout);
	float rv = right->getValue(rout);

	if (op == "+")return  lv + rv;
	if (op == "-")return  lv - rv;
	if (op == "*")return  lv * rv;
	if (op == "/")return  lv / rv;
	return -42;
}
float* Variable_old::feed(float **v, int rows){
	float out[rows]; 
	for (int i = 0; i< rows;i++){
		out[i] = getValue(v[i]);
	}
	fValues = out;
	return fValues;
}
float Variable_old::getDerivValue(int g,float *v){
	float* pass = new float[derivs[g]->countOrderedDeps];
	int help = 0;

	// for (int i=0; i<countOrderedDeps; i++){
	// 	bool inDeriv;
	// 	if (derivs[g]->type == function_old || derivs[g]->type == special_old)
	// 	inDeriv = std::find(std::begin(derivs[g]->allOrderedDeps), std::end(derivs[g]->allOrderedDeps), allOrderedDeps[i]) != std::end(derivs[g]->allOrderedDeps);
	// 	else if (derivs[g]->type == independent_old && derivs[g]->id == allOrderedDeps[i])
	// 	inDeriv = true;
	// 	else
	// 	inDeriv = false;
	// 	if (inDeriv){
	// 		pass[help] = v[i];
	// 		help++;
	// 	}
	// }
	//return derivs[g]->getValue(v);
	return directDerivs[1]->getValue(v) * right->derivs[0]->getValue(v);
}