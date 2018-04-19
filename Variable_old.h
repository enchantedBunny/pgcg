#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <stdexcept>
#include <chrono>
#include <algorithm>
#include <math.h>
#include <Eigen/Dense>
namespace calc_old {
	enum vType {independent_old, constant_old, function_old, matrix_old, special_old};
	class Variable_old {
		public:
			int id;
			int pw;
			std::string tree[2];
			Variable_old *left;
			Variable_old *right;
			unsigned short deps[2];
			unsigned short depsList[2][100];
			unsigned short allOrderedDeps[100];
			unsigned short countOrderedDeps;
			std::string op;
			std::string build();
			void buildTree();
			std::string preview();
			std::string previewDeriv();
			vType type;
			float value;
			Variable_old();
			void setID(int bId);
			void i();
			void m(Variable_old *a);
			void c(float value);
			std::string f(Variable_old *a, Variable_old *b, std::string *op, bool getDerivs = true);
			std::string f(Variable_old *a, std::string *op, bool getDerivs = true);
			std::string f(Variable_old *a, std::string *op, int p, bool getDerivs = true);
			~Variable_old();
			float getValue(float *v);
			float getValue(float **v, int rows);
			float* feed(float **v, int rows);
			float* fValues;
			Variable_old* derivs[100];
			Variable_old* directDerivs[2];
			float getDerivValue(int g,float *v);
	};
} 
