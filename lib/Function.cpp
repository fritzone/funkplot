#include "Function.h"
#include "util.h"
#include "RuntimeProvider.h"
#include <QDebug>
#include <math.h>
#include <string.h>
#include <memory>
#include <set>
#include <charconv>

Function::Function(const char *expr, Statement* s) : funBody(expr)
{
    const char* eqp_chr = strchr(expr, '=');
    if(!eqp_chr)
    {
        throw syntax_error_exception(ERRORCODE(16), "Invalid function definition: %s. Missing assignment in body.", expr);
    }
    int eqpos = (strchr(expr, '=') - expr);
    char* expr_p1 = before(eqpos, expr);
    char* ppar = strchr(expr_p1, '(');
    if(!ppar)
    {
        throw syntax_error_exception(ERRORCODE(17), "Improper parametrization of the given function: <b>%s</b> (missing parameters, such as f(x) = ...)", expr_p1);
    }
    expr_p1[ppar - expr_p1] = 0;
    m_name = expr_p1;
    m_name = strim(m_name);
    ppar++;

    char* pparc = strchr(ppar, ')');

    if(!pparc)
    {
        throw syntax_error_exception(ERRORCODE(17), "Improper parametrization of the given function: <b>%s</b> (missing closing parenthesis)", expr_p1);
    }
    ppar[pparc - ppar] = 0;
    while (ppar)
    {
        char* pve = strchr(ppar, ',');
        if (pve != nullptr)
        {
            ppar[pve - ppar] = 0;
            std::string sst(ppar);
            std::string vn = strim(sst);
            vars[vn] = std::numeric_limits<double>::quiet_NaN();
            ppar = pve + 1;
        }
        else
        {
            std::string sst(ppar);

            std::string ujv = strim(sst);
            vars[ujv] = std::numeric_limits<double>::quiet_NaN();
            ppar = nullptr;
        }
    }

    // now begin translating the part two of the formula
    char* expr_p2 = after(eqpos, expr);
    std::string sc = preverify_formula(expr_p2);
    char* cexpr_p2 = strdup(sc.c_str());
    // first: a new tree will be created
    root = new tree;
    root->left = nullptr;
    root->right = nullptr;
    root->stmt = s;

    doit(cexpr_p2, root, RuntimeProvider::get());
    free(reinterpret_cast<void*>(cexpr_p2));
    delete[] expr_p1;
    delete[] expr_p2;

}

Function::~Function()
{
    free_tree(root);
}

void Function::SetVariable(const std::string& varn, double valu)
{
    std::string sst(varn);

    vars[sst] = valu;
}

double Function::Calculate(RuntimeProvider *rp, IndexedAccess*& ia, Assignment*& a)
{
    double res = calc(root, rp, ia, a);
    if(ia != nullptr)
    {
        res =rp->getIndexedVariableValue(ia->indexedVariable.toStdString().c_str(), ia->index);
        if(!std::isnan(res))
        {
            delete ia;
            ia = nullptr;
        }
    }
    return res;
}

double Function::Calculate()
{
    IndexedAccess* ia = nullptr;
    Assignment* a = nullptr;
    return Calculate(RuntimeProvider::get(), ia, a);
}

const std::string &Function::get_name() const
{
    return m_name;
}

std::vector<std::string> Function::get_domain_variables() const
{
    std::vector<std::string> result;

    std::transform(vars.begin(), vars.end(), std::back_inserter(result), [](std::pair<std::string, double> p) -> std::string { return p.first; });

    return result;
}

void Function::add_variable(const char * vn)
{
    vars[vn] = 0;
}

const QString &Function::get_funBody() const
{
    return funBody;
}

