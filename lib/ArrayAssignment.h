#ifndef ARRAYASSIGNMENT_H
#define ARRAYASSIGNMENT_H

#include "Assignment.h"
#include "Function.h"

struct ArrayAssignment : public Assignment
{
    explicit ArrayAssignment(int ln, const QString& s) : Assignment(ln, s) {}

    QVector<QSharedPointer<Function>> m_elements;
    QString toString() override;

    bool execute(RuntimeProvider *rp) override
    {
        return true;
    }
};

#endif // ARRAYASSIGNMENT_H
