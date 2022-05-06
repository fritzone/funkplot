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

#include <cstdio> // snprintf
#include <string>
#include <stdexcept> // runtime_error
#include <memory> // unique_ptr
#include <cmath>

#include <QSet>
#include <QChar>
#include <QPointF>
#include <set>

class Statement;

struct fun_desc_solve
{
    std::string name;
    std::string desc;
    std::function<double(double,double)> solver;
    bool standalone_plottable;

};


static const std::vector<fun_desc_solve> supported_functions
{
     // basic stuff
     {"+", "Simple addition", [](double v, double v2) -> double { return v + v2; }, false },
     {"-", "Simple substraction", [](double v, double v2) -> double { return v - v2; }, false },
     {"*", "Simple multiplication", [](double v, double v2) -> double { return v * v2; }, false },
     {"%", "Simple modulo operator", [](double v, double v2) -> double { return  v2 == 0 ? std::numeric_limits<double>::quiet_NaN() : (static_cast<int>(v) % static_cast<int>(v2)); }, false },
     {"/", "Simple division", [](double v, double v2) -> double { return  v2 == 0 ? std::numeric_limits<double>::quiet_NaN() : v / v2; }, false },

     //trygonometry, simple
     {"sin", "The sine function", [](double v, double v2) -> double { return std::sin(v); }, true },
     {"cos", "The cosine function", [](double v, double v2) -> double { return std::cos(v); } , true},
     {"tan", "The tangent function", [](double v, double v2) -> double { return std::tan(v); } , true},
     {"cot", "The cotangent function", [](double v, double v2) -> double { return std::sin(v) != 0 ? std::cos(v) / std::sin(v) : std::numeric_limits<double>::quiet_NaN(); }, true },
     {"sec", "The secant function", [](double v, double v2) -> double { return std::cos(v) != 0 ? 1.0 / std::cos(v) : std::numeric_limits<double>::quiet_NaN(); } , true},
     {"cosec", "The cosecant function", [](double v, double v2) -> double { return std::sin(v)!= 0 ? 1.0 / std::sin(v) : std::numeric_limits<double>::quiet_NaN(); } , true},

     //trygonometry, arc*/inverse functions
     {"asin", "The arcsine function", [](double v, double v2) -> double { if(v >= -1 && v <= 1) return std::asin(v); return std::numeric_limits<double>::quiet_NaN();} , true},
     {"acos", "The arccosine function", [](double v, double v2) -> double { if(v >= -1 && v <= 1) return std::acos(v); return std::numeric_limits<double>::quiet_NaN();} , true},
     {"atan", "The arctangent function", [](double v, double v2) -> double { return std::atan(v); } },
     {"actg", "The arc-cotangent function", [](double v, double v2) -> double { if(std::tan(1.0/v) != 0) return 1 / std::tan(1.0/v); return std::numeric_limits<double>::quiet_NaN(); } , true},
     {"asec", "The arcsecant function", [](double v, double v2) -> double { return std::cos(1.0/v) != 0 ? 1.0 / std::cos(1.0/v) : std::numeric_limits<double>::quiet_NaN(); } , true},
     {"acosec", "The arc-cosecant function", [](double v, double v2) -> double { return std::sin(1.0/v)!= 0 ? 1.0 / std::sin(1.0/v) : std::numeric_limits<double>::quiet_NaN(); } , true},

     // Algebraic functions
     {"exp", "The exponential function", [](double v, double v2) -> double { return std::exp(v); } , true},
     {"log", "The common logarithmic function", [](double v, double v2) -> double { return std::log10(v); } , true},
     {"ln", "The logarithmic function", [](double v, double v2) -> double { return std::log(v); } , true},
     {"sqrt", "The square root function", [](double v, double v2) -> double { return std::sqrt(v); } , true},
     {"pow", "The power function", [](double v, double v2) -> double { return std::pow(v, v2); } , false},

     // Some easy stuff
     {"inc", "Increment by one", [](double v, double v2) -> double { return v + 1; } , false},
     {"dec", "Decrement by one", [](double v, double v2) -> double { return v - 1; } , false},

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

namespace details
{
template <typename... Args>
std::unique_ptr<char[]> formatImplS(
    size_t bufSizeGuess,
    char const* formatCStr,
    Args&&... args)
{
    std::unique_ptr<char[]> buf(new char[bufSizeGuess]);

    size_t expandedStrLen = std::snprintf(buf.get(), bufSizeGuess, formatCStr, args...);

    if (expandedStrLen >= 0 && expandedStrLen < bufSizeGuess)
    {
        return buf;
    } else if (expandedStrLen >= 0
               && expandedStrLen < std::numeric_limits<size_t>::max())
    {
        // buffer was too small, redo with the correct size
        return formatImplS(expandedStrLen+1, formatCStr, std::forward<Args>(args)...);
    } else {
        throw std::runtime_error("snprintf failed with return value: "+std::to_string(expandedStrLen));
    }
}

char const* ifStringThenConvertToCharBuf(std::string const& cpp);

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
    syntax_error_exception(Args& ...args) : errst(format(std::forward<Args>(args)...))
    {}

    template<class ... Args>
    syntax_error_exception(const Args& ...args) : errst(format(std::forward<const Args>(args)...))
    {}

    syntax_error_exception(const char* e) : errst(e)
    {}

    const char* what() const noexcept override
    {
        return errst.c_str();
    }

private:
    std::string errst;
};


//this is a hash structure. through it are mapped different values to different strings
struct hash_struct
{
	char *name;
	void *data;
};

//this structure is a list. It contains a void pointer and a next.
//the void pointer can be anything, for ex a hash_struct
struct list
{
	list *next;
	void *inf;
};

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

//returns true if c is a paranthesis
int isparanthesis(char c);

//returns true if s is a number
int isnumber(const char *s);

//transforms a character to a string
std::string c2str(char c);

//this checks if an expression is a function or not (ex: sin(x) is s function)
// kesobb atirni, hogy checkeljuk a sajat fuggvenyeinket is :))
char* isfunc(const char *s);

//this function is an ENUMERATOR
//it tells us, if the name of a hash_struct equals some other string
int keyz(void * hashstructadr, void *param);


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
std::string extract_proper_expression(const char *&p, std::set<char> seps, std::set<std::string> first_not_accepted_identifier = {}, bool till_end = false);
QString extract_proper_expression(QString &p, QSet<QChar> seps = {' '}, QSet<QString> fnai = {}, bool till_end = false);
QPointF rotatePoint(float cx, float cy, float angle, QPointF p);

#endif
