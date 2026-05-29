#include "Draw.h"
#include "RuntimeProvider.h"
#include "util.h"
#include "LineAssignment.h"
#include "SegmentAssignment.h"
#include <QColor>

#define _USE_MATH_DEFINES
#include <cmath>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

Draw::Draw(int ln, const QString &s) : Statement(ln, s)
{}

// Split "ABC", "A1BC2", etc. into up to three labels of the form [A-Za-z][0-9]*.
// Returns however many labels were found (0–3).
static int splitAngleLabels(const QString& s, QString& l1, QString& l2, QString& l3)
{
    auto readOne = [&](int pos) -> std::pair<QString, int> {
        if (pos >= s.length() || !s[pos].isLetter()) return {{}, pos};
        QString lbl;
        lbl += s[pos++];
        while (pos < s.length() && s[pos].isDigit())
            lbl += s[pos++];
        return {lbl, pos};
    };
    int p = 0;
    auto [a, p1] = readOne(p);  l1 = a; p = p1;
    auto [b, p2] = readOne(p);  l2 = b; p = p2;
    auto [c, p3] = readOne(p);  l3 = c;
    return (!l1.isEmpty() ? 1 : 0) + (!l2.isEmpty() ? 1 : 0) + (!l3.isEmpty() ? 1 : 0);
}

QString Draw::keyword() const
{
    return kw();
}

static void parseEndpoint(QString& body, QSharedPointer<Function>& x, QSharedPointer<Function>& y, Statement* s)
{
    consumeSpace(body);
    if (body.startsWith("(")) {
        body = body.mid(1);
        QString fnai;
        QString sx = extract_proper_expression(body, fnai, {','});
        QString sy = extract_proper_expression(body, fnai, {')'});
        x = Function::temporaryFunction(sx, s);
        y = Function::temporaryFunction(sy, s);
    } else {
        QString fnai;
        QString pointExpr = extract_proper_expression(body, fnai, {' '}, {Keywords::KW_TO, Keywords::KW_COUNTS, Keywords::KW_OVER}, false);
        x = Function::temporaryFunction(pointExpr + ".x", s);
        y = Function::temporaryFunction(pointExpr + ".y", s);
    }
}

