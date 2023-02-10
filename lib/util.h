/*
 * The Header file UTIL.H contains several useful definitions
 */

#ifndef _UTIL_H_
#define _UTIL_H_

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <string>
#include <string>
#include <cctype>
#include <memory>
#include <algorithm>
#include <functional>
#include <map>
#include <vector>
#include <iostream>
#include <cstdio> // snprintf
#include <string>
#include <stdexcept> // runtime_error
#include <memory> // unique_ptr
#include <cmath>
#include <sstream>

#include <QSet>
#include <QChar>
#include <QPointF>
#include <set>

#if defined(_MSC_VER) && _MSC_VER < 1500 // VC++ 8.0 and below
#define snprintf _snprintf
#endif

#define _CRT_SECURE_NO_WARNINGS

struct Statement;
class RuntimeProvider;

struct fun_desc_solve
{
    std::string name;
    std::string desc;
    std::function<double(double,double)> solver;
    bool standalone_plottable;
    bool needs_highlight;
};


#define ERRORCODE(x) x

static const std::vector<fun_desc_solve> supported_functions
{
     {"==", "Equality of elements", [](double v, double v2) -> double { return std::fabs(v - v2) < 0.000001; }, false, false },
     {"<=", "Equality of elements", [](double v, double v2) -> double { return v >= v2; }, false, false },    
     {">=", "Equality of elements", [](double v, double v2) -> double { return v >= v2; }, false, false },    
     {"<", "Equality of elements", [](double v, double v2) -> double { return v < v2; }, false, false },    
     {">", "Equality of elements", [](double v, double v2) -> double { return v > v2; }, false, false },    
     {">=", "Equality of elements", [](double v, double v2) -> double { return v != v2; }, false, false },    

     // basic stuff
     {"^", "Power of an element", [](double v, double v2) -> double { return pow(v, v2); }, false, false },
     {"+", "Simple addition", [](double v, double v2) -> double { return v + v2; }, false, false },
     {"-", "Simple substraction", [](double v, double v2) -> double { return v - v2; }, false, false },
     {"*", "Simple multiplication", [](double v, double v2) -> double { return v * v2; }, false, false },
     {"%", "Simple modulo operator", [](double v, double v2) -> double { return  v2 == 0 ? std::numeric_limits<double>::quiet_NaN() : (static_cast<int>(v) % static_cast<int>(v2)); }, false, false },
     {"/", "Simple division", [](double v, double v2) -> double { return  v2 == 0 ? std::numeric_limits<double>::quiet_NaN() : v / v2; }, false, false },

     //trygonometry, simple
     {"sin", "The sine function", [](double v, double v2) -> double { return std::sin(v); }, true, true },
     {"cos", "The cosine function", [](double v, double v2) -> double { return std::cos(v); } , true, true},
     {"tan", "The tangent function", [](double v, double v2) -> double { return std::tan(v); } , true, true},
     {"cot", "The cotangent function", [](double v, double v2) -> double { return std::sin(v) != 0 ? std::cos(v) / std::sin(v) : std::numeric_limits<double>::quiet_NaN(); }, true, true },
     {"sec", "The secant function", [](double v, double v2) -> double { return std::cos(v) != 0 ? 1.0 / std::cos(v) : std::numeric_limits<double>::quiet_NaN(); } , true, true},
     {"cosec", "The cosecant function", [](double v, double v2) -> double { return std::sin(v)!= 0 ? 1.0 / std::sin(v) : std::numeric_limits<double>::quiet_NaN(); } , true, true},

     //trygonometry, arc*/inverse functions
     {"asin", "The arcsine function", [](double v, double v2) -> double { if(v >= -1 && v <= 1) return std::asin(v); return std::numeric_limits<double>::quiet_NaN();} , true, true},
     {"acos", "The arccosine function", [](double v, double v2) -> double { if(v >= -1 && v <= 1) return std::acos(v); return std::numeric_limits<double>::quiet_NaN();} , true, true},
     {"atan", "The arctangent function", [](double v, double v2) -> double { return std::atan(v); }, true, true },
     {"actg", "The arc-cotangent function", [](double v, double v2) -> double { if(std::tan(1.0/v) != 0) return 1 / std::tan(1.0/v); return std::numeric_limits<double>::quiet_NaN(); } , true, true},
     {"asec", "The arcsecant function", [](double v, double v2) -> double { return std::cos(1.0/v) != 0 ? 1.0 / std::cos(1.0/v) : std::numeric_limits<double>::quiet_NaN(); } , true, true},
     {"acosec", "The arc-cosecant function", [](double v, double v2) -> double { return std::sin(1.0/v)!= 0 ? 1.0 / std::sin(1.0/v) : std::numeric_limits<double>::quiet_NaN(); } , true, true},

     // Algebraic functions
     {"exp", "The exponential function", [](double v, double v2) -> double { return std::exp(v); } , true, true},
     {"log", "The common logarithmic function", [](double v, double v2) -> double { return std::log10(v); } , true, true},
     {"ln", "The logarithmic function", [](double v, double v2) -> double { return std::log(v); } , true, true},
     {"sqrt", "The square root function", [](double v, double v2) -> double { return std::sqrt(v); } , true, true},
     // {"pow", "The power function", [](double v, double v2) -> double { return std::pow(v, v2); } , false},

     // Some easy stuff
     {"inc", "Increment by one", [](double v, double v2) -> double { return v + 1; } , false, true},
     {"dec", "Decrement by one", [](double v, double v2) -> double { return v - 1; } , false, true},

     // logic stuff
     {"and", "Logical and", [](double v, double v2) -> double { return static_cast<double>(static_cast<bool>(v) && static_cast<bool>(v2)); } , false, true},
     {"or", "Logical or", [](double v, double v2) -> double { return static_cast<double>(static_cast<bool>(v) || static_cast<bool>(v2)); } , false, true},
     {"not", "Logical and", [](double v, double v2) -> double { return static_cast<double>(! static_cast<bool>(v)); } , false, true},
};

