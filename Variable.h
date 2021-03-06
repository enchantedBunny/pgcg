#include <string>
#include <Eigen/Dense>
#include <algorithm> 
#include <map>
namespace calc {
	enum vType {independent, constant, function};
	enum fType { two, one_plus, one};
	class Variable {
		public:
			int id;
			int itid = -199;
			vType type;
			fType ftype;
			int rows = -1;
			int columns = -2;
			void setID(int bId);
			int getID();
			int deps_count;
			int opperand =1;
			void setOpperand(int p);
			Variable(const Eigen::Map<Eigen::MatrixXd> &mat);
			Eigen::MatrixXd val;
			Eigen::MatrixXd lastVal;
			void show();
			Variable(int &r, int &c);
			int *deps_list;
			Variable *left;
			Variable *right;
			int *l_ordered;
			int *r_ordered;
			int l_l = 0 ;	
			int r_l = 0;
			std::string op;
			int err = -199;
			int getErr();
			void resetErr();
			Variable(Variable *a, Variable *b, std::string *op, bool getDerivs = true);
			Variable(Variable *a, std::string *op, bool getDerivs = true);
			Eigen::MatrixXd getValue(int it); //from now on get value will only get the value, you feed the data before that
			Eigen::MatrixXd getLastValue();
			void setValue(int targetID, const Eigen::Map<Eigen::MatrixXd> &in);
	};
} 