std::string Function::preverify_formula(char* expr)
{
    std::string s;
    auto l = strlen(expr);
    for (int i = 0; i < l; i++) {
        if (expr[i] == ',' || expr[i] == '$' || isalnum(expr[i]) || isoperator(expr[i]) || isparenthesis(expr[i]))
        {
            if (expr[i] != '-')
            {
                s += expr[i];
            }
            else
            {
                if(s.empty() || s[s.length() - 1] == '(')
                {
                    s += "(0-1)*";
                }
                else
                {
                    s += "+(0-1)*";
                }
            }
        }
        if(std::isspace(expr[i]))
        {
            // peek forward, see if we have a keyword there
            int j = i;
            while(isspace(expr[j])) j++;
            int firstNonSpace = j;
            std::string peekWord;
            while(isalnum(expr[j]))
            {
                peekWord += expr[j]; j++;
            }
            if(islogicop(peekWord))
            {
                s += " " + peekWord + " ";
                i = j - 1;
            }

        }
    }
    return s;
}

void Function::doit(const char* expr, tree* node, RuntimeProvider* rp)
{

    // or check comes before "and" check, because and has higher priority
    if(breakUpAsLogicOps(expr, node, "or", rp))
    {
        return;
    }

    if(breakUpAsLogicOps(expr, node, "and", rp))
    {
        return;
    }

    std::string zlcmpOp;
    int zlcmp = l0cmp(expr, zlcmpOp);
    if(zlcmp != -1)
    {
        char* beforOp = before(zlcmp, expr);
        const char *afterOp = expr + zlcmp + zlcmpOp.length();
        
        node->info = zlcmpOp;
        node->left = new tree;
        node->left->parent = node;

        doit(beforOp, node->left, rp);

        node->right = new tree;
        node->right->parent = node;
        doit(afterOp, node->right, rp);
        
        return;
    }    
    
    int zladd = l0add(expr), zlmlt = l0mlt(expr),
            zlop = zladd == -1 ? zlmlt == -1 ? -1 : zlmlt : zladd;

    if (zlop != -1) 
    {
        char* beforer = before(zlop, expr);
        if (strlen(beforer) == 0) {
            throw syntax_error_exception(ERRORCODE(18), "Possible error in statement: <b>%s</b>. No data before position %i (%c)", expr, zlop, expr[zlop]);
        }
        char* afterer = after(zlop, expr);
        if (strlen(afterer) == 0) {
            throw syntax_error_exception(ERRORCODE(19), "Possible error in statement: <b>%s</b>. No data after position %i (%c)", expr, zlop, expr[zlop]);
        }

        node->info = c2str(expr[zlop]);
        node->left = new tree;
        node->left->parent = node;

        doit(beforer, node->left, rp);
        delete[] beforer;

        node->right = new tree;
        node->right->parent = node;
        doit(afterer, node->right, rp);
        delete[] afterer;
    }
    else
    {
        if (expr[0] == '(')
        {
            if (expr[strlen(expr) - 1] == ')')
            {
                char* expr2 = new char[strlen(expr)];
                const char* ext = expr + 1;
                strcpy(expr2, ext);
                expr2[strlen(expr2) - 1] = 0;
                if (strlen(expr2) == 0)
                {
                    delete[] expr2;
                    throw syntax_error_exception(ERRORCODE(20), "Syntax error in statement: <b>%s</b>. Unmatched empty parenthesis", expr);
                }
                doit(expr2, node, rp);
                delete[] expr2;
            }
            else
            {
                throw syntax_error_exception(ERRORCODE(21), "Possible error in statement: <b>%s</b>. Not found enclosing parenthesis", expr);
            }

        }
        else
        {
            char* iffunc = isfunc(expr, rp);
            std::unique_ptr<char[]> wrapper;
            wrapper.reset(iffunc);

            if (iffunc) {
                std::string siffunc (iffunc);
                node->info = iffunc;

                if(siffunc == "pow")
                {
                    // the power function has 2 parameters

                    // getting tjh first one
                    const char* expr3 = expr + strlen(iffunc);

                    if (!strcmp(expr3, "()"))
                    {
                        throw syntax_error_exception(ERRORCODE(22), "Possible error in statement: <b>%s</b> No parameters for a function (%s)", expr, iffunc);
                    }
                    if (strlen(expr3) == 0)
                    {
                        throw syntax_error_exception(ERRORCODE(23), "Possible error in statement: <b>%s</b> Meaningless use of a function (%s)", expr, iffunc);
                    }

                    // skipping the parentheses
                    if(*expr3 == '(')
                    {
                        expr3 ++;
                    }
                    [[maybe_unused]] std::string fnai;
                    std::string p1 = extract_proper_expression(expr3, fnai, {','});
                    std::string p2 =extract_proper_expression(expr3, fnai, {')'});

                    node->left = new tree;
                    node->left->parent = node;
                    doit(p1.c_str(), node->left, rp);

                    node->right = new tree;
                    node->right->parent = node;
                    doit(p2.c_str(), node->right, rp);
                }
                else
                {
                    node->right = nullptr;

                    const char* expr3 = expr + strlen(iffunc);

                    if (!strcmp(expr3, "()"))
                    {
                        throw syntax_error_exception(ERRORCODE(24), "Possible error in statement: <b>%s</b> No parameters for a function (%s)", expr, iffunc);
                    }
                    if (strlen(expr3) == 0)
                    {
                        throw syntax_error_exception(ERRORCODE(23), "Possible error in statement: <b>%s</b> Meaningless use of a function (%s)", expr, iffunc);
                    }
                    node->left = new tree;
                    node->left->parent = node;

                    if(expr3[0] == '^') // function on a power of something, such as: sin^2(x), the power goes in node->right
                    {
                        expr3++;
                        node->right= new tree;
                        node->right->parent = node;
                        [[maybe_unused]] std::string fnai;
                        std::string power = extract_proper_expression(expr3, fnai, {'('}, {}, false);
                        if(power.empty())
                        {
                            throw syntax_error_exception(ERRORCODE(26), "Possible error in statement: <b>%s</b> Improper power expression (%s)", expr, expr3);
                        }
                        doit(power.c_str(), node->right, rp);
                    }

                    doit(expr3, node->left, rp);
                }

            }
            else if (expr[strlen(expr) - 1] == ')')
            {
                throw syntax_error_exception(ERRORCODE(27), "Possible error in formula: <b>%s</b>. This does not look like a valid expression.", expr);
            }
            else
                if(strchr(expr, '[') && strchr (expr, ']')) // is this an indexed expression=, like a[2]
                {
                    [[maybe_unused]] std::string fnai;
                    auto indxd = extract_proper_expression(expr, fnai, {'['}); // consumes the [ too
                    auto indx = extract_proper_expression(expr, fnai, {']'});
                    node ->info = "[]";
                    node->left = new tree;
                    node->left->parent = node;
                    node->right = new tree;
                    node->right->parent = node;
                    doit(indxd.c_str(), node->left, rp);
                    doit(indx.c_str(), node->right, rp);
                }
                else
                {
                    // last resort: a power, which is not a function
                    int l0pwr = l0ops(expr, '^', '^', '^');
                    if(l0pwr != -1)
                    {
                        node->info = "^";
                        char* beforer2 = before(l0pwr, expr);
                        if (strlen(beforer2) == 0)
                        {
                            throw syntax_error_exception(ERRORCODE(28), "Possible error in statement:<b>%s</b>. No data before position %i (%c)", expr, zlop, expr[l0pwr]);
                        }
                        char* afterer2 = after(l0pwr, expr);
                        if (strlen(afterer2) == 0)
                        {
                            throw syntax_error_exception(ERRORCODE(29), "Possible error in statement: <b>%s</b>. No data after position %i (%c)", expr, zlop, expr[l0pwr]);
                        }
                        node->left = new tree;
                        node->left->parent = node;

                        doit(beforer2, node->left, rp);
                        delete[] beforer2;

                        node->right = new tree;
                        node->right->parent = node;
                        doit(afterer2, node->right, rp);
                        delete[] afterer2;
                    }
                    else
                    {
                        node->info = expr;
                        node->left = nullptr;
                        node->right = nullptr;
                    }
                }
        }
    }
}