enum class random_string_class
{
    RSC_HEX     = 0,
    RSC_B64     = 1,
    RSC_FULL    = 2,
    RSC_ASC_DEC = 3,
    RSC_DEC     = 4,
    RSC_CHARS   = 5
};

// gives a rendom string
std::string random_string( size_t length, random_string_class cls = random_string_class::RSC_CHARS );

// converts the given hex char to a number
int hex2int(char ch);

// converts the given hex char to a number
int hex2int(QChar hexchar);


template <typename Arg, typename... Args>
void doPrint(std::ostream& out, Arg&& arg, Args&&... args)
{
    out << std::forward<Arg>(arg);
    ((out << ',' << std::forward<Args>(args)), ...);
}

namespace details
{
template <typename... Args>
std::unique_ptr<char[]> formatImplS(size_t bufSizeGuess, char const* formatCStr, Args&&... args)
{
    std::unique_ptr<char[]> buf(new char[bufSizeGuess]);

   // doPrint(std::cout, std::forward<Args>(args)...);

    size_t expandedStrLen = snprintf(buf.get(), bufSizeGuess, formatCStr, args...);

    if(expandedStrLen < bufSizeGuess)
    {
        return buf;
    }
    else
    if(expandedStrLen < std::numeric_limits<size_t>::max())
    {
        // buffer was too small, redo with the correct size
        return formatImplS(expandedStrLen+1, formatCStr, std::forward<Args>(args)...);
    }
    else
    {
        throw std::runtime_error("snprintf failed with return value: "+std::to_string(expandedStrLen));
    }
}

char const* ifStringThenConvertToCharBuf(std::string const& cpp);
const char* ifStringThenConvertToCharBuf(const QString &cpp);

template <typename T>
T ifStringThenConvertToCharBuf(T const& t)
{
    return t;
}
}

template <typename... Args>
std::string format(std::string const& formatString, Args&&... args)
{
    // unique_ptr<char[]> calls delete[] on destruction
    std::unique_ptr<char[]> chars = details::formatImplS(4096, formatString.c_str(),
                                    details::ifStringThenConvertToCharBuf(args)...);

    // string constructor copies the data
    return std::string(chars.get());
}

