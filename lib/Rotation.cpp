#include "Rotation.h"

#include <QtMath>

#include "PointArrayAssignment.h"
#include "RuntimeProvider.h"

bool Rotation::execute(RuntimeProvider *rp)
{
    for(auto& adef : rp->getAssignments())
    {
        if(what == adef->varName || what + ":" == adef->varName)
        {
            // first implemented rotation: a point of object
            if(adef.dynamicCast<PointDefinitionAssignment>())
            {
                adef.dynamicCast<PointDefinitionAssignment>()->rotated = false;

                auto fcp = adef->fullCoordProvider(rp);
                if( std::get<0>(fcp) && std::get<1>(fcp) )
                {
                    IndexedAccess* ia = nullptr; Assignment* as = nullptr;
                    double x = std::get<0>(fcp)->Calculate(rp, ia, as);
                    double y = std::get<1>(fcp)->Calculate(rp, ia, as);

                    double a = degree->Calculate(rp, ia, as);

                    qDebug() << "Rotate:" << a << "degrees";

                    if(unit != "radians")
                    {
                        a = qDegreesToRadians(a);
                    }


                    auto rotfX = Function::temporaryFunction(aroundX, this);
                    auto rotfY = Function::temporaryFunction(aroundY, this);

                    double dRPx = rotfX->Calculate(rp, ia, as);
                    double dRPy = rotfY->Calculate(rp, ia, as);

                    auto p = rotatePoint(dRPx, dRPy, a, {x, y});

                    adef.dynamicCast<PointDefinitionAssignment>()->rotated_x = p.x();
                    adef.dynamicCast<PointDefinitionAssignment>()->rotated_y = p.y();
                    adef.dynamicCast<PointDefinitionAssignment>()->rotated = true;

                    return true;
                }
            }
            else
            {
                // second rotation: list of points
                auto pointArrayAssignment = rp->getAssignmentAs<PointArrayAssignment>(adef->varName);

                if(pointArrayAssignment)
                {
                    QVector<std::tuple<QSharedPointer<Function>,QSharedPointer<Function>>> rotatedPoints;

                    for(int idx_v = 0; idx_v < pointArrayAssignment->m_elements.size(); idx_v ++)
                    {
                        auto p = pointArrayAssignment->m_elements[idx_v];

                        IndexedAccess* ia = nullptr; Assignment* a = nullptr;
                        double x = std::get<0>(p)->Calculate(rp, ia, a);
                        double y = std::get<1>(p)->Calculate(rp, ia, a);

                        double angle = degree->Calculate(rp, ia, a);

                        qDebug() << "Rotate:" << a << "degrees";

                        if(unit != "radians")
                        {
                            angle = qDegreesToRadians(angle);
                        }


                        auto rotfX = Function::temporaryFunction(aroundX, this);
                        auto rotfY = Function::temporaryFunction(aroundY, this);

                        double dRPx = rotfX->Calculate(rp, ia, a);
                        double dRPy = rotfY->Calculate(rp, ia, a);

                        auto pRotated = rotatePoint(dRPx, dRPy, angle, {x, y});


                        auto fx = Function::temporaryFunction(QString::number(pRotated.x(), 'f', 6), this);
                        auto fy = Function::temporaryFunction(QString::number(pRotated.y(), 'f', 6), this);


                        rotatedPoints.append({fx, fy});

                    }

                    pointArrayAssignment->m_elements = rotatedPoints;

                    pointArrayAssignment->resolvePrecalculatedPointsForIndexedAccessWithList(nullptr, rp);
                    pointArrayAssignment->setPrecalculatedSetForce(true);
                }
            }

        }
    }
    return false;
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
