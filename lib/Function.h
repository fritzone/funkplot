#ifndef _FUNCTION_H_
#define _FUNCTION_H_

#include "IndexedAccess.h"
#include "ArithmeticAssignment.h"
#include "util.h"
#include "ArithmeticAssignmentToArrayElement.h"
#include "PointDefinitionAssignmentToOtherPoint.h"
#include <iomanip>
#include <sstream>

#include <QSharedPointer>
#include <QString>
#include <QString>
#include <map>
#include <set>
#include <optional>

class RuntimeProvider;
struct Assignment;


/*
 * This Class represents one function. A function needs:
 *  1. a name
 *  2. a variables (parameter) list
 *  3. a definition
 * And of course, a few methods, which will be desribed later.
 */

class Function
{
public:

    Function() = default;
    Function(const char *expr, Statement *s);
    virtual ~Function();

	/*
	 * This sets the value of a variable. 
	 */
    void SetVariable(const std::string&varn, double valu);

    double Calculate(int parFIdx = 1);

    const std::string &get_name() const;

    std::vector<std::string> get_domain_variables() const;

    void add_variable(const char*);

    const QString &get_funBody() const;

public:

    static QSharedPointer<Function> temporaryFunction(const QString& definition, Statement* s);
    static QSharedPointer<Function> temporaryFunction(const char* definition, Statement* s);

public:

    template<class T> static QSharedPointer<Function> temporaryFunction(T definition, Statement* s)
    {
        return Function::temporaryFunction(QString::number(definition, 'f', 6), s);
    }

    std::string getPreverified() const;

private:

    Function(const Function&) = delete;
    const Function& operator = (const Function&) = delete;

    /*
	 * Eliminates all spaces from the string, also it puts (0-1)* instead of a - sign
	 */
    std::string preverify_formula(char *expr);

	/*
	 * The dirty job maker :))
	 */
    void interpret(const char *expr, tree* node, RuntimeProvider *rp);

	//look for a level 0 additive or multiplicative operator
    int l0ops(const char *expr, char op1, char op2, char op3 = 0, char op4 = 0);

	//looks for a lvel 0 additive operator
    int l0add(const char *expr);

	//looks for a level 0 multiplicative operator
    int l0mlt(const char *expr);
	
	//looks for a level 0 comparison operator
    int l0cmp(const char *expr, std::string&);

    //looks for a level 0 logic operator: And, Or, Not
    int l0logic(const char *expr, std::string&zlop, const std::string &r);


	/*
	 * This calculates the value of the expression, for defined values, and also numbers
	 */
    std::optional<double> calc(tree*node, RuntimeProvider *rp, IndexedAccess*& ia, Assignment *&a, int parFIdx);

    void free_tree(tree* node);

	/*
	 * This depending on s returns the result of the operation betwen op1, op2
	 */
    std::optional<double> op(const std::string &s, double op1, double op2, RuntimeProvider *rp, int parFIdx);

	/*
	 * returns true if s is a defined variable or a number
	 */
    int defd(const std::string &s, RuntimeProvider *rp, Assignment *&assig);

	//this takes the value from the hashtable for s
    double value(const std::string &s, RuntimeProvider *rp);

    bool breakUpAsLogicOps(const char *expr, tree *node, const char* o, RuntimeProvider *rp);

    /*
     * This returns the value of the function for the given variables
     */
    std::optional<double> Calculate(RuntimeProvider *rp, IndexedAccess *&ia, Assignment *&a, int parFIdx = 1);
    friend bool ArithmeticAssignment::execute(RuntimeProvider*);
    friend bool ArithmeticAssignmentToArrayElement::execute(RuntimeProvider *);
    friend bool PointDefinitionAssignmentToOtherPoint::execute(RuntimeProvider *rp);
    friend class RuntimeProvider;


private:

    //the name of the function. Ex.: f
    std::string m_name;
    //the variables of the function. Ex: "x" -> 12
    std::map<std::string, double> vars;

    QString funBody;

    //the tree of the function
    tree* root = nullptr;
    std::string preverified;
};

#endif
