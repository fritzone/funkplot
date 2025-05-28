#include "Stepped.h"
#include "Function.h"
#include "Keywords.h"

Stepped::Stepped() noexcept : step(Function::temporaryFunction("0.01", nullptr))
{

}

void Stepped::resolveOverKeyword(QString codeline, QSharedPointer<Stepped> stepped, Statement *s)
{
    codeline = codeline.mid(Keywords::KW_OVER.length());
    consumeSpace(codeline);
    char close_char = 0;
    if(!codeline.startsWith("(") || !codeline.startsWith("["))
    {
        if (!codeline.isEmpty())
        {
            close_char = codeline[0].toLatin1() == '(' ? ')' : ']';
            codeline = codeline.mid(1);
        }
    }

    if (close_char == 0)
    {
        return;
    }

    // first parameter
    char delim;
    QString first_par = getDelimitedId(codeline, {',', ' '}, delim);

    // skipping , or anything else
    while(!codeline.isEmpty() && (codeline.at(0) == ' ' || codeline.at(0) == ','))
    {
        codeline = codeline.mid(1);
    }
    QString second_par = getDelimitedId(codeline, {' ', close_char}, delim);
    // skipping , or anything else
    while(!codeline.isEmpty() && (codeline.at(0) == ' ' || codeline.at(0) == close_char) )
    {
        codeline = codeline.mid(1);
    }
    if(codeline.startsWith(Keywords::KW_CONTINUOUS))
    {
        stepped->continuous = true;
        codeline = codeline.mid(Keywords::KW_CONTINUOUS.length());
        consumeSpace(codeline);
    };

    if (codeline.startsWith(Keywords::KW_STEP))
    {
        codeline = codeline.mid(Keywords::KW_STEP.length());
        consumeSpace(codeline);
        stepped->stepValue = codeline;

        stepped->step = Function::temporaryFunction(codeline, s);
    }
    else
        if (codeline.startsWith(Keywords::KW_COUNTS))
        {
            resolveCountsKeyword(codeline, stepped, s);
        }
        else
            if(!codeline.isEmpty())
            {
                throw funkplot::syntax_error_exception(ERRORCODE(53), "Invalid keyword: <b>%s</b>", codeline.toStdString().c_str());
            }
    stepped->start = Function::temporaryFunction(first_par, s);
    stepped->end = Function::temporaryFunction(second_par, s);

}

void Stepped::resolveCountsKeyword(QString codeline, QSharedPointer<Stepped> stepped, Statement *s)
{
    codeline = codeline.mid(Keywords::KW_COUNTS.length());
    consumeSpace(codeline);
    QString strPointCount = getDelimitedId(codeline);
    stepped->stepValue = strPointCount;

    stepped->step = Function::temporaryFunction(strPointCount, s);
    stepped->counted = true;

    if(codeline == Keywords::KW_SEGMENTS)
    {
                stepped->step = Function::temporaryFunction(strPointCount + " + 1", s);
                stepped->continuous = true;
    }
}
