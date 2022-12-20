#include "ArrayAssignment.h"

QString ArrayAssignment::toString()
{
    QString result = "[";
    for(int i=0; i<m_elements.size(); i++)
    {
        IndexedAccess* ia = nullptr; Assignment* a = nullptr;
        double v = m_elements[i]->Calculate(runtimeProvider, ia, a);
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
