#include "ArithmeticAssignment.h"
#include "Function.h"
#include "RuntimeProvider.h"

#include <QDebug>

bool ArithmeticAssignment::execute(RuntimeProvider *rp)
{
    if(arithmetic)
    {
        try
        {
            IndexedAccess* ia_m = nullptr; Assignment* a = nullptr;
            double v = arithmetic->Calculate(rp, ia_m, a);

            // if this is an indexed something ...
            if(ia_m)
            {
                // same logic as in RuntimeProvider.h
                dealWithIndexedAssignmentToSomething(rp, ia_m);
            }
            else
            if(a) // an assignment?
            {
                QString at = rp->typeOfVariable(a->varName.toLocal8Bit().data());

                // assigning to a point
                if(at == "point")
                {
                    // rewriting the current assignment
                    auto fcp = a->fullCoordProvider(rp);
                    if( std::get<0>(fcp) && std::get<1>(fcp) )
                    {
                        IndexedAccess* ia = nullptr; Assignment* a = nullptr;
                        double x = std::get<0>(fcp)->Calculate(rp, ia, a);
                        double y = std::get<1>(fcp)->Calculate(rp, ia, a);

                        rp->addOrUpdatePointDefinitionAssignment(lineNumber, this, x, y, varName);
                    }
                }
                else
                {
                    // TODO: not a point
                }
            }
            else
            {
                rp->variables()[varName] = v;
            }
        }
        catch(...)
        {
            QString vt = rp->typeOfVariable(varName.toStdString().c_str());
            if(vt == "point" || vt == "point")
            {
                throw syntax_error_exception(ERRORCODE(65), "Cannot use the point type variable <b>%s</b> in this context: <b>%s</b>",  varName.toStdString().c_str(), statement.toStdString().c_str());
            }
            throw;
        }
    }

    return true;
}

QString ArithmeticAssignment::toString()
{
    return "";
}