QVector<QSharedPointer<Statement>> Draw::create(int ln, const QString &codeline, QStringList &codelines, StatementCallback cb, StatementReaderCallback srcb)
{
    QVector<QSharedPointer<Statement>> result;
    QString body = codeline.mid(Keywords::KW_DRAW.length());
    consumeSpace(body);

    QSharedPointer<Draw> drawStmt(new Draw(ln, codeline));

    if (body.startsWith(Keywords::KW_LINE) || body.startsWith(Keywords::KW_SEGMENT)) {
        drawStmt->drawType = body.startsWith(Keywords::KW_LINE) ? Line : Segment;
        body = body.mid(body.startsWith(Keywords::KW_LINE) ? Keywords::KW_LINE.length() : Keywords::KW_SEGMENT.length());
        consumeSpace(body);

        if (body.startsWith(Keywords::KW_FROM)) {
            body = body.mid(Keywords::KW_FROM.length());
            consumeSpace(body);
            parseEndpoint(body, drawStmt->x1, drawStmt->y1, drawStmt.data());
            consumeSpace(body);
            if (body.startsWith(Keywords::KW_TO)) {
                body = body.mid(Keywords::KW_TO.length());
                consumeSpace(body);
            }
            parseEndpoint(body, drawStmt->x2, drawStmt->y2, drawStmt.data());
        } else {
            QString tok = getDelimitedId(body);
            QString la, lb, lc;
            // Compact two-point form "AB": two adjacent point names, no explicit segment variable
            if (splitAngleLabels(tok, la, lb, lc) == 2
                && RuntimeProvider::get()->typeOfVariable(tok) != Types::TYPE_SEGMENT
                && RuntimeProvider::get()->typeOfVariable(la) == Types::TYPE_POINT
                && RuntimeProvider::get()->typeOfVariable(lb) == Types::TYPE_POINT)
            {
                drawStmt->x1 = Function::temporaryFunction(la + ".x", drawStmt.data());
                drawStmt->y1 = Function::temporaryFunction(la + ".y", drawStmt.data());
                drawStmt->x2 = Function::temporaryFunction(lb + ".x", drawStmt.data());
                drawStmt->y2 = Function::temporaryFunction(lb + ".y", drawStmt.data());
            } else {
                drawStmt->targetVar = tok;
            }
        }

        consumeSpace(body);
        if (body.startsWith(Keywords::KW_COUNTS)) {
            body = body.mid(Keywords::KW_COUNTS.length());
            consumeSpace(body);
            QString fnai;
            QString scount = extract_proper_expression(body, fnai, {' '}, {}, false);
            drawStmt->count = Function::temporaryFunction(scount, drawStmt.data());
        }

        consumeSpace(body);
        if (body.startsWith(Keywords::KW_TICKS)) {
            body = body.mid(Keywords::KW_TICKS.length());
            consumeSpace(body);
            QString fnai;
            QString st = extract_proper_expression(body, fnai, {' '}, {}, false);
            drawStmt->ticks = qRound(Function::temporaryFunction(st, drawStmt.data())->Calculate());
        }

        // Optional color/double modifiers for tick marks (same syntax as angle arcs)
        consumeSpace(body);
        while (!body.isEmpty()) {
            consumeSpace(body);
            if (body.isEmpty()) break;
            QString snapshot = body;
            QString token = getDelimitedId(body);
            if (token.toLower() == "double") {
                drawStmt->doubleLine = true;
            } else if (token.toLower() == "single") {
                drawStmt->doubleLine = false;
            } else if (QColor(token).isValid()) {
                drawStmt->drawColor = token;
            } else {
                body = snapshot;
                break;
            }
            consumeSpace(body);
        }

        result.append(drawStmt);
    } else if (body.startsWith(Keywords::KW_POINTS) || body.startsWith("point")) {
        drawStmt->drawType = Point;
        if (body.startsWith(Keywords::KW_POINTS)) {
            body = body.mid(Keywords::KW_POINTS.length());
        } else {
            body = body.mid(QString("point").length());
        }
        consumeSpace(body);

        if (body.startsWith(Keywords::KW_AT)) {
            body = body.mid(Keywords::KW_AT.length());
            consumeSpace(body);
        }

        parseEndpoint(body, drawStmt->x1, drawStmt->y1, drawStmt.data());

        consumeSpace(body);
        if (body.startsWith(Keywords::KW_AS)) {
            body = body.mid(Keywords::KW_AS.length());
            consumeSpace(body);
            if (!body.isEmpty() && body[0] == '"') {
                body = body.mid(1);
                int closeQuote = body.indexOf('"');
                if (closeQuote >= 0) {
                    drawStmt->pointLabel = body.left(closeQuote);
                    body = body.mid(closeQuote + 1);
                }
            }
        }

        result.append(drawStmt);
    } else if (body.startsWith("angle")) {
        drawStmt->drawType = Angle;
        body = body.mid(QString("angle").length());
        consumeSpace(body);

        if (body.startsWith(Keywords::KW_BETWEEN)) {
            // Form: draw angle between <seg1> and <seg2> [arcs N] [as "ABC"]
            body = body.mid(Keywords::KW_BETWEEN.length());
            consumeSpace(body);
            drawStmt->targetVar = getDelimitedId(body);
            consumeSpace(body);
            if (body.startsWith(Keywords::KW_AND)) {
                body = body.mid(Keywords::KW_AND.length());
                consumeSpace(body);
            }
            drawStmt->targetVar2 = getDelimitedId(body);
        } else if (body.startsWith(Keywords::KW_AT)) {
            // Form: draw angle at <vertex> between <arm1> and <arm2> — explicit vertex
            body = body.mid(Keywords::KW_AT.length());
            consumeSpace(body);
            parseEndpoint(body, drawStmt->x1, drawStmt->y1, drawStmt.data()); // vertex
            consumeSpace(body);
            if (body.startsWith(Keywords::KW_BETWEEN)) {
                body = body.mid(Keywords::KW_BETWEEN.length());
                consumeSpace(body);
            }
            parseEndpoint(body, drawStmt->x2, drawStmt->y2, drawStmt.data()); // arm1
            consumeSpace(body);
            if (body.startsWith(Keywords::KW_AND)) {
                body = body.mid(Keywords::KW_AND.length());
                consumeSpace(body);
            }
            parseEndpoint(body, drawStmt->x3, drawStmt->y3, drawStmt.data()); // arm2
        } else {
            // Three-point forms using ∠ABC convention: middle point is vertex.
            // Handles both compact "ABC" and space-separated "A B C".
            consumeSpace(body);
            QString tok1 = getDelimitedId(body);
            QString la, lb, lc;
            if (splitAngleLabels(tok1, la, lb, lc) == 3) {
                // Compact "ABC": la=arm1, lb=vertex, lc=arm2
                drawStmt->x1 = Function::temporaryFunction(lb + ".x", drawStmt.data()); // vertex
                drawStmt->y1 = Function::temporaryFunction(lb + ".y", drawStmt.data());
                drawStmt->x2 = Function::temporaryFunction(la + ".x", drawStmt.data()); // arm1
                drawStmt->y2 = Function::temporaryFunction(la + ".y", drawStmt.data());
                drawStmt->x3 = Function::temporaryFunction(lc + ".x", drawStmt.data()); // arm2
                drawStmt->y3 = Function::temporaryFunction(lc + ".y", drawStmt.data());
            } else {
                // Space-separated "A B C": tok1=arm1, tok2=vertex, tok3=arm2
                consumeSpace(body);
                QString tok2 = getDelimitedId(body);
                consumeSpace(body);
                QString tok3 = getDelimitedId(body);
                drawStmt->x1 = Function::temporaryFunction(tok2 + ".x", drawStmt.data()); // vertex
                drawStmt->y1 = Function::temporaryFunction(tok2 + ".y", drawStmt.data());
                drawStmt->x2 = Function::temporaryFunction(tok1 + ".x", drawStmt.data()); // arm1
                drawStmt->y2 = Function::temporaryFunction(tok1 + ".y", drawStmt.data());
                drawStmt->x3 = Function::temporaryFunction(tok3 + ".x", drawStmt.data()); // arm2
                drawStmt->y3 = Function::temporaryFunction(tok3 + ".y", drawStmt.data());
            }
        }

        // Shared optional suffixes: arcs N, as "..."
        consumeSpace(body);
        if (body.startsWith(Keywords::KW_ARCS)) {
            body = body.mid(Keywords::KW_ARCS.length());
            consumeSpace(body);
            QString fnai;
            QString sa = extract_proper_expression(body, fnai, {' '}, {}, false);
            drawStmt->arcCount = qRound(Function::temporaryFunction(sa, drawStmt.data())->Calculate());
            if (drawStmt->arcCount < 1) drawStmt->arcCount = 1;
        }

        // Optional modifiers: color name, double/single (any order, before optional as "...")
        consumeSpace(body);
        while (!body.isEmpty() && !body.startsWith(Keywords::KW_AS)) {
            consumeSpace(body);
            if (body.isEmpty() || body.startsWith(Keywords::KW_AS)) break;
            QString snapshot = body;
            QString token = getDelimitedId(body);
            if (token.toLower() == "double") {
                drawStmt->doubleLine = true;
            } else if (token.toLower() == "single") {
                drawStmt->doubleLine = false;
            } else if (QColor(token).isValid()) {
                drawStmt->drawColor = token;
            } else {
                body = snapshot; // unknown token — stop consuming
                break;
            }
            consumeSpace(body);
        }

        consumeSpace(body);
        if (body.startsWith(Keywords::KW_AS)) {
            body = body.mid(Keywords::KW_AS.length());
            consumeSpace(body);
            if (!body.isEmpty() && body[0] == '"') {
                body = body.mid(1);
                int closeQuote = body.indexOf('"');
                if (closeQuote >= 0) {
                    drawStmt->angleLabel = body.left(closeQuote);
                    body = body.mid(closeQuote + 1);
                }
            }
        }

        result.append(drawStmt);
    } else {
        throw funkplot::syntax_error_exception(ERRORCODE(35), "Invalid draw statement: %s", codeline.toStdString().c_str());
    }

    return handleStatementCallback(result, cb);
}

