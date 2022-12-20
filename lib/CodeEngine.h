#ifndef CODEENGINE_H
#define CODEENGINE_H

#include "IndexedAccess.h"
#include "Statement.h"
#include "Stepped.h"
#include "Assignment.h"

class RuntimeProvider;

// This class is responsible for parsing the code and generating the required structures for RuntimProvider
class CodeEngine
{
public:
    CodeEngine(QStringList codelines, RuntimeProvider* rp);
    bool parse();
    QSharedPointer<Statement> resolveCodeline(int& ln, QStringList& codelines, QVector<QSharedPointer<Statement>>& statements, QSharedPointer<Statement> parentScope);

    QVector<QSharedPointer<Statement> > createVariableDeclaration(int ln, const QString& codeline);
    void resolveOverKeyword(QString codeline, QSharedPointer<Stepped> stepped, Statement* s);
    void resolveCountsKeyword(QString codeline, QSharedPointer<Stepped> stepped, Statement* s);

#ifdef ENABLE_PYTHON
    QSharedPointer<Statement> createPythonSriptlet(int ln, const QString &codeline, QStringList &codelines);
#endif

    QSharedPointer<Statement> createAssignment(int ln, const QString &codeline);
    QSharedPointer<Statement> createFunction(int ln, const QString &codeline);
    QSharedPointer<Statement> createListInsert(int ln, const QString &codeline);
    QSharedPointer<Statement> createListAppend(int ln, const QString &codeline);
    QSharedPointer<Statement> createPlot(int ln, const QString& codeline);
    QSharedPointer<Statement> createSet(int ln, const QString &codeline);
    QSharedPointer<Statement> createLoop(int ln, const QString &codeline, QStringList &codelines);
    QSharedPointer<Statement> createIf(int ln, const QString &codeline, QStringList &codelines);
    QSharedPointer<Statement> createRotation(int ln, const QString &codeline, QStringList &codelines);

    QSharedPointer<Assignment> providePointsOfDefinition(int ln, const QString& codeline, QString assignment_body, const QString& varName, const QString& targetProperties);

private:

    QStringList m_codelines;
    RuntimeProvider* m_rp;
};

#endif // CODEENGINE_H
