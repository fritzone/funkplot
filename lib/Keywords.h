#ifndef KEYWORDS_H
#define KEYWORDS_H

#include <QString>
#include <QVector>

#define KEYWORD(w) add_kw(w)
#define TYPE(w) add_type(w)
#define SETTARGET(w) add_settarget(w)
#define DEFINEDOMAIN(w) add_domain(w)
#define SCRIPTING_LANGUAGE(w) add_scripting_language(w)

namespace Keywords
{

static QVector<QString> all_kws;

static QString add_kw(QString nkw)
{
    all_kws.append(nkw);
    return nkw;
}

const QString KW_ROTATE = KEYWORD("rotate");        // rotate <scene|point|assignment_of_points|object> with X [degrees|radians] [around <point>]
const QString KW_FUNCTION = KEYWORD("function");    // function f(x) = x * 2 + 5
const QString KW_PLOT = KEYWORD("plot");            // plot f over (-2, 2) [continuous]
const QString KW_OVER = KEYWORD("over");            // plot f over (-2, 2) [continuous]
const QString KW_CONTINUOUS = KEYWORD("continuous");// plot f over (-2, 2) [continuous|step<cnt=0.01>]
const QString KW_LET = KEYWORD("let");              // let something = points of f over (-2, 2) [continuous|step<cnt|0.01>] or let something = point at x, y or let something = a + b + c
const QString KW_OF  = KEYWORD("of");               // let something = points of f over (-2, 2) [continuous|step<cnt|0.01>]
const QString KW_STEP = KEYWORD("step");            // plot f over (-2, 2) [continuous|step<cnt|0.01>]
const QString KW_COUNTS = KEYWORD("counts");        // plot f over (-2, 2) [continuous|step<cnt|0.01>|counts<X[points|segments]> ]
const QString KW_SET = KEYWORD("set");              // set color name, set color #RGB, set color #RRGGBB, set color #RRGGBBAA, set color R,G,B[,A], set color name,alpha, set palette <palette>
const QString KW_IN = KEYWORD("in");                // foreach p in something do ... done
const QString KW_DO = KEYWORD("do");                // foreach p in something do ... done
const QString KW_DONE = KEYWORD("done");            // foreach p in something do ... done
const QString KW_RANGE = KEYWORD("range");          // foreach p in range (0, 1) do ... done
const QString KW_ELSE = KEYWORD("else");            // foreach p in something do ... done
const QString KW_SEGMENTS = KEYWORD("segments");    // plot f over (-2, 2) [continuous|step<cnt|0.01>|counts<X[points|segments]> ]
const QString KW_POINTS = KEYWORD("points");        // plot f over (-2, 2) [continuous|step<cnt|0.01>|counts<X[points|segments]> ]
const QString KW_FOR = KEYWORD("for");              // for i = 1 to 256 step 1 do ... done
const QString KW_IF = KEYWORD("if");                // if x == 1 do ... else ... done
const QString KW_TO = KEYWORD("to");                // for i = 1 to 256 step 1 do ... done
const QString KW_INTO = KEYWORD("into");            // insert into listname numbers 1,2 at position 4
const QString KW_AROUND = KEYWORD("around");        // rotate <scene|point|assignment_of_points|object> with X [degrees|radians] [around <point>]
const QString KW_WITH = KEYWORD("with");            // rotate <scene|point|assignment_of_points|object> with X [degrees|radians] [around <point>]
const QString KW_VAR = KEYWORD("var");              // var i a b <numeric|point|array>[, bla bu b point]
const QString KW_BEGIN = KEYWORD("begin");          // python script begin ... end
const QString KW_END = KEYWORD("end");              // python script begin ... end
const QString KW_START = KEYWORD("start");
const QString KW_ZOOM = KEYWORD("zoom");
const QString KW_ROTATION = KEYWORD("rotation");
const QString KW_RUN = KEYWORD("run");              // python script run print(a)
const QString KW_PARAMETRIC = KEYWORD("parametric");// parametric function f(t) = { x = function(t), y = function(t) }
const QString KW_APPEND = KEYWORD("append");        // append to l values 1,2,3,4
const QString KW_SCRIPT = KEYWORD("script");        // python script begin ... end
const QString KW_POLAR = KEYWORD("polar");          // polar plot ...

static const QVector<QString> all()
{
    return all_kws;
}

};


namespace Types
{

static QVector<QString> all_supported_types;

static QString add_type(QString nt)
{
    all_supported_types.append(nt);
    return nt;
}

const QString TYPE_NUMBER = TYPE("number");
const QString TYPE_LIST = TYPE("list");
const QString TYPE_POINT = TYPE("point");
const QString TYPE_UNKNOWN = TYPE("unknown");

static const QVector<QString> all()
{
    return all_supported_types;
}

}

namespace Domains
{

static QVector<QString> all_supported_domains;

static QString add_domain(QString nt)
{
    all_supported_domains.append(nt);
    return nt;
}

const QString DOMAIN_NUMBERS = DEFINEDOMAIN("numbers");
const QString DOMAIN_POINTS = DEFINEDOMAIN("points");

static const QVector<QString> all()
{
    return all_supported_domains;
}

}

namespace SetTargets
{
static QVector<QString> all_supported_settargets;

static QString add_settarget(QString nt)
{
    all_supported_settargets.append(nt);
    return nt;
}

const QString TARGET_COLOR = SETTARGET("color");
const QString TARGET_COLOUR = SETTARGET("colour");
const QString TARGET_PALETTE = SETTARGET("palette");
const QString TARGET_COORDINATES = SETTARGET("coordinates");
const QString TARGET_PIXEL = SETTARGET("pixel");

const QString TARGET_ON = SETTARGET("on");
const QString TARGET_OFF = SETTARGET("off");

static const QVector<QString> all()
{
    return all_supported_settargets;
}
}


namespace ScriptingLangugages
{
static QVector<QString> all_supported_scripting_languages;

static QString add_scripting_language(QString sl)
{
    all_supported_scripting_languages.append(sl);
    return sl;
}

#ifdef ENABLE_PYTHON
const QString PYTHON = SCRIPTING_LANGUAGE("python");
#endif

static const QVector<QString> all()
{
    return all_supported_scripting_languages;
}
}


#endif // KEYWORDS_H
