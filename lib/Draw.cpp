#include "Draw.h"
#include "RuntimeProvider.h"
#include "util.h"
#include "LineAssignment.h"
#include "SegmentAssignment.h"

Draw::Draw(int ln, const QString &s) : Statement(ln, s)
{}

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
            drawStmt->targetVar = getDelimitedId(body);
        }

        consumeSpace(body);
        if (body.startsWith(Keywords::KW_COUNTS)) {
            body = body.mid(Keywords::KW_COUNTS.length());
            consumeSpace(body);
            QString fnai;
            QString scount = extract_proper_expression(body, fnai, {' '}, {}, false);
            drawStmt->count = Function::temporaryFunction(scount, drawStmt.data());
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
    } else if (drawType == Point) {
        rp->drawPoint(x1->Calculate(), y1->Calculate());
    }
    return true;
}
