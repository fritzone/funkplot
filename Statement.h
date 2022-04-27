#ifndef STATEMENT_H
#define STATEMENT_H

#include <QSharedPointer>
#include <QString>

class RuntimeProvider;

/**
 * @brief The Statement struct is the base structure for all the statements the
 * code interpreter can build and execute
 */
struct Statement
{
    explicit Statement(const QString& s) : statement(s) {}
    Statement() = delete;

    virtual ~Statement() = default;

    // executes this statement
    virtual bool execute(RuntimeProvider* mw) = 0;

    // return the keyword this statement represents
    virtual QString keyword() const = 0;

    QString statement;
    QSharedPointer<Statement> parent = nullptr;
    RuntimeProvider* runtimeProvider = nullptr; // this is the "scope" of the running piece of code
};


#endif // STATEMENT_H
