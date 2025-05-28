#include "Loop.h"
#include "RuntimeProvider.h"
#include "LoopTarget.h"
#include "RangeIteratorLoopTarget.h"
#include "constants.h"
#include "ArrayIteratorLoopTarget.h"
#include "ArithmeticAssignment.h"
#include "FunctionIteratorLoopTarget.h"
#include "Function.h"

bool Loop::execute(RuntimeProvider *rp)
{
    int ln = 0;
    auto looper = [&rp, &ln, this]()
    {
        try
        {
            for(const auto& stmt : qAsConst(body))
            {
                // qInfo() << "LOOP: Running:" << stmt->statement;
                if(!rp->isRunning())
                {
                    break;
                }

                ln = stmt->lineNumber;
                rp->setCurrentStatement(stmt->statement);
                stmt->execute(rp);
            }
        }
        catch(funkplot::syntax_error_exception& ex)
        {
            rp->reportError(ln, ex.error_code(), ex.what());
        }
        catch(std::exception& ex)
        {
            rp->reportError(ln, ERRORCODE(41), ex.what());
        }
    };

    try
    {
        rp->setCurrentStatement(this->statement);
        loop_target->loop(looper, rp);
    }
    catch(funkplot::syntax_error_exception& ex)
    {
        rp->reportError(ln, ex.error_code(), ex.what());
    }
    catch(std::exception& ex)
    {
        rp->reportError(lineNumber, ERRORCODE(41), ex.what());
    }

    return true;
}

void Loop::updateLoopVariable(double v)
{
    runtimeProvider->setValue(loop_variable, v);
}

void Loop::updateLoopVariable(QPointF v)
{
    runtimeProvider->setValue(loop_variable, v);
}

