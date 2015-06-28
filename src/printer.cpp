
#include "printer.h"

#include <math.h>

#define PHANTOMJS_PDF_DPI 72 // Different defaults. OSX: 72, X11: 75(?), Windows: 96

Printer::Printer(QObject *parent) :
    QObject(parent),
    m_isFirstPage(true)
{
    m_printer = new QPrinter();
    m_printer->setOutputFormat(QPrinter::PdfFormat);
    m_printer->setResolution(PHANTOMJS_PDF_DPI);

    m_painter = new QPainter();
}

Printer::~Printer()
{
    delete m_painter;
    delete m_printer;
}

bool Printer::begin()
{
    return m_painter->begin(m_printer);
}

// Do not change the signature argument as this is introspected by the Qt type API to bind JavaScript calls
void Printer::printPage(QObject * object)
{
    if (!m_isFirstPage)
        m_printer->newPage();
    else
        m_isFirstPage = false;

    WebPage* page = (WebPage*) object;
    QWebFrame* frame = page->mainFrame();
    frame->render(m_painter, QRegion(QRect(QPoint(0, 0), frame->contentsSize())));
}

bool Printer::end()
{
    return m_painter->end();
}

QString Printer::outputFileName() const
{
    return m_printer->outputFileName();
}

void Printer::setOutputFileName(const QString &fileName)
{
    m_printer->setOutputFileName(fileName);
}

QVariantMap Printer::paperSize() const
{
    return m_paperSize;
}

void Printer::setPaperSize(const QVariantMap &paperSize)
{
    m_paperSize = paperSize;

    const QSizeF sizePt(ceil(stringToPointSize(paperSize.value("width").toString())),
                        ceil(stringToPointSize(paperSize.value("height").toString())));
    m_printer->setPaperSize(sizePt, QPrinter::Point);

    qreal marginLeft = 0, marginTop = 0, marginRight = 0, marginBottom = 0;
    if (paperSize.contains("margin")) {
        const QVariant margins = paperSize["margin"];
        if (margins.canConvert(QVariant::Map)) {
            const QVariantMap map = margins.toMap();
            marginLeft = printMargin(map, "left");
            marginTop = printMargin(map, "top");
            marginRight = printMargin(map, "right");
            marginBottom = printMargin(map, "bottom");
        } else if (margins.canConvert(QVariant::String)) {
            const qreal margin = stringToPointSize(margins.toString());
            marginLeft = margin;
            marginTop = margin;
            marginRight = margin;
            marginBottom = margin;
        }
    }
    m_printer->setPageMargins(marginLeft, marginTop, marginRight, marginBottom, QPrinter::Point);
}

qreal Printer::stringToPointSize(const QString &string)
{
    static const struct {
        QString unit;
        qreal factor;
    } units[] = {
        { "mm", 72 / 25.4 },
        { "cm", 72 / 2.54 },
        { "in", 72 },
        { "px", 72.0 / PHANTOMJS_PDF_DPI },
        { "", 72.0 / PHANTOMJS_PDF_DPI }
    };
    for (uint i = 0; i < sizeof(units) / sizeof(units[0]); ++i) {
        if (string.endsWith(units[i].unit)) {
            QString value = string;
            value.chop(units[i].unit.length());
            return value.toDouble() * units[i].factor;
        }
    }
    return 0;
}

qreal Printer::printMargin(const QVariantMap &map, const QString &key)
{
    const QVariant margin = map.value(key);
    if (margin.isValid() && margin.canConvert(QVariant::String))
        return stringToPointSize(margin.toString());
    else
        return 0;
}