int Function::l0cmp(const char *expr, std::string& zlop)
{
    static std::set<std::string> comp_operators = {"==", "!=", "<=", ">=", "<", ">"};
    unsigned int i = 0, level = 0;
    while (i < strlen(expr))
    {
        if (expr[i] == '(' || expr[i] == '[')
            level++;
        if (expr[i] == ')' || expr[i] == ']')
            level--;
        
        for(const std::string& op : comp_operators)
        {
                    
            if(expr[i] == op[0])
            {
                if(op.length() > 1)
                {
                    if(i < strlen(expr) - 1)
                    {
                        if(expr[i + 1] == op[1])
                        {
                            zlop = op;
                            return i;
                        }
                    }                    
                }
                else
                {
                    // if expr[i+1] == '=' then do not return yet
                    if(! (i < strlen(expr) - 1 &&  expr[i + 1] == '='))
                    {
                        zlop = op;
                        return i;
                    }
                }
            }
        }

        i++;
    }
    return -1;
}

int Function::l0logic(const char *expr, std::string & zlop, const std::string& r)
{
    std::set<std::string> logic_operators = {r};
    unsigned int i = 0, level = 0;

    while (i < strlen(expr))
    {
        if (expr[i] == '(' || expr[i] == '[')
            level++;
        if (expr[i] == ')' || expr[i] == ']')
            level--;

        for(const std::string& op : logic_operators)
        {

            if(expr[i] == op[0])
            {
                bool found = true;
                for(int j=1; j<op.length(); j++)
                {
                    if(op[j] != expr[i + j])
                    {
                        found = false;
                        break;
                    }
                }

                if(found)
                {
                    zlop = r;
                    return i;
                }
            }
        }

        i++;
    }
    return -1;
}


