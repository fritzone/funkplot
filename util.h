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

static char const* ifStringThenConvertToCharBuf(std::string const& cpp)
{
    return cpp.c_str();
}

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
	tree* left;
    tree* right;
    std::string info;
};

//return the substring of src before pos. pos is NOT included in the result
static char* before(int pos, const char *src)
{
    char *befs=new char[pos + 1];
	for(int i=0;i<pos;i++)
	{
		befs[i]=src[i];
	}
	befs[pos]=0;
	return befs;
}

//and after pos. The char at pos is not included
static char* after(int pos, const char *src)
{
char *afts=new char[strlen(src)-pos+1];
unsigned int i;
	for(i=0;i<strlen(src)-pos+1;afts[i++]=0);
	for(i=pos+1;i<strlen(src);i++)
	{
		afts[i-pos-1]=src[i];
	}
	return afts;
}

//returns true if c is an operator (+/-*)
static int isoperator(char c)
{
	return c=='+' || c=='-' || c== '/' || c=='*' || c=='.';
}

//returns true if c is a paranthesis
static int isparanthesis(char c)
{
	return c=='(' || c==')';
}

//returns true if s is a number
static int isnumber(const char *s)
{
    unsigned int i;
	for(i=0;i<strlen(s);i++)
	{
		if(!isdigit(s[i]) && !(s[i]=='.'))
		{
			return 0;
		}
	}
	return 1;
}

//transforms a character to a string
static std::string c2str(char c)
{
    std::string s;
    s+=c;
	return s;
}

//this checks if an expression is a function or not (ex: sin(x) is s function)
// kesobb atirni, hogy checkeljuk a sajat fuggvenyeinket is :))
static char* isfunc(const char *s)
{
    unsigned int i=0, sc2=0;
    char *s2=new char[strlen(s) + 1];
	while(i<strlen(s) && s[i]!='(')
	{
		s2[sc2++]=s[i++];
	}
    s2[sc2]=0;

    if( !strcmp(s2,"sin") || !strcmp(s2,"cos") || !strcmp(s2,"tan") || !strcmp(s2,"ctg") || !strcmp(s2, "exp") )
	{
        return s2;
	}
    delete[] s2;
    return NULL;
}

//this function is an ENUMERATOR
//it tells us, if the name of a hash_struct equals some other string
static int keyz(void * hashstructadr, void *param)
{
	if(!strcmp( ((hash_struct*)hashstructadr)->name, (char*)param) )
		return 0;
	else
		return 1;
	
}


// trim from start
static inline std::string &sltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
    return s;
}

// trim from end
static inline std::string &srtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
    return s;
}

// trim from both ends
static inline std::string strim(std::string &s)
{
    return sltrim(srtrim(s));
}

template <typename T> int sgn(T val) {
    return (T(0) < val) - (val < T(0));
}


#endif
