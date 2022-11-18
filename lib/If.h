#ifndef IF_H
#define IF_H

#include "Function.h"
#include "Keywords.h"
#include "Statement.h"


/**
 * @brief The If struct is a conditional expression
 */
struct If : public Statement
{

    explicit If (int ln, const QString& s) : Statement(ln, s) {}

    bool execute(RuntimeProvider* rp) override;
    QString keyword() const override
    {
        return Keywords::KW_IF;
    }

    QVector<QSharedPointer<Statement>> ifBody;
    QVector<QSharedPointer<Statement>> elseBody;
    QSharedPointer<Function> expression;

};


#endif // IF_H
