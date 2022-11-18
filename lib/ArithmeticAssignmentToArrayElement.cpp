#include "ArithmeticAssignmentToArrayElement.h"

#include <QPointF>

#include "ArrayAssignment.h"
#include "Function.h"
#include "PointDefinitionAssignment.h"
#include "RuntimeProvider.h"

bool ArithmeticAssignmentToArrayElement::execute(RuntimeProvider *rp)
{

    if(arithmetic)
    {
        try
        {
            IndexedAccess* ia_m = nullptr; Assignment* a = nullptr;
            double v = arithmetic->Calculate(rp, ia_m, a);
            double idx_v = index->Calculate(rp, ia_m, a);

            if(idx_v < 0)
            {
                throw syntax_error_exception(ERRORCODE(56), "Invalid index (<b>%d</b>) specified for: <b>%s</b> in this context: <b>%s</b>.", static_cast<int>(idx_v), varName.toStdString().c_str(), statement.toStdString().c_str());
            }

            auto arrayAssignment = rp->getAssignmentAs<ArrayAssignment>(varName);
            //ArrayAssignment* arrayAssignment = dynamic_cast<ArrayAssignment*>(assignment);

            if(!arrayAssignment)
            {
                throw syntax_error_exception(ERRORCODE(57), "Cannot identify the object to assign to: <b>%s</b> in this context: <b>%s</b>", varName.toStdString().c_str(), statement.toStdString().c_str());
            }

            if(arrayAssignment->m_elements.size() > idx_v)
            {

                // are we trying to assign a variable to an array element
                if(a)
                {
                    if(rp->typeOfVariable(a->varName) != Types::TYPE_LIST)
                    {
                        if(rp->typeOfVariable(a->varName) != rp->domainOfVariable(varName))
                        {
                            throw syntax_error_exception(ERRORCODE(58), "Incompatible assignment, types don't match. Target <b>%s</b> (<b>%s</b>), source: <b>%s</b> (being <b>%s</b>), in this context: <b>%s</b>", varName.toStdString().c_str(), rp->domainOfVariable(varName).toStdString().c_str(), a->varName.toStdString().c_str(), rp->typeOfVariable(a->varName).toStdString().c_str(), statement.toStdString().c_str());
                        }

                        if(rp->domainOfVariable(varName) == Domains::DOMAIN_NUMBERS)
                        {
                            arrayAssignment->m_elements[idx_v] = Function::temporaryFunction(QString::number(rp->value(a->varName.toStdString()), 'f', 6), this);
                        }
                    }
                    else
                    {
                        // TODO: assign indexed to indexed
                    }
                }
                else // just normal assignment
                {
                    arrayAssignment->m_elements[idx_v] = Function::temporaryFunction(QString::number(v, 'f', 6), this);
                }
            }
            else
            {
                // fill up the array
                int avail = arrayAssignment->m_elements.size();
                for(int i = avail; i<idx_v; i++)
                {
                    arrayAssignment->m_elements.append(Function::temporaryFunction(QString::number(0, 'f', 6), this));
                }
                arrayAssignment->m_elements.append(Function::temporaryFunction(QString::number(v, 'f', 6), this));

            }

        }
        catch(...)
        {
            throw;
        }

    }

    return true;
}
