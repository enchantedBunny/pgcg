#include <string>
#include <stdlib.h>
#include <algorithm>
#include <math.h>
#include <Eigen/Dense>
namespace calc_old {
	enum vType {independent_old, constant_old, function_old, matrix_old, special_old};
	class Variable_old {
		public:
			int id;
			int pw;
			Variable_old *left;
			Variable_old *right;
			unsigned short* allOrderedDeps;
			unsigned short countOrderedDeps = 0;
			unsigned short *l_ordered;
			unsigned short *r_ordered;
			unsigned short l_l = 0 ;	
			unsigned short r_l = 0;
			std::string op;
			vType type;
			float value;
			Variable_old();
			void setID(int bId);
			void i();
			void c(float value);
			void f(Variable_old *a, Variable_old *b, std::string *op, bool getDerivs = true);
			void f(Variable_old *a, std::string *op, bool getDerivs = true);
			void f(Variable_old *a, std::string *op, int p, bool getDerivs = true);
			float getValue(float *v);
			Variable_old* derivs[100];
			Variable_old* directDerivs[2];
			float getDerivValue(int g,float *v);
	};
} 
