#ifndef STATEMENTHANDLER_H
#define STATEMENTHANDLER_H

#include "Statement.h"

#include <QStringList>
#include <QDebug>
#include <functional>

// The generic signature of a method that a statement handler must implement under the name "create". That method should be public and static.
using HandlerCallback = std::function<QVector<QSharedPointer<Statement>>(int, const QString&, QStringList&, Statement::StatementCallback, Statement::StatementReaderCallback)>;

/**
 * @brief The AH class is the Abstract Handler of a statement
 */
struct AH
{
    AH() = default;


    /**
     * @brief execute is the method that is called for each registered handler if they accept a codeline
     * @param ln - the line number
     * @param codeline - the actual line of code that needs to be handled
     * @param codelines - the block of lines this takes part from
     * @param cb - the callback that each statement will be passed to upon successful completion
     * @return a vector of statements as read from codelines
     */
    virtual QVector<QSharedPointer<Statement>> execute(int ln, const QString &codeline, QStringList& codelines, Statement::StatementCallback cb, Statement::StatementReaderCallback) = 0;

    /**
     * @brief accepts tells us if the handler accepts the given codeline or not
     * @param codeline the codeline to accept
     * @return true/false
     */
    virtual bool accepts(const QString& codeline) = 0;
};


/**
 * @brief The Handler class is the base class for the handlers of keyword based statements
 */
template<class H> struct Handler : public H, public AH
{
    Handler() = default;
    HandlerCallback hc = H::create;

    QVector<QSharedPointer<Statement>> execute(int ln, const QString &codeline, QStringList& codelines, Statement::StatementCallback cb, Statement::StatementReaderCallback srcb) override
    {
        return hc(ln, codeline, codelines, cb, srcb);
    }

    bool accepts(const QString& codeline) override
    {
        return codeline.startsWith(H::kw());
    }

};

/**
 * @brief The HandlerStore class will keep all the handlers in a good defined place of their own
 */
struct HandlerStore
{
    inline static std::vector<std::shared_ptr<AH>> handlerClasses;

    static HandlerStore& instance()
    {
        static HandlerStore i;
        return i;
    }

    std::shared_ptr<AH> getHandler(const QString& s);
};

// Register the given class as a statement handler
#define REGISTER_STATEMENTHANDLER(v) \
namespace ns##v { \
struct Hh##v : public Handler<v> \
{\
    static bool create() {\
        HandlerStore::instance().handlerClasses.push_back( std::make_shared<Hh##v>()); \
        qInfo() << "Registering:" << #v; \
        return true; \
    }\
};\
}

/**
 * @brief registerClasses Someone needs to call this method in their main function, after a valid RProvider has been created
 */
void registerClasses();

#endif // STATEMENTHANDLER_H
