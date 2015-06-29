
#ifndef PRINTER_H
#define PRINTER_H

#include <QPrinter>
#include <QPainter>

#include "webpage.h"

class Printer : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString outputFileName READ outputFileName WRITE setOutputFileName)
    Q_PROPERTY(QVariantMap paperSize READ paperSize WRITE setPaperSize)

public:
    Printer(QObject *parent);
    ~Printer();

    void setOutputFileName(const QString &);
    QString outputFileName() const;

    void setPaperSize(const QVariantMap &size);
    QVariantMap paperSize() const;

public slots:
    bool begin();
    void printPage(QObject *);
    bool end();

private:
    QPrinter *m_printer;
    QPainter *m_painter;
    QVariantMap m_paperSize;
    bool m_isFirstPage;

    qreal stringToPointSize(const QString &);
    qreal printMargin(const QVariantMap &, const QString &);
};

#endif // PRINTER_H