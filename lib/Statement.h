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
    // this will be called after each statement was succesfully parsed
    using StatementCallback = std::function<bool(QSharedPointer<Statement>)>;

    // this is used to read in more statements
    using StatementReaderCallback = std::function<QSharedPointer<Statement>(int&, QStringList&, QVector<QSharedPointer<Statement>> &, QSharedPointer<Statement>)>;

    Statement(int ln, const QString& s);
    Statement() = default;

    virtual ~Statement() = default;

    // executes this statement
    virtual bool execute(RuntimeProvider* mw)
    {
        return true;
    }

    static const QVector<QSharedPointer<Statement>>& handleStatementCallback(const QVector<QSharedPointer<Statement>>&, StatementCallback cb);

    static QVector<QSharedPointer<Statement>> vectorize(QSharedPointer<Statement>);

    // return the keyword this statement represents
    virtual QString keyword() const = 0;

    QString statement;
    QSharedPointer<Statement> parent = nullptr;
    RuntimeProvider* runtimeProvider = nullptr; // this is the "scope" of the running piece of code
    int lineNumber = -1; // at which line this statement resides
    bool needsRun = true;
};



#endif // STATEMENT_H