bool Draw::execute(RuntimeProvider *rp)
{
    if (drawType == Line || drawType == Segment) {
        double vx1, vy1, vx2, vy2;
        QSharedPointer<SegmentAssignment> drawnSa;

        if (!targetVar.isEmpty()) {
            auto a = rp->getAssignment(targetVar);
            if (a) {
                auto la = qSharedPointerDynamicCast<LineAssignment>(a);
                if (la) {
                    vx1 = la->x1->Calculate(); vy1 = la->y1->Calculate();
                    vx2 = la->x2->Calculate(); vy2 = la->y2->Calculate();
                } else {
                    auto sa = qSharedPointerDynamicCast<SegmentAssignment>(a);
                    if (sa) {
                        vx1 = sa->x1->Calculate(); vy1 = sa->y1->Calculate();
                        vx2 = sa->x2->Calculate(); vy2 = sa->y2->Calculate();
                        drawnSa = sa;
                    } else return true;
                }
            } else return true;
        } else {
            vx1 = x1->Calculate();
            vy1 = y1->Calculate();
            vx2 = x2->Calculate();
            vy2 = y2->Calculate();
        }

        double x_min = rp->coordStartX();
        double x_max = rp->coordEndX();
        double y_min = rp->coordStartY();
        double y_max = rp->coordEndY();

        double draw_x1 = vx1, draw_y1 = vy1, draw_x2 = vx2, draw_y2 = vy2;

        if (drawType == Line) {
            double dx = vx2 - vx1;
            double dy = vy2 - vy1;

            if (dx == 0 && dy == 0) return true;

            QVector<QPointF> intersections;
            if (dx != 0) {
                auto get_y = [&](double x) { return vy1 + (dy / dx) * (x - vx1); };
                double y_at_min = get_y(x_min);
                if (y_at_min >= y_min && y_at_min <= y_max) intersections.append({x_min, y_at_min});
                double y_at_max = get_y(x_max);
                if (y_at_max >= y_min && y_at_max <= y_max) intersections.append({x_max, y_at_max});
            } else {
                // Vertical line
                if (vx1 >= x_min && vx1 <= x_max) {
                    intersections.append({vx1, y_min});
                    intersections.append({vx1, y_max});
                }
            }

            if (dy != 0) {
                auto get_x = [&](double y) { return vx1 + (dx / dy) * (y - vy1); };
                double x_at_min = get_x(y_min);
                if (x_at_min >= x_min && x_at_min <= x_max) intersections.append({x_at_min, y_min});
                double x_at_max = get_x(y_max);
                if (x_at_max >= x_min && x_at_max <= x_max) intersections.append({x_at_max, y_max});
            } else if (dx != 0) {
                // Horizontal line
                if (vy1 >= y_min && vy1 <= y_max) {
                    intersections.append({x_min, vy1});
                    intersections.append({x_max, vy1});
                }
            }

            if (intersections.size() >= 2) {
                draw_x1 = intersections[0].x();
                draw_y1 = intersections[0].y();
                draw_x2 = intersections[1].x();
                draw_y2 = intersections[1].y();
            } else {
                return true;
            }
        }

        if (count) {
            int n = static_cast<int>(count->Calculate());
            if (n < 1) n = 1;
            for (int i = 0; i < n; ++i) {
                double t = (n > 1) ? static_cast<double>(i) / (n - 1) : 0.0;
                double px = draw_x1 + t * (draw_x2 - draw_x1);
                double py = draw_y1 + t * (draw_y2 - draw_y1);
                rp->drawPoint(px, py);
            }
        } else {
            rp->drawLine(draw_x1, draw_y1, draw_x2, draw_y2);
        }

        // Congruency tick marks drawn at the midpoint of the actual segment
        if (ticks > 0 && drawType == Segment) {
            double sdx = vx2 - vx1, sdy = vy2 - vy1;
            double segLen = std::sqrt(sdx*sdx + sdy*sdy);
            if (segLen > 1e-9) {
                double ndx = sdx / segLen, ndy = sdy / segLen;
                // Perpendicular biased upward/rightward for consistency
                double ppx = -ndy, ppy = ndx;
                if (ppy < 0.0 || (ppy == 0.0 && ppx < 0.0)) { ppx = -ppx; ppy = -ppy; }

                double tickHalf = std::min(0.05 * segLen, 0.10);
                double spacing  = tickHalf * 1.1;
                double mx = (vx1 + vx2) / 2.0;
                double my = (vy1 + vy2) / 2.0;

                QColor savedTickColor = rp->currentPenColor();
                int savedTickPs = rp->getPixelSize();
                if (!drawColor.isEmpty()) {
                    QColor c(drawColor);
                    if (c.isValid())
                        rp->setPen(c.red(), c.green(), c.blue(), c.alpha());
                }
                if (doubleLine)
                    rp->setPixelSize(savedTickPs * 2);

                for (int i = 0; i < ticks; ++i) {
                    double offset = (ticks > 1) ? (i - (ticks - 1) / 2.0) * spacing : 0.0;
                    double cx = mx + offset * ndx;
                    double cy = my + offset * ndy;
                    rp->drawLine(cx - ppx * tickHalf, cy - ppy * tickHalf,
                                 cx + ppx * tickHalf, cy + ppy * tickHalf);
                }

                rp->setPen(savedTickColor.red(), savedTickColor.green(), savedTickColor.blue(), savedTickColor.alpha());
                rp->setPixelSize(savedTickPs);
            }
        }

        if (drawnSa) {
            // Compute normalised segment direction
            double dx = vx2 - vx1, dy = vy2 - vy1;
            double segLen = std::sqrt(dx*dx + dy*dy);
            if (segLen < 1e-9) segLen = 1.0;
            double ndx = dx / segLen, ndy = dy / segLen;

            // Perpendicular in world space: (-ndy, ndx).
            // Flip sign so it points "upward" (positive world y) or, if horizontal,
            // "rightward" (positive world x).  This keeps labels consistently on the
            // same side for any segment orientation.
            double px = -ndy, py = ndx;
            if (py < 0.0 || (py == 0.0 && px < 0.0)) { px = -px; py = -py; }

            // ep1: offset = perp + backward  (away from ep2, one side of segment)
            // ep2: offset = perp + forward   (away from ep1, same side of segment)
            // Normalise each combined direction.
            auto makeDir = [](double cx, double cy) -> QPointF {
                double l = std::sqrt(cx*cx + cy*cy);
                if (l < 1e-9) l = 1.0;
                return {cx / l, cy / l};
            };

            if (!drawnSa->ep1Label.isEmpty()) {
                QPointF d1 = rp->segLabelFirstDir().isNull()
                             ? makeDir(px - ndx, py - ndy)
                             : rp->segLabelFirstDir();
                rp->setPendingLabelDir(d1);
                rp->setPendingLabelDistance(rp->labelDistance());
                rp->drawPoint(vx1, vy1);
                rp->drawText(vx1, vy1, drawnSa->ep1Label);
            }
            if (!drawnSa->ep2Label.isEmpty()) {
                QPointF d2 = rp->segLabelSecondDir().isNull()
                             ? makeDir(px + ndx, py + ndy)
                             : rp->segLabelSecondDir();
                rp->setPendingLabelDir(d2);
                rp->setPendingLabelDistance(rp->labelDistance());
                rp->drawPoint(vx2, vy2);
                rp->drawText(vx2, vy2, drawnSa->ep2Label);
            }
        }
    } else if (drawType == Point) {
        int ps = rp->getPixelSize();
        double px = x1->Calculate(), py = y1->Calculate();
        if (!pointLabel.isEmpty()) {
            QPointF dir = rp->labelDir();
            if (dir.isNull()) dir = {0.707, 0.707};
            rp->setPendingLabelDir(dir);
            rp->setPendingLabelDistance(rp->labelDistance());
        }
        rp->drawPoint(px, py, ps < 3 ? 3 : ps);
        if (!pointLabel.isEmpty())
            rp->drawText(px, py, pointLabel);
    } else if (drawType == Angle) {
        double vx, vy, ax, ay, bx, by;

        if (!targetVar2.isEmpty()) {
            // Form: draw angle between seg1 and seg2 — resolve common vertex
            auto sa1 = rp->getAssignmentAs<SegmentAssignment>(targetVar);
            auto sa2 = rp->getAssignmentAs<SegmentAssignment>(targetVar2);
            if (!sa1 || !sa2) return true;

            double s1x1 = sa1->x1->Calculate(), s1y1 = sa1->y1->Calculate();
            double s1x2 = sa1->x2->Calculate(), s1y2 = sa1->y2->Calculate();
            double s2x1 = sa2->x1->Calculate(), s2y1 = sa2->y1->Calculate();
            double s2x2 = sa2->x2->Calculate(), s2y2 = sa2->y2->Calculate();

            auto same = [](double xa, double ya, double xb, double yb) {
                return std::abs(xa - xb) < 1e-9 && std::abs(ya - yb) < 1e-9;
            };

            if      (same(s1x1,s1y1, s2x1,s2y1)) { vx=s1x1; vy=s1y1; ax=s1x2; ay=s1y2; bx=s2x2; by=s2y2; }
            else if (same(s1x1,s1y1, s2x2,s2y2)) { vx=s1x1; vy=s1y1; ax=s1x2; ay=s1y2; bx=s2x1; by=s2y1; }
            else if (same(s1x2,s1y2, s2x1,s2y1)) { vx=s1x2; vy=s1y2; ax=s1x1; ay=s1y1; bx=s2x2; by=s2y2; }
            else if (same(s1x2,s1y2, s2x2,s2y2)) { vx=s1x2; vy=s1y2; ax=s1x1; ay=s1y1; bx=s2x1; by=s2y1; }
            else return true; // no shared endpoint — nothing to draw
        } else {
            vx = x1->Calculate(); vy = y1->Calculate();
            ax = x2->Calculate(); ay = y2->Calculate();
            bx = x3->Calculate(); by = y3->Calculate();
        }

        // Draw the two arms with the inherited (current) pen
        rp->drawLine(vx, vy, ax, ay);
        rp->drawLine(vx, vy, bx, by);

        double theta1 = std::atan2(ay - vy, ax - vx);
        double theta2 = std::atan2(by - vy, bx - vx);

        // Sweep from theta1 toward theta2 through the interior (shorter) arc
        double span = theta2 - theta1;
        while (span >  M_PI) span -= 2.0 * M_PI;
        while (span < -M_PI) span += 2.0 * M_PI;

        double d1 = std::sqrt((ax-vx)*(ax-vx) + (ay-vy)*(ay-vy));
        double d2 = std::sqrt((bx-vx)*(bx-vx) + (by-vy)*(by-vy));
        double r  = 0.15 * std::min(d1, d2);
        if (r < 1e-9) r = 0.25;

        double angleDeg = std::abs(span) * 180.0 / M_PI;
        const double rightAngleTolerance = 0.5; // degrees
        const double arcGap = r * 0.20;         // gap between concentric arcs

        bool isRight = (arcCount == 1) && (std::abs(angleDeg - 90.0) < rightAngleTolerance);

        // Apply color/thickness overrides only to the arc markings
        QColor savedColor = rp->currentPenColor();
        int savedPs = rp->getPixelSize();
        if (!drawColor.isEmpty()) {
            QColor c(drawColor);
            if (c.isValid())
                rp->setPen(c.red(), c.green(), c.blue(), c.alpha());
        }
        if (doubleLine)
            rp->setPixelSize(savedPs * 2);

        for (int arcIdx = 0; arcIdx < arcCount; ++arcIdx) {
            double ri = r + arcIdx * arcGap;

            if (isRight) {
                // Right angle marker: small square (only drawn once, arcCount == 1)
                double ux = std::cos(theta1), uy = std::sin(theta1);
                double wx = std::cos(theta2), wy = std::sin(theta2);
                double qx1 = vx + ri * ux,          qy1 = vy + ri * uy;
                double qx2 = vx + ri * ux + ri * wx, qy2 = vy + ri * uy + ri * wy;
                double qx3 = vx + ri * wx,           qy3 = vy + ri * wy;
                rp->drawLine(qx1, qy1, qx2, qy2);
                rp->drawLine(qx2, qy2, qx3, qy3);
            } else {
                // General arc (also used for right angles when arcCount > 1)
                const int N = 48;
                double prevX = vx + ri * std::cos(theta1);
                double prevY = vy + ri * std::sin(theta1);
                for (int i = 1; i <= N; ++i) {
                    double t  = theta1 + span * static_cast<double>(i) / N;
                    double nx = vx + ri * std::cos(t);
                    double ny = vy + ri * std::sin(t);
                    rp->drawLine(prevX, prevY, nx, ny);
                    prevX = nx; prevY = ny;
                }
            }
        }

        rp->setPen(savedColor.red(), savedColor.green(), savedColor.blue(), savedColor.alpha());
        rp->setPixelSize(savedPs);

        // Angle value label — place it beyond the outermost arc
        double midAngle = theta1 + span / 2.0;
        double outerR   = r + (arcCount - 1) * arcGap;
        double labelR   = outerR * 1.7;
        double lx = vx + labelR * std::cos(midAngle);
        double ly = vy + labelR * std::sin(midAngle);

        rp->setPendingLabelDir({std::cos(midAngle), std::sin(midAngle)});
        rp->setPendingLabelDistance(0.0);

        QString angleStr = QString::number(angleDeg, 'f', 1) + QChar(0x00B0);
        rp->drawText(lx, ly, angleStr);

        // Optional labeled points: "as ABC" draws dots at arm1 (A), vertex (B), arm2 (C)
        if (!angleLabel.isEmpty()) {
            QString lA, lB, lC;
            splitAngleLabels(angleLabel, lA, lB, lC);

            auto normDir = [](double dx, double dy) -> QPointF {
                double l = std::sqrt(dx*dx + dy*dy);
                if (l < 1e-9) l = 1.0;
                return {dx / l, dy / l};
            };

            int dotSize = savedPs < 3 ? 3 : savedPs;

            // arm1 point — label directed away from vertex
            if (!lA.isEmpty()) {
                rp->setPendingLabelDir(normDir(ax - vx, ay - vy));
                rp->setPendingLabelDistance(rp->labelDistance());
                rp->drawPoint(ax, ay, dotSize);
                rp->drawText(ax, ay, lA);
            }
            // vertex — label directed opposite to the angle bisector (away from interior)
            if (!lB.isEmpty()) {
                rp->setPendingLabelDir(normDir(-std::cos(midAngle), -std::sin(midAngle)));
                rp->setPendingLabelDistance(rp->labelDistance());
                rp->drawPoint(vx, vy, dotSize);
                rp->drawText(vx, vy, lB);
            }
            // arm2 point — label directed away from vertex
            if (!lC.isEmpty()) {
                rp->setPendingLabelDir(normDir(bx - vx, by - vy));
                rp->setPendingLabelDistance(rp->labelDistance());
                rp->drawPoint(bx, by, dotSize);
                rp->drawText(bx, by, lC);
            }
        }
    }
    return true;
}
