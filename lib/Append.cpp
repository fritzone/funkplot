#include "Append.h"
#include "ArrayAssignment.h"
#include "PointArrayAssignment.h"
#include "RuntimeProvider.h"

bool Append::execute(RuntimeProvider* rp)
{
    if(domain == Domains::DOMAIN_NUMBERS)
    {
        auto arrayAssignment = rp->getAssignmentAs<ArrayAssignment>(varName);

        if(arrayAssignment)
        {
            for(int idx_v = 0; idx_v < numberValues.size(); idx_v ++)
            {
                arrayAssignment->m_elements.append(Function::temporaryFunction(numberValues[idx_v].toStdString(), this));
            }
        }
    }
    else
    if(domain == Domains::DOMAIN_POINTS)
    {
        auto pointArrayAssignment = rp->getAssignmentAs<PointArrayAssignment>(varName);

        if(pointArrayAssignment)
        {
            for(int idx_v = 0; idx_v < pointValues.size(); idx_v ++)
            {
                pointArrayAssignment->m_elements.append(pointValues[idx_v]);
            }
        }
    }
    return true;
}
