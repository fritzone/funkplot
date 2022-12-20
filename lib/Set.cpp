#include "Assignment.h"
#include "IndexedAccess.h"
#include "Set.h"
#include "util.h"
#include "RuntimeProvider.h"
#include "colors.h"
#include "Plot.h"

#include <QFile>
#include <QtMath>

bool Set::execute(RuntimeProvider *rp)
{
    if(what == SetTargets::TARGET_COLOR || what == SetTargets::TARGET_COLOUR)
    {
        setColor(rp);
    }
    else
    if(what == SetTargets::TARGET_PALETTE)
    {
        rp->setPalette(value);
    }
    else
    if(what == SetTargets::TARGET_PIXEL)
    {
        // split up value
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
        QStringList vs = value.split(" ", Qt::SkipEmptyParts);
#else
        QStringList vs = value.split(" ", QString::SkipEmptyParts);
#endif
        if(vs.size() < 2)
        {
            throw syntax_error_exception(ERRORCODE(31), "Erroneous pixel property: <b>%s</b>", what.toStdString().c_str());
        }

        if(vs[0] == "size")
        {
            IndexedAccess* ia = nullptr; Assignment* as = nullptr;
            QString size;
            for(int i=1; i<vs.size(); i++)
            {
                size += vs[i];
            }
            auto s = Function::temporaryFunction(size.simplified(), this)->Calculate(rp, ia, as);
            qDebug() << "Setting pixel size:" << s;
            rp->setPixelSize(s);
        }
    }
    else
    if(what == SetTargets::TARGET_COORDINATES)
    {
        if(value == "on" || value == "off")
        {
            bool on = value == "on";
            rp->setShowCoordinates(on);
            return true;
        }

        QStringList vs = value.split(" ");
        if(!vs.isEmpty())
        {
            if(vs[0] == "start")
            {
                if(vs.length() == 3 && vs[1] == "x")
                {
                    double dv = vs[2].toDouble();
                    emit rp->coordStartXChange(dv);
                }
                if(vs.length() == 3 && vs[1] == "y")
                {
                    double dv = vs[2].toDouble();
                    emit rp->coordStartYChange(dv);
                }
            }
            else
            if(vs[0] == "end")
            {
                if(vs.length() == 3 && vs[1] == "x")
                {
                    double dv = vs[2].toDouble();
                    emit rp->coordEndXChange(dv);
                }
                if(vs.length() == 3 && vs[1] == "y")
                {
                    double dv = vs[2].toDouble();
                    emit rp->coordEndYChange(dv);
                }
            }
            else
            if(vs[0] == "grid")
            {
                if(vs.length() == 2)
                {
                    bool bv = (vs[1] == "on");
                    emit rp->gridChange(bv);
                }
            }
            else
            if(vs[0] == "zoom")
            {
                if(vs.length() == 2)
                {
                    double dv = vs[1].toDouble();
                    emit rp->zoomFactorChange(dv);
                }
            }
            else
            if(vs[0] == "rotation")
            {
                if(vs.length() == 2)
                {
                    double dv = vs[1].toDouble();
                    emit rp->rotationAngleChange(dv);
                }
                else
                if(vs.length() == 3)
                {
                    double dv = vs[1].toDouble();
                    if(vs[2] == "radians")
                    {
                        emit rp->rotationAngleChange(dv);
                    }
                    else
                    {
                        emit rp->rotationAngleChange(qDegreesToRadians(dv));
                    }
                }

            }
        }
        else
        {
            rp->setShowCoordinates(true);
            return true;
        }
    }

    return true;
}

