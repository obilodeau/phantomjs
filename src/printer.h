
#ifndef PRINTER_H
#define PRINTER_H

#include <QPrinter>
#include <QPainter>

#include "webpage.h"

class Printer : public QObject
{
    Q_OBJECT

public:
    Printer(QObject *parent);
    ~Printer();

    void setOutputFileName(const QString &);

public slots:
    void printPage(QObject *);

private:
    QPrinter *m_printer;
    QPainter *m_painter;
    
    bool isFirstPage = true;
    
    qreal stringToPointSize(const QString &);
    qreal printMargin(const QVariantMap &, const QString &);
};

#endif // PRINTER_H