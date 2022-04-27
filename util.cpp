#include "util.h"

#include <vector>
#include <string>
#include <QString>

std::string random_string(size_t length, random_string_class cls)
{
    auto randchar = [cls]() -> char
    {
        auto charset = [cls]() -> std::string {
            switch (cls) {
            case random_string_class::RSC_DEC:
                return "0123456789";
            case random_string_class::RSC_CHARS:
                return "abcdfghijklmnopqrstuvwxyzABCDFGHIJKLMNOPQRSTUVWXYZ";
            case random_string_class::RSC_HEX:
                return "0123456789abcdef";
            case random_string_class::RSC_ASC_DEC:
                return "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
            case random_string_class::RSC_B64:
                return "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ+/";
            case random_string_class::RSC_FULL:
                return "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ|!#$%&/()=?{[]}+\\-_.:,;'*^";
            }
            return "10";
        }();

        const size_t max_index = (charset.length() - 1);
        return charset[ rand() % max_index ];
    };
    std::string str(length, 0);
    std::generate_n( str.begin(), length, randchar );
    return str;
}

int hex2int(char ch)
{
    if (ch >= '0' && ch <= '9')
        return ch - '0';
    if (ch >= 'A' && ch <= 'F')
        return ch - 'A' + 10;
    if (ch >= 'a' && ch <= 'f')
        return ch - 'a' + 10;
    return -1;
}

int hex2int(QChar hexchar)
{
    int v;
    if ( hexchar.isDigit() )
        v = hexchar.digitValue();
    else if ( hexchar >= 'A' && hexchar <= 'F' )
        v = hexchar.cell() - 'A' + 10;
    else if ( hexchar >= 'a' && hexchar <= 'f' )
        v = hexchar.cell() - 'a' + 10;
    else
        v = -1;
    return v;
}

const char *details::ifStringThenConvertToCharBuf(const std::string &cpp)
{
    return cpp.c_str();
}

char *before(int pos, const char *src)
{
    char *befs=new char[pos + 1];
    for(int i=0;i<pos;i++)
    {
        befs[i]=src[i];
    }
    befs[pos]=0;
    return befs;
}

char *after(int pos, const char *src)
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

int isoperator(char c)
{
    return c=='+' || c=='-' || c== '/' || c=='*' || c=='.' || c == '%';
}

int isparanthesis(char c)
{
    return c=='(' || c==')' || c == '[' || c == ']';
}

int isnumber(const char *s)
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

std::string c2str(char c)
{
    std::string s;
    s+=c;
    return s;
}

char *isfunc(const char *s)
{
    unsigned int i=0, sc2=0;
    char *s2=new char[strlen(s) + 1];
    while(i<strlen(s) && s[i]!='(')
    {
        s2[sc2++]=s[i++];
    }
    s2[sc2]=0;

    auto it = std::find_if(supported_functions.begin(), supported_functions.end(), [s2](fun_desc_solve fds){ return fds.name == s2;});
    if(it != supported_functions.end())
    {
        return s2;
    }
    delete[] s2;
    return nullptr;
}

int keyz(void *hashstructadr, void *param)
{
    if(!strcmp( ((hash_struct*)hashstructadr)->name, (char*)param) )
        return 0;
    else
        return 1;

}

std::string& sltrim(std::string& s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int c) {return !std::isspace(c); }));
    return s;
}

std::string& srtrim(std::string& s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](int c) {return !std::isspace(c); }).base(), s.end() );
    return s;
}

std::string strim(std::string &s)
{
    return sltrim(srtrim(s));
}

void consumeSpace(QString &s)
{
    while(!s.isEmpty() && s[0].isSpace())
    {
        s = s.mid(1);
    }
}

QString getDelimitedId(QString &s, QSet<char> delims, char& delim)
{
    QString result;
    consumeSpace(s);
    // name of the assigned variable
    while(!s.isEmpty() && !delims.contains(s[0].toLatin1()))
    {
        result += s[0];
        s = s.mid(1);
    }
    if (s.isEmpty())
    {
        delim = 0;
        return result;
    }
    delim = s[0].toLatin1();
    // skip the delimiter
    s = s.mid(1);
    // and the space folowing that
    consumeSpace(s);

    return result;
}

