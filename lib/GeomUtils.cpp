#include "GeomUtils.h"
#include "RuntimeProvider.h"
#include "LineAssignment.h"
#include "SegmentAssignment.h"

#include <cmath>

bool resolveObjectLine(RuntimeProvider* rp, const QString& name, QPointF& p1, QPointF& p2)
{
    auto a = rp->getAssignment(name);
    if (a) {
        if (auto la = qSharedPointerDynamicCast<LineAssignment>(a)) {
            if (la->x1 && la->y1 && la->x2 && la->y2) {
                p1 = {la->x1->Calculate(), la->y1->Calculate()};
                p2 = {la->x2->Calculate(), la->y2->Calculate()};
                return true;
            }
        }
        if (auto sa = qSharedPointerDynamicCast<SegmentAssignment>(a)) {
            if (sa->x1 && sa->y1 && sa->x2 && sa->y2) {
                p1 = {sa->x1->Calculate(), sa->y1->Calculate()};
                p2 = {sa->x2->Calculate(), sa->y2->Calculate()};
                return true;
            }
        }
        return false;
    }

    // Try compact two-point form "AB": letter+digits then letter+digits
    auto readLabel = [&](int pos) -> std::pair<QString,int> {
        if (pos >= name.length() || !name[pos].isLetter()) return {{}, pos};
        QString lbl; lbl += name[pos++];
        while (pos < name.length() && name[pos].isDigit()) lbl += name[pos++];
        return {lbl, pos};
    };
    int p = 0;
    auto [la, p1i] = readLabel(p); p = p1i;
    auto [lb, p2i] = readLabel(p); p = p2i;
    if (la.isEmpty() || lb.isEmpty() || p != name.length()) return false;

    auto aA = rp->getAssignment(la);
    auto aB = rp->getAssignment(lb);
    if (!aA || !aB) return false;
    auto [axf, ayf] = aA->fullCoordProvider(rp);
    auto [bxf, byf] = aB->fullCoordProvider(rp);
    if (!axf || !ayf || !bxf || !byf) return false;
    p1 = {axf->Calculate(), ayf->Calculate()};
    p2 = {bxf->Calculate(), byf->Calculate()};
    return true;
}

QPointF projectPointOntoLine(const QPointF& P, const QPointF& lineP1, const QPointF& lineP2)
{
    double dx = lineP2.x() - lineP1.x();
    double dy = lineP2.y() - lineP1.y();
    double len2 = dx * dx + dy * dy;
    if (len2 < 1e-12) return lineP1;
    double t = ((P.x() - lineP1.x()) * dx + (P.y() - lineP1.y()) * dy) / len2;
    return {lineP1.x() + t * dx, lineP1.y() + t * dy};
}
