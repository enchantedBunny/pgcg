#include <string>
#include <Eigen/Dense>
#include <algorithm> 
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
			std::string op;
			Variable(Variable *a, Variable *b, std::string *op, bool getDerivs = true);
			Variable(Variable *a, std::string *op, bool getDerivs = true);
			Variable(Variable *a, std::string *op, int p, bool getDerivs = true);
			Eigen::MatrixXd getValue(const Eigen::MatrixXd &in);

	};
} 