QString getDelimitedId(QString& s, QSet<char> delim)
{
    char d;
    return getDelimitedId(s, delim, d);
}

QPointF rotatePoint(float cx, float cy, float angle, QPointF p)
{
    float s = sin(angle);
    float c = cos(angle);

    // translate point back to origin:
    p.setX(p.x() - cx);
    p.setY(p.y() - cy);

    // rotate point
    float xnew = p.x() * c - p.y() * s;
    float ynew = p.x() * s + p.y() * c;

    // translate point back:
    p.setX( xnew + cx );
    p.setY( ynew + cy );
    return p;
}

std::string extract_proper_expression(const char *&p, std::set<char> seps, std::set<std::string> first_not_accepted_identifier)
{
    std::string res = "";
    bool done = false;
    int current_par_level = 1;
    std::string current_word = "";
    const auto first_p = p;

    while(!done)
    {
        if(*p == ' ') // see the current word
        {
            if(first_not_accepted_identifier.find(current_word) != first_not_accepted_identifier.end())
            {
                // shortcut return, go back to the caller we just parsed a word we were not supposed to
                p -= current_word.length();
                res = res.erase(res.length() - current_word.length());
                // remove the last space
                while(res.length() > 0 && res.at(res.length() - 1) == ' ')
                {
                    res = res.erase(res.length() - 1);
                    p --;
                }

                // looks sort of redundant ...
                while(*p && isspace(*p))
                {
                    p++;
                }
                return res;
            }
            else
            {
                current_word = "";
            }
        }

        bool added = false;
        if(*p == '(')
        {
            current_par_level ++;
            res += *p;
            if(*p != ' ') current_word += *p;

            added = true;
        }
        else
        if(*p == ')')
        {
            current_par_level--;
            if(current_par_level >= 1)
            {
                res += *p;
                if(*p != ' ') current_word += *p;

                added = true;
            }
        }


        // see if we can leave: no more parentheses and the current one is separator
        if(seps.count(*p) > 0 and current_par_level <= 1)
        {
            if(current_par_level == 0)
            {
                done = true;
            }

            // except if the current one is a closing parenthesys and the separator is also a parenthesis
            if(  !(*p == ')' && seps.count(*p) > 0))
            {
                done = true;
            }
        }
        else
        {
            if(!added)
            {
                res += *p;

                if(*p != ' ') current_word += *p;
            }
        }

        p++;

        if(!*p)
        {
            done = true;

            if(first_not_accepted_identifier.find(current_word) != first_not_accepted_identifier.end())
            {
                // shortcut return, go back to the caller we just parsed a word we were not supposed to
                p -= current_word.length();
                res = res.erase(res.length() - current_word.length());
                // remove the last space
                while(res.length() > 0 && res.at(res.length() - 1) == ' ')
                {
                    res = res.erase(res.length() - 1);
                    p --;
                }

                // looks sort of redundant ...
                while(*p && isspace(*p))
                {
                    p++;
                }
                return res;
            }

        }

    }

    // skip the whitespace
    while(*p && isspace(*p))
    {
        p++;
    }

    return res;
}

QString extract_proper_expression(QString &p, QSet<QChar> seps, QSet<QString> fnai)
{
    std::string s = p.toStdString();
    std::set<char> t;
    for(const QChar c : seps)
    {
        t.insert(c.toLatin1());
    }

    char* ss = (char*)calloc(s.length() + 1, sizeof(char));
    strncpy(ss, s.c_str(), s.length());
    const char* css = ss;

    std::string rs = extract_proper_expression(css, t);
    std::string ts(css);
    p = QString::fromStdString(ts);
    QString result = QString::fromStdString(rs);
    free(ss);
    return result;
}
