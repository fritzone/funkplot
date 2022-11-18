#ifndef POINTDEFINITIONASSIGNMENTTOOTHERPOINT_H
#define POINTDEFINITIONASSIGNMENTTOOTHERPOINT_H

#include "Function.h"
#include "PointDefinitionAssignment.h"

struct PointDefinitionAssignmentToOtherPoint : public PointDefinitionAssignment
{
    explicit PointDefinitionAssignmentToOtherPoint(int ln, const QString& s) : PointDefinitionAssignment(ln, s) {}
    std::tuple<QSharedPointer<Function>, QSharedPointer<Function> > fullCoordProvider(RuntimeProvider *rp) override;


    QString otherPoint;
    bool execute(RuntimeProvider* rp) override;

    template<class T>
    void copyFrom(T* same)
    {
        rotated = same->rotated;
        x = same->x;
        y = same->y;
        rotated_x = same->rotated_x;
        rotated_y = same->rotated_y;
    }


    template<class T>
    void resolveAssignmentFromIndexed(T* pas, RuntimeProvider* rp)
    {
        QString indexStr = otherPoint.mid(otherPoint.indexOf("[") + 1, otherPoint.indexOf("]") - otherPoint.indexOf("[") - 1);
        IndexedAccess* ia_m = nullptr; Assignment* a = nullptr;
        auto tempFun = Function::temporaryFunction(indexStr, this);
        double idx = tempFun->Calculate(rp, ia_m, a);
        int pps = pas->precalculatedPoints.size();
        if(idx < pps)
        {
            QPointF p = pas->precalculatedPoints[idx];
            x = Function::temporaryFunction(QString::number(p.x(), 'f', 6), this);
            y = Function::temporaryFunction(QString::number(p.y(), 'f', 6), this);
            rotated = false;
            rotated_x = p.x();
            rotated_y = p.y();
        }
        else
        {

            std::stringstream ss;
            ss <<"Index out of bounds for <b>" << pas->varName.toStdString().c_str() << "</b> in this context: <b>" <<  statement.toStdString().c_str() <<
                "</b>. Requested <b>" << idx << "</b>, available: <b>" << pps - 1 << "</b> (Arrays start from index 0, not 1)";
            throw syntax_error_exception(ERRORCODE(55), ss.str().c_str());
        }
    }
    
    void copyFrom();
};

#endif // POINTDEFINITIONASSIGNMENTTOOTHERPOINT_H