class syntax_error_exception : public std::exception
{
public:
    template<class ... Args>
    syntax_error_exception(int en, const char* fmt, Args& ...args) : errst(format(fmt, std::forward<Args>(args)...)), error_number(en)
    {
        //doPrint(std::cout, std::forward<Args>(args)...);
    }

    template<class ... Args>
    syntax_error_exception(int en, const char* fmt, const Args& ...args) : errst(format(fmt, std::forward<const Args>(args)...)), error_number(en)
    {
        //doPrint(std::cout, std::forward<const Args>(args)...);
    }

    syntax_error_exception(int en, const char* e) : errst(e), error_number(en)
    {
    }

    const char* what() const noexcept override
    {
        return errst.c_str();
    }

    constexpr int error_code() const noexcept
    {
        return error_number;
    }

private:
    std::string errst;
    int error_number;
};


////this is a hash structure. through it are mapped different values to different strings
//struct hash_struct
//{
//	char *name;
//	void *data;
//};

////this structure is a list. It contains a void pointer and a next.
////the void pointer can be anything, for ex a hash_struct
//struct list
//{
//	list *next;
//	void *inf;
//};

// this is an enumerator function
typedef int (*ENUMERATOR)(void *, void *);

//this tree holds the interpreted form of a function formula
// the info tag is either a number or an operator or a function name or a variable :((
struct tree
{
    tree* left = nullptr;
    tree* right = nullptr;
    tree* parent = nullptr;
    std::string info;
    Statement* stmt = nullptr;
};

//return the substring of src before pos. pos is NOT included in the result
char* before(int pos, const char *src);

//and after pos. The char at pos is not included
char* after(int pos, const char *src);

//returns true if c is an operator (+/-*)
int isoperator(char c);

//returns true if c is a parenthesis
int isparenthesis(char c);

//returns true if s is a number
int isnumber(const char *s);

//transforms a character to a string
std::string c2str(char c);

//this checks if an expression is a function or not (ex: sin(x) is s function)
char* isfunc(const char *s, RuntimeProvider *rp);

//this function is an ENUMERATOR
//it tells us, if the name of a hash_struct equals some other string
int keyz(void * hashstructadr, void *param);

bool islogicop(const std::string& p);

// trim from start
std::string &sltrim(std::string &s);

// trim from end
std::string &srtrim(std::string &s);

// trim from both ends
std::string strim(std::string &s);

template <typename T> int sgn(T val) {
    return (T(0) < val) - (val < T(0));
}

void consumeSpace(QString &s);
QString getDelimitedId(QString&, QSet<char>, char &delim);
QString getDelimitedId(QString&, QSet<char> = {' '});
std::string extract_proper_expression(const char *&p, std::string& fnai_word, std::set<char> seps, std::set<std::string> first_not_accepted_identifier = {}, bool till_end = false);
QString extract_proper_expression(QString &p, QString& sepWord, QSet<QChar> seps = {' '}, QSet<QString> fnai = {}, bool till_end = false);
QPointF rotatePoint(double cx, double cy, double angle, QPointF p);
QPointF rotatePoint(std::tuple<double, double>, double angle, QPointF p);

template<class T, class D>
std::string toString(const std::vector<T>& x, const D& delimiter)
{
    std::stringstream ss;
    ss.imbue(std::locale("C"));

    for(size_t i=0; i<x.size(); i++)
    {
        ss << x.at(i);
        if(i < x.size() - 1)
        {
            ss << delimiter;
        }
    }
    return ss.str();
}

template<class T>
std::string toString(const T& x)
{
    std::stringstream ss;
    ss.imbue(std::locale("C"));

    ss << x;
    return ss.str();
}

// returns the given double from the string, locale independent way, use . as decimal sep
double fromString(const std::string& s);

// splits the given string using the given separator
std::vector<std::string> split(const std::string& s, const std::string& delimiter, bool keepEmpty = false);

// expects: X:xxxx&Y:yyyyy&Z:zzzz then returns the appropriate value for the given key
std::string fromPythonDict(const std::string& ps, const std::string& key);

// normalizes the string, removes all strange characters and adds only mathematically correct ones
QString normalize(const QString&);

#endif
