#ifndef FRAMEFORLINENUMBERS_H
#define FRAMEFORLINENUMBERS_H

#include <QFrame>
#include <QVector>


class FrameForLineNumbers : public QFrame
{
    Q_OBJECT

    struct NumberPosition
    {
        NumberPosition() : nr(0), y(0), disabled(false) {}
        NumberPosition(int n, int a, bool d) : nr(n), y(a), disabled(d) {}
        int nr;
        int y;
        bool disabled;
    };
public:
    explicit FrameForLineNumbers(QWidget *parent = 0);

    void beginLineNumbers();
    void addLineNumber(int nr, int y, bool disabled);
    void endLineNumbers();

protected:

    void paintEvent(QPaintEvent *);

signals:

public slots:
private:
    QVector<NumberPosition> m_nrs;
};

#endif // FRAMEFORLINENUMBERS_H
