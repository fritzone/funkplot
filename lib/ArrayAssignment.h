#ifndef ARRAYASSIGNMENT_H
#define ARRAYASSIGNMENT_H

#include "Assignment.h"
#include "Function.h"

struct ArrayAssignment : public Assignment
{
    explicit ArrayAssignment(int ln, const QString& s) : Assignment(ln, s) {}

    QVector<QSharedPointer<Function>> m_elements;
    QString toString() override
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

    bool execute(RuntimeProvider *rp) override
    {
        return true;
    }
};

#endif // ARRAYASSIGNMENT_H
