#ifndef _FUNCTION_H_
#define _FUNCTION_H_

#include "util.h"

#include <QSharedPointer>
#include <QString>
#include <map>

class RuntimeProvider;


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

	/*
	 * This constructor in fact does all the dirty job. The Upper layer takes care, that
	 * expr is a semantically logic formula: f(par1,...parx)=formula. If the formula is wrong,
	 * then life suxx for the programmer :(( but not me. The programmer, who created the function
	 */
    Function(const char *expr);
    virtual ~Function();

	/*
	 * This sets the value of a variable. 
	 */
    void SetVariable(const std::string&varn, double valu);

	/*
	 * This returns the value of the function for the given variables
	 */
    double Calculate(RuntimeProvider *rp);

    const std::string &get_name() const;

    std::vector<std::string> get_domain_variables() const;

    const QString &get_funBody() const;

private:
    //the name of the function. Ex.: f
    std::string m_name;
    //the variables of the function. Ex: "x" -> 12
    std::map<std::string, double> vars;

    QString funBody;

	//the tree of the function
	tree* root;

	/*
	 * Eliminates all spaces from the string, also it puts (0-1)* instead of a - sign
	 */
    std::string preverify_formula(char *expr);

	/*
	 * The dirty job maker :))
	 */
    void doit(const char *expr, tree* node);

	//look for a level 0 additive or multiplicative operator
    int l0ops(const char *expr, char op1, char op2);

	//looks for a lvel 0 additive operator
    int l0add(const char *expr);

	//looks for a level 0 multiplicative operator
    int l0mlt(const char *expr);

	/*
	 * This calculates the value of the expression, for defined values, and also numbers
	 */
    double calc(tree*node, RuntimeProvider *rp);

    void free_tree(tree* node);

	/*
	 * This depending on s returns the result of the operation betwen op1, op2
	 */
    double op(const std::string &s, double op1, double op2);

	/*
	 * returns true if s is a defined variable or a number
	 */
    int defd(const std::string &s, RuntimeProvider *rp);

	//this takes the value from the hashtable for s
    double value(const std::string &s, RuntimeProvider *rp);

    //

};

QSharedPointer<Function> temporaryFunction(const QString &definition);


#endif