void Set::setColor(RuntimeProvider *rp)
{
    std::string s = value.toStdString();

    if(Colors::colormap.count(s))
    {
        auto cui = Colors::colormap.at(s);

        rp->setPen(cui.r, cui.g, cui.b, 255);
    }
    else // a color from the current palette?
    if(value.startsWith("palette"))
    {
        QString local_value = value.mid(7);

        // skip space
        while(!local_value.isEmpty() && local_value.at(0).isSpace()) local_value = local_value.mid(1);
        // skip [

        bool needs_pq = false;
        if(!local_value.isEmpty() && local_value.at(0) == '[')
        {
            needs_pq = true;
            local_value = local_value.mid(1);
        }

        QString palIdx = "";
        while(!local_value.isEmpty())
        {
            if(local_value.at(0) != ']')
            {
                palIdx += local_value.at(0);
            }
            else
            {
                break;
            }
            local_value = local_value.mid(1);
        }

        if(!local_value.isEmpty()&& local_value.at(0) == ']' && !needs_pq || local_value.isEmpty() && needs_pq )
        {
            throw syntax_error_exception(ERRORCODE(32), "Wrong palette indexing: <b>%s</b>", s.c_str());
        }

        if(!local_value.isEmpty()) local_value = local_value.mid(1);


        IndexedAccess* ia = nullptr; Assignment* as = nullptr;
        int pidx = Function::temporaryFunction(palIdx.simplified(), this)->Calculate(rp, ia, as),
            cidx = 0;

        QString p = rp->getCurrentPalette();
        QFile f(":/palettes/upals/" + p + ".map");
        if(f.open(QIODevice::Text | QIODevice::ReadOnly))
        {
            QTextStream stream(&f);
            while (!stream.atEnd())
            {
                QString line = stream.readLine();
                if(cidx == pidx)
                {
    #if QT_VERSION > QT_VERSION_CHECK(5, 15, 0)
                    QStringList rgbs = line.split(" ", Qt::SkipEmptyParts);
    #else
                    QStringList rgbs = line.split(" ", QString::SkipEmptyParts);
    #endif
                    // see if we have an alpha value
                    int alpha = 255;
                    if(!local_value.isEmpty())
                    {
                        if(local_value.startsWith(","))
                        {
                            local_value = local_value.mid(1);
                            if(!local_value.isEmpty())
                            {
                                IndexedAccess* ia = nullptr; Assignment* as = nullptr;
                                alpha = Function::temporaryFunction(local_value.simplified(), this)->Calculate(rp, ia, as);
                            }
                        }
                    }
                    rp->setPen(rgbs[0].toInt(), rgbs[1].toInt(), rgbs[2].toInt(), alpha);
                    break;
                }
                cidx ++;
            }
        }
        else
        {
            qWarning() << "Cannot set paletted color, file not found:" << p;
        }
    }
    else // is this an RGB color?
    if(value.startsWith("#") && value.indexOf(',') == -1)
    {
        auto rgb = Colors::decodeHexRgb(value.toLocal8Bit().data());

        rp->setPen(std::get<0>(rgb), std::get<1>(rgb), std::get<2>(rgb), std::get<3>(rgb));
    }
    else // what remains is a list of RGB values
    {
        QStringList values = value.split(",");
        qreal a = 1.0;
        switch(values.length())
        {
        case 4: // R,G,B,A
        {
            IndexedAccess* ia = nullptr; Assignment* as = nullptr;
            a = Function::temporaryFunction(values[3].simplified(), this)->Calculate(rp, ia, as);
        }
        [[fallthrough]];

        case 3: // R,G,B
        {
            IndexedAccess* ia = nullptr; Assignment* as = nullptr;
            qreal r = Function::temporaryFunction(values[0].simplified(), this)->Calculate(rp, ia, as);
            qreal g = Function::temporaryFunction(values[1].simplified(), this)->Calculate(rp, ia, as);
            qreal b = Function::temporaryFunction(values[2].simplified(), this)->Calculate(rp, ia, as);

            qreal finalR = r;
            qreal finalG = g;
            qreal finalB = b;
            qreal finalA = a;

            if(r < 1.0)
            {
                finalR = 255.0 * r;
            }
            if(g < 1.0)
            {
                finalG = 255.0 * g;
            }
            if(b < 1.0)
            {
                finalB = 255.0 * b;
            }
            if(a <= 1.0)
            {
                finalA = 255.0 * a;
            }

            rp->setPen(static_cast<int>(finalR), static_cast<int>(finalG),
                       static_cast<int>(finalB), static_cast<int>(finalA));
            break;
        }
        case 2: // colorname,A
        {
            if(Colors::colormap.count( values[0].simplified().toStdString() ))
            {
                auto cui = Colors::colormap.at(values[0].simplified().toStdString());

                IndexedAccess* ia = nullptr; Assignment* as = nullptr;
                a = Function::temporaryFunction(values[1].simplified(), this)->Calculate(rp, ia, as);
                qreal finalA = a;

                if(a < 1.0)
                {
                    finalA = 255.0 * a;
                }

                rp->setPen(cui.r, cui.g, cui.b, static_cast<int>(finalA));
            }
            else // is this an RGB color?
            if(value.startsWith("#"))
            {
                auto rgb = Colors::decodeHexRgb(values[0].simplified().toStdString().c_str());
                IndexedAccess* ia = nullptr; Assignment* as = nullptr;
                a = Function::temporaryFunction(values[1].simplified(), this)->Calculate(rp, ia, as);
                qreal finalA = a < 1.0 ? 255.0 * a : a;

                rp->setPen(std::get<0>(rgb), std::get<1>(rgb), std::get<2>(rgb), static_cast<int>(finalA));
            }
            break;
        }
        default:
        {
            throw syntax_error_exception(ERRORCODE(33), "Invalid color to set:<b>%s</b>", value.toStdString().c_str());
        }
        }
    }
}
