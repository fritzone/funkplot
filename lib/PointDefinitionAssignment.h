#ifndef POINTDEFINITIONASSIGNMENT_H
#define POINTDEFINITIONASSIGNMENT_H

#include "Assignment.h"

/**
 * @brief The PointDefinitionAssignment struct represents the assignment/definition to a point
 */
struct PointDefinitionAssignment : public Assignment
{
    explicit PointDefinitionAssignment(int ln, const QString& s) : Assignment(ln, s) {}

    bool execute(RuntimeProvider* rp) override;
    std::tuple<QSharedPointer<Function>, QSharedPointer<Function>> fullCoordProvider(RuntimeProvider* rp) override;
    QString toString() override;
    void rotate(std::tuple<double, double> rp, double angle);

public:

    QSharedPointer<Function> x;       // x position
    QSharedPointer<Function> y;       // y position

    bool rotated = false;

    double rotated_x = 0.0;
    double rotated_y = 0.0;
};


#endif // POINTDEFINITIONASSIGNMENT_H
