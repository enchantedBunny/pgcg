#include <string>
#include <Eigen/Dense>
#include <algorithm> 
#include <map>
namespace calc {
	enum vType {independent, constant, function};
	class Variable {
		public:
			int id;
			vType type;
			int rows;
			int columns;
			void setID(int bId);
			int deps_count;
			//constant
			Variable(const Eigen::MatrixXd &mat);
			Eigen::MatrixXd val;
			void show();
			//independent
			Variable(int &r, int &c);
			//function
			int *deps_list;
			Variable *left;
			Variable *right;
			int *l_ordered;
			int *r_ordered;
			int l_l ;	
			int r_l ;
			std::string op;
			Variable(Variable *a, Variable *b, std::string *op, bool getDerivs = true);
			Variable(Variable *a, std::string *op, bool getDerivs = true);
			Variable(Variable *a, std::string *op, int p, bool getDerivs = true);
			//Eigen::MatrixXd getValue(const Eigen::MatrixXd &in);
			Eigen::MatrixXd getValue(); //from now on get value will only get the value, you feed the data before that
			void setValue(int targetID, const Eigen::MatrixXd &in);
	};
} 