int Function::l0ops(const char* expr, char op1, char op2, char op3)
{
    unsigned int i = 0, level = 0;
    while (i < strlen(expr))
    {
        if (expr[i] == '(' || expr[i] == '[')
            level++;
        if (expr[i] == ')' || expr[i] == ']')
            level--;
        if ((expr[i] == op1 || expr[i] == op2 || (expr[i] == op3 && op3) ) && level == 0)
            return i;
        i++;
    }
    return -1;
}

int Function::l0add(const char* expr)
{
    return l0ops(expr, '-', '+');
}

int Function::l0mlt(const char* expr)
{
    return l0ops(expr, '*', '/', '%');
}

double Function::calc(tree* node, RuntimeProvider* rp, IndexedAccess*& ia, Assignment*& a)
{
    if (node->left)
    {
        // if this is a function on a power, node->right contains the power of it, but we don't want to go in there
        if (node->right)
        {

            auto it = std::find_if(supported_functions.begin(), supported_functions.end(), [node](fun_desc_solve fds){ return fds.name == node->info && fds.standalone_plottable;});
            if(it == supported_functions.end())
            {
                auto r = calc(node->right, rp, ia, a);
                if(node->info == "[]")
                {
                    // do this only if node->left->info is a point type variable

                    if(rp->typeOfVariable(node->left->info.c_str()) == Types::TYPE_LIST)
                    {

                        ia = new IndexedAccess;
                        ia->indexedVariable = QString::fromStdString(node->left->info);
                        ia->index = r;

                        return std::numeric_limits<double>::quiet_NaN();
                    }
                    else
                    {
                        return rp->getIndexedVariableValue(node->left->info.c_str(), r);
                    }
                }
                else
                {
                    auto l = calc(node->left, rp, ia, a);
                    return op(node->info, l, r, rp);
                }
            }
            else
            {
                double the_value = op(node->info, calc(node->left, rp, ia, a), 0, rp);
                // let's calculate the power amount of it
                double the_power = calc(node->right, rp, ia, a);
                return pow(the_value, the_power);
            }
        }
        else
        {
            // this is a plain old function calculation
            return op(node->info, calc(node->left, rp, ia, a), 0, rp);
        }
    }
    else
    {
        if (defd(node->info, rp, a))
        {
            return value(node->info, rp);
        }
        else
        {
            if(!a)
            {
                // see if node->info is a member access
                if(node->info.find('.') != std::string::npos)
                {
                    std::string obj = node->info.substr(0, node->info.find('.'));
                    std::string attr = node->info.substr(node->info.find('.') + 1);

                    return rp->value(obj, attr);
                }
                else
                {
                    // find the root of this:
                    tree* t = node;
                    while(t && t->parent) t = t->parent;
                    throw syntax_error_exception(ERRORCODE(30), "Possible error in <b>%s</b> statement: <b>%s</b> is not understood.", t->stmt->statement.toLocal8Bit().data(), node->info);
                }
            }
        }
    }
    return std::numeric_limits<double>::quiet_NaN();
}

