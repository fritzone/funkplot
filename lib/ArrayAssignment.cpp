#include "ArrayAssignment.h"
#include "Function.h"

QString ArrayAssignment::toString()
{
    QString result = "[";
    for(int i=0; i<m_elements.size(); i++)
    {
        double v = m_elements[i]->Calculate();
        QString r = QString::number(v, 'f', 6);
        result += r;
        if(i < m_elements.size() - 1)
        {
            result += ", ";
        }
    }
    result += "]";
    return result;

}