QVector<QSharedPointer<Statement> > Loop::create(int ln, const QString &codeline, QStringList &codelines, StatementCallback cb, StatementReaderCallback srcb)
{
    QString l_decl = codeline.mid(Keywords::KW_FOR.length());
    consumeSpace(l_decl);
    //    QVector<QSharedPointer<Statement>> loop_body;

    // the loop object, regardless if its an in loop or assignment loop
    QSharedPointer<Loop> result = nullptr;
    result.reset(new Loop(ln, codeline));

    // the loop variable
    [[maybe_unused]] QString fnai;
    QString loop_variable = extract_proper_expression(l_decl, fnai);

    if(!RuntimeProvider::get()->hasVariable(loop_variable))
    {
        throw funkplot::syntax_error_exception(ERRORCODE(37), "Undeclared variable <b>%s</b> cannot be used in for loop (%s)", loop_variable.toStdString().c_str(), codeline.toStdString().c_str());
    }

    result->loop_variable = loop_variable;

    consumeSpace(l_decl);

    // in keyword
    if(l_decl.startsWith(Keywords::KW_IN))
    {
        // skip it
        l_decl = l_decl.mid(Keywords::KW_IN.length());
        consumeSpace(l_decl);

        // see what are we looping through
        QString loop_over = extract_proper_expression(l_decl, fnai);

        if(loop_over == Keywords::KW_RANGE)
        {
            // see if it starts with parenthesis or not
            bool needs_closing_paren = false;
            if(!l_decl.isEmpty() && l_decl[0] == '(')
            {
                needs_closing_paren = true;
                // skip openin parenthesis
                l_decl = l_decl.mid(1);
                consumeSpace(l_decl);
            }

            auto ritl = new RangeIteratorLoopTarget(result);
            result->loop_target.reset(ritl);


            // get the range start functions
            QString range_start = extract_proper_expression(l_decl, fnai, {','});
            consumeSpace(l_decl);

            if(!l_decl.isEmpty() && l_decl[0] == ',')
            {
                l_decl = l_decl.mid(1);
                consumeSpace(l_decl);
            }

            ritl->startFun = Function::temporaryFunction(range_start, result.get());

            // range end
            QString range_end = extract_proper_expression(l_decl, fnai, (needs_closing_paren ? QSet<QChar>{QChar(')')} : QSet<QChar>{QChar(' ')}) );
            ritl->endFun = Function::temporaryFunction(range_end, result.get());

            if(!l_decl.isEmpty() && l_decl[0] == ')' && needs_closing_paren)
            {
                l_decl = l_decl.mid(1);
            }

            consumeSpace(l_decl);
            // do we have a step
            if(l_decl.startsWith(Keywords::KW_STEP))
            {
                l_decl = l_decl.mid(Keywords::KW_STEP.length());
                consumeSpace(l_decl);

                QString step = extract_proper_expression(l_decl, fnai);
                ritl->stepFun = Function::temporaryFunction(step, result.get());

                consumeSpace(l_decl);
            }
            else
            {
                ritl->stepFun = Function::temporaryFunction(DEFAULT_RANGE_STEP, result.get());
            }
        }
        else
            if(auto a = RuntimeProvider::get()->getAssignment(loop_over)) // are we looping thorugh an array?
            {
                QString vt = RuntimeProvider::get()->typeOfVariable(a->varName);
                if(vt == Types::TYPE_LIST)
                {
                    result->loop_target.reset(new ArrayIteratorLoopTarget(result));
                    result->loop_target->name = loop_over;

                    QString domainOfVar = RuntimeProvider::get()->domainOfVariable(a->varName);
                    if(domainOfVar == Domains::DOMAIN_POINTS)
                    {
                        // here add a temporary assignment for the loop_variable to point to the whatever it can
                        QSharedPointer<PointDefinitionAssignment> temp;
                        temp.reset(new PointDefinitionAssignment(ln, codeline));

                        temp->varName = loop_variable;
                        RuntimeProvider::get()->addOrUpdateAssignment(temp);
                    }
                    else
                    {
                        // here add a temporary assignment for the loop_variable to point to the whatever it can
                        QSharedPointer<ArithmeticAssignment> temp;
                        temp.reset(new ArithmeticAssignment(ln, codeline));

                        temp->varName = loop_variable;
                        RuntimeProvider::get()->addOrUpdateAssignment(temp);
                    }
                }
            }
            else
            {
                result->loop_target.reset(new FunctionIteratorLoopTarget(result));
                result->loop_target->name = loop_over;
            }
    }
    else
        if(l_decl.startsWith("=")) // the classical loop: for i = 0 to 256 step 1 do
        {
            [[maybe_unused]] QString fnai;

            // skip the equal sign
            l_decl = l_decl.mid(1);
            consumeSpace(l_decl);

            // the loop target of this is a range iterator, since this loop is a stepped range
            auto ritl = new RangeIteratorLoopTarget(result);
            result->loop_target.reset(ritl);

            // the start:
            QString range_start = extract_proper_expression(l_decl, fnai);
            ritl->startFun = Function::temporaryFunction(range_start, result.get());

            if(!l_decl.startsWith(Keywords::KW_TO))
            {
                throw funkplot::syntax_error_exception(ERRORCODE(38), "for statement with direct assignment does not contain the to keyword");
            }

            // create an assignment
            QSharedPointer<ArithmeticAssignment> temp;
            temp.reset(new ArithmeticAssignment(ln, codeline));
            temp->arithmetic = ritl->startFun;

            temp->varName = loop_variable;
            RuntimeProvider::get()->addOrUpdateAssignment(temp);

            l_decl = l_decl.mid(Keywords::KW_TO.length());
            consumeSpace(l_decl);

            // range end
            QString range_end = extract_proper_expression(l_decl, fnai);
            ritl->endFun = Function::temporaryFunction(range_end, result.get());

            consumeSpace(l_decl);
            // do we have a step
            if(l_decl.startsWith(Keywords::KW_STEP))
            {
                l_decl = l_decl.mid(Keywords::KW_STEP.length());
                consumeSpace(l_decl);

                QString step = extract_proper_expression(l_decl, fnai);
                ritl->stepFun = Function::temporaryFunction(step, result.get());

                consumeSpace(l_decl);
            }
            else
            {
                ritl->stepFun = Function::temporaryFunction(DEFAULT_FOR_STEP, result.get());
            }
        }
        else
        {
            throw funkplot::syntax_error_exception(ERRORCODE(39), "invalid for statement");
        }

    if(!l_decl.startsWith(Keywords::KW_DO))
    {
        throw funkplot::syntax_error_exception(ERRORCODE(40), "for statement does not contain the do keyword");
    }

    bool done = false;
    while(!codelines.isEmpty() && !done)
    {
        QSharedPointer<Statement> st = nullptr;
        QString at0 = codelines.at(0).simplified();
        qDebug() << "try" << at0;

        if(at0.isEmpty() || at0[0] == '#')
        {
            codelines.pop_front();
        }
        else
        {
            try
            {
                st = srcb(ln, codelines, result->body, result);
            }
            catch(funkplot::syntax_error_exception& a)
            {
                RuntimeProvider::get()->reportError(ln, a.error_code(), a.what());
                throw;
            }

            if(st)
            {
                done = st->keyword() == Keywords::KW_DONE;
            }
            else
            {
                throw funkplot::syntax_error_exception(ERRORCODE(42), "something is wrong with this expression: %s", codeline.toStdString().c_str());
            }
        }
    }

    if(!done)
    {
        throw funkplot::syntax_error_exception(ERRORCODE(43), "for body does not end with done");
    }

    return handleStatementCallback(vectorize(result), cb);

}