void Function::free_tree(tree *node)
{
    if (node->left)
    {
        free_tree(node->left);
    }

    if(node->right)
    {
        free_tree(node->right);
    }

    delete node;
}

double Function::op(const std::string& s, double op1, double op2, RuntimeProvider* rp)
{
    auto it = std::find_if(supported_functions.begin(), supported_functions.end(), [s](fun_desc_solve fds){ return fds.name == s;});
    if(it != supported_functions.end())
    {
        return it->solver(op1, op2);
    }

    // let's see if rp has a function for this
    auto f = rp->getFunction(s.c_str());
    if(f)
    {
        auto pars = f->get_domain_variables();
        if(pars.size() == 1)
        {
            IndexedAccess* ia = nullptr; Assignment* a = nullptr;
            f->SetVariable(pars[0].c_str(), op1);
            auto fv = f->Calculate(rp, ia, a);
            return fv;
        }
    }

    return -1;
}

int Function::defd(const std::string& s, RuntimeProvider* rp, Assignment*& assig)
{
    if (vars.count(s))
    {
        return 1;
    }
    if (isnumber(s.c_str()))
    {
        return 1;
    }

    // if the assig is not nullptr after the call, this is a complex object, defined in the rp
    if(rp->defd(s, assig))
    {
        return 1;
    }

    return 0;
}

double Function::value(const std::string& s, RuntimeProvider* rp)
{
    if (isnumber(s.c_str()))
    {
        return fromString(s);
    }

    if (vars.count(s))
    {
        return vars[s];
    }

    Assignment* assig = nullptr;
    if(rp->defd(s, assig))
    {
        return rp->value(s);
    }

    return std::numeric_limits<double>::quiet_NaN();
}

bool Function::breakUpAsLogicOps(const char*expr, tree*node, const char *o, RuntimeProvider *rp)
{
    std::string szlLogic;
    int zlAnd = l0logic(expr, szlLogic, o);
    if(zlAnd != -1)
    {
        char* beforOp = before(zlAnd, expr);
        const char *afterOp = expr + zlAnd + szlLogic.length();

        while(isspace(*afterOp)) afterOp++;
        while(isspace(*(beforOp + strlen(beforOp) - 1))) *(beforOp + strlen(beforOp) - 1) = 0;

        node->info = szlLogic;
        node->left = new tree;
        node->left->parent = node;

        doit(beforOp, node->left, rp);

        node->right = new tree;
        node->right->parent = node;
        doit(afterOp, node->right, rp);

        return true;
    }
    return false;
}


QSharedPointer<Function> Function::temporaryFunction(const QString &definition, Statement* s)
{
    QString funString = QString::fromStdString(random_string(16)) +  "($) = " + definition;
    funString = funString.simplified();
    return QSharedPointer<Function>(new Function(funString.toLocal8Bit().data(), s));
}

QSharedPointer<Function> Function::temporaryFunction(const char *definition, Statement *s)
{
    return temporaryFunction(QString(definition), s);
}
