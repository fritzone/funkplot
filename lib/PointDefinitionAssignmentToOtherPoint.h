#ifndef POINTDEFINITIONASSIGNMENTTOOTHERPOINT_H
#define POINTDEFINITIONASSIGNMENTTOOTHERPOINT_H

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


    void resolveAssignmentFromIndexed(Assignment* pas, RuntimeProvider* rp);
    
    void copyFrom();
};

#endif // POINTDEFINITIONASSIGNMENTTOOTHERPOINT_H
