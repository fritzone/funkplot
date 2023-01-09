#include "Rotation.h"

#include <QtMath>

#include "PointArrayAssignment.h"
#include "PointsOfObjectAssignment.h"
#include "RuntimeProvider.h"

bool Rotation::execute(RuntimeProvider *rp)
{
    for(auto& adef : rp->getAssignments())
    {
        if(what == adef->varName || what + ":" == adef->varName)
        {
            auto pointDefinitionAssignment = rp->getAssignmentAs<PointArrayAssignment>(adef->varName);

            // first implemented rotation: a point of object
            if(pointDefinitionAssignment)
            {
                pointDefinitionAssignment->rotate(getRotationPoint(), getAngle());
            }
            else
            {
                // second rotation: list of points
                auto pointArrayAssignment = rp->getAssignmentAs<PointArrayAssignment>(adef->varName);
                if(pointArrayAssignment)
                {
                    pointArrayAssignment->rotate(getRotationPoint(), getAngle());
                }
                else
                {
                    // third option list of points of object
                    auto pointsOfObjectAssignment = rp->getAssignmentAs<PointsOfObjectAssignment>(adef->varName);
                    if(pointsOfObjectAssignment)
                    {
                        pointsOfObjectAssignment->rotate(getRotationPoint(), getAngle());
                    }
                }
            }

        }
    }
    return false;
}

double Rotation::getAngle() const
{
    IndexedAccess* ia = nullptr; Assignment* a = nullptr;
    double angle = degree->Calculate(RuntimeProvider::get(), ia, a);

    if(unit != "radians")
    {
        angle = qDegreesToRadians(angle);
    }

    return angle;
}

std::tuple<double, double> Rotation::getRotationPoint()
{
    IndexedAccess* ia = nullptr; Assignment* as = nullptr;

    auto rotfX = Function::temporaryFunction(aroundX, this);
    auto rotfY = Function::temporaryFunction(aroundY, this);

    double dRPx = rotfX->Calculate(RuntimeProvider::get(), ia, as);
    double dRPy = rotfY->Calculate(RuntimeProvider::get(), ia, as);

    return {dRPx, dRPy};
}

QVector<QSharedPointer<Statement> > Rotation::create(int ln, const QString &codeline, QStringList &codelines, StatementCallback cb, StatementReaderCallback srcb)
{
    QString r_decl = codeline.mid(Keywords::KW_ROTATE.length());
    consumeSpace(r_decl);

    QString rotate_what = getDelimitedId(r_decl);

    if(!RuntimeProvider::get()->hasVariable(rotate_what))
    {
        throw syntax_error_exception(ERRORCODE(44), "rotate statement tries to rotate undeclared variable");
    }

    QString with_keyword = getDelimitedId(r_decl);
    if(with_keyword != "with")
    {
        throw syntax_error_exception(ERRORCODE(45), "rotate statement does not contain <b>with</b> keyword");
    }

    [[maybe_unused]] QString fnai;

    QString rotation_amount = extract_proper_expression(r_decl, fnai, {' '}, QSet<QString>{QString("degrees"), QString("radians"), "around"} );
    QString rotation_unit = getDelimitedId(r_decl);

    QSharedPointer<Rotation> result;
    result.reset(new Rotation(ln, codeline));
    result->degree = Function::temporaryFunction(rotation_amount, result.get());
    result->what = rotate_what;

    if(rotation_unit != "degrees" && rotation_unit != "radians" && !rotation_unit.isEmpty())
    {
        throw syntax_error_exception(ERRORCODE(46), "Rotation unit must be either degree or radians (default)");
    }
    result->unit = rotation_unit;

    QString around_kw = getDelimitedId(r_decl);
    if(!around_kw.isEmpty() && around_kw != Keywords::KW_AROUND)
    {
        throw syntax_error_exception(ERRORCODE(47), "Unknown keyword in rotation: <b>%s</b>" , around_kw.toStdString().c_str());
    }
    QString nextWord = getDelimitedId(r_decl);
    if(nextWord == "point")
    {
        nextWord = getDelimitedId(r_decl);
        if(nextWord != "at")
        {
            throw syntax_error_exception(ERRORCODE(48), "Invalid reference: <b>%s</b> (missing <b><pre>at</pre></b> keyword)", codeline.toStdString().c_str());
        }

        if(!r_decl.isEmpty())
        {
            if(r_decl[0] == '(') // around a specific point
            {
                // skip paren
                r_decl = r_decl.mid(1);
                QString px = extract_proper_expression(r_decl, fnai, {','});
                QString py = extract_proper_expression(r_decl, fnai, {')'});

                result->aroundX = px;
                result->aroundY = py;
            }
        }
    }
    return handleStatementCallback(vectorize(result), cb);

}
