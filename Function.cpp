#include "Function.h"
#include "util.h"
#include "RuntimeProvider.h"
#include "CodeEngine.h"
#include <QDebug>
#include <math.h>
#include <string.h>
#include <memory>
#include <set>

Function::Function(const char *expr, Statement* s) : funBody(expr)
{
//    qDebug() << "Creating fun:" << expr;

//    QString as(expr);
//    if(as.indexOf('e') >= 0)
//    {
//        qDebug() << "Weird";
//    }

    const char* eqp_chr = strchr(expr, '=');
    if(!eqp_chr)
    {
        throw syntax_error_exception("Invalid function defintion: %s. Missing assignment in body.", expr);
    }
    int eqpos = (strchr(expr, '=') - expr);
    char* expr_p1 = before(eqpos, expr);
    char* ppar = strchr(expr_p1, '(');
    if(!ppar)
    {
        throw syntax_error_exception("Improper parametrization of the given function: %s", expr_p1);
    }
    expr_p1[ppar - expr_p1] = 0;
    m_name = expr_p1;
    m_name = strim(m_name);
    ppar++;

    ppar[strchr(ppar, ')') - ppar] = 0;
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

    doit(cexpr_p2, root);
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
//    qDebug() << "varn:" << varn.c_str() << "set to:" << valu;
}

double Function::Calculate(RuntimeProvider *rp, IndexedAccess*& ia, Assignment*& a)
{
    return calc(root, rp, ia, a);
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

const QString &Function::get_funBody() const
{
    return funBody;
}

std::string Function::preverify_formula(char* expr)
{
    std::string s;
    for (int i = 0; i < strlen(expr); i++) {
        if (expr[i] == ',' || expr[i] == '$' || isalnum(expr[i]) || isoperator(expr[i]) || isparanthesis(expr[i])) {
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
    }
    return s;
}

void Function::doit(const char* expr, tree* node)
{
    int zladd = l0add(expr), zlmlt = l0mlt(expr),
        zlop = zladd == -1 ? zlmlt == -1 ? -1 : zlmlt : zladd;

    if (zlop != -1) {
        char* beforer = before(zlop, expr);
        if (strlen(beforer) == 0) {
            throw syntax_error_exception("Possible error in statement: %s. No data before position %i (%c)", expr, zlop, expr[zlop]);
        }
        char* afterer = after(zlop, expr);
        if (strlen(afterer) == 0) {
            throw syntax_error_exception("Possible error in statement: %s. No data after position %i (%c)", expr, zlop, expr[zlop]);
        }

        node->info = c2str(expr[zlop]);
        node->left = new tree;
        node->left->parent = node;

        doit(beforer, node->left);
        delete[] beforer;

        node->right = new tree;
        node->right->parent = node;
        doit(afterer, node->right);
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
                    throw syntax_error_exception("Syntax error in statement: %s. Unmatched empty paranthesis", expr);
                }
                doit(expr2, node);
                delete[] expr2;
            }
            else
            {
                throw syntax_error_exception("Possible error in statement: %s. Not found enclosing paranthesis", expr);
            }

        }
        else
        {
            char* iffunc = isfunc(expr);
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
                        throw syntax_error_exception("Possible error in statement: %s No parameters for a function (%s)", expr, iffunc);
                    }
                    if (strlen(expr3) == 0)
                    {
                        throw syntax_error_exception("Possible error in statement: %s Meaningless use of a function (%s)", expr, iffunc);
                    }

                    // skipping the parentheses
                    if(*expr3 == '(')
                    {
                        expr3 ++;
                    }

                    std::string p1 = extract_proper_expression(expr3, {','});
                    std::string p2 =extract_proper_expression(expr3, {')'});

                    node->left = new tree;
                    node->left->parent = node;
                    doit(p1.c_str(), node->left);

                    node->right = new tree;
                    node->right->parent = node;
                    doit(p2.c_str(), node->right);
                }
                else
                {
                    node->right = nullptr;

                    const char* expr3 = expr + strlen(iffunc);

                    if (!strcmp(expr3, "()")) {
                        throw syntax_error_exception("Possible error in statement: %s No parameters for a function (%s)", expr, iffunc);
                    }
                    if (strlen(expr3) == 0) {
                        throw syntax_error_exception("Possible error in statement: %s Meaningless use of a function (%s)", expr, iffunc);
                    }
                    node->left = new tree;
                    node->left->parent = node;
                    doit(expr3, node->left);
                }

            }
            else if (expr[strlen(expr) - 1] == ')')
            {
                throw syntax_error_exception("Possible error in formula: %s No open paranthesis for a closed one.", expr);
            }
            else
            if(strchr(expr, '[') && strchr (expr, ']')) // is this an indexed expression=, like a[2]
            {
                auto indxd = extract_proper_expression(expr, {'['}); // consumes the [ too
                auto indx = extract_proper_expression(expr, {']'});
                node ->info = "[]";
                node->left = new tree;
                node->left->parent = node;
                node->right = new tree;
                node->right->parent = node;
                doit(indxd.c_str(), node->left);
                doit(indx.c_str(), node->right);
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
        if (node->right)
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
                    throw syntax_error_exception("Possible error in <b>%s</b> statement: <b>%s</b> is not understood.", t->stmt->statement.toLocal8Bit().data(), node->info);
                }
            }
        }
    }
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
//    qDebug() << "Getting value of:" << s.c_str();

    if (isnumber(s.c_str()))
    {
        return atof(s.c_str());
    }

    if (vars.count(s))
    {
//        qDebug() << "is:" << vars[s];
        return vars[s];
    }

    Assignment* assig = nullptr;
    if(rp->defd(s, assig))
    {
        return rp->value(s);
    }

    return std::numeric_limits<double>::quiet_NaN();
}


QSharedPointer<Function> temporaryFunction(const QString &definition, Statement* s)
{
    QString funString = QString::fromStdString(random_string(16)) +  "($) = " + definition;
    return QSharedPointer<Function>(new Function(funString.toLocal8Bit().data(), s));
}
