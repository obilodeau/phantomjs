
#include "printer.h"

#define PHANTOMJS_PDF_DPI 72            // Different defaults. OSX: 72, X11: 75(?), Windows: 96

Printer::Printer(QObject *parent) :
    QObject(parent)
{
    m_printer = new QPrinter();
    m_printer->setOutputFormat(QPrinter::PdfFormat);
    m_printer->setOutputFileName("test.pdf");
    m_printer->setResolution(PHANTOMJS_PDF_DPI);
    QVariantMap paperSize;
//    
    if (paperSize.isEmpty()) {
//        const QSize pageSize = m_mainFrame->contentsSize();
        paperSize.insert("width", /*QString::number(pageSize.width()) +*/ "1280px");
        paperSize.insert("height", /*QString::number(pageSize.height()) +*/ "720px");
        paperSize.insert("margin", "0px");
    }
    
    if (paperSize.contains("width") && paperSize.contains("height")) {
        const QSizeF sizePt(ceil(stringToPointSize(paperSize.value("width").toString())),
                            ceil(stringToPointSize(paperSize.value("height").toString())));
        m_printer->setPaperSize(sizePt, QPrinter::Point);
    }
    
    if (paperSize.contains("border") && !paperSize.contains("margin")) {
        // backwards compatibility
        paperSize["margin"] = paperSize["border"];
    }

    qreal marginLeft = 0;
    qreal marginTop = 0;
    qreal marginRight = 0;
    qreal marginBottom = 0;
    
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
    
    m_painter = new QPainter(m_printer);
}

Printer::~Printer()
{
    delete m_painter;
    delete m_printer;
}

void Printer::printPage(QObject * object)
{
    if (!isFirstPage) {
        m_printer->newPage();
    } else {
        isFirstPage = false;
    }

    WebPage* page = (WebPage*) object;
    QWebFrame* frame = page->mainFrame();
    QSize contentsSize = frame->contentsSize();
    QRect frameRect = QRect(QPoint(0, 0), contentsSize);
//    page->setViewportSize(contentsSize);

    frame->render(m_painter, QRegion(frameRect));
}

void Printer::setOutputFileName(const QString &fileName)
{
    m_printer->setOutputFileName(fileName);
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
    if (margin.isValid() && margin.canConvert(QVariant::String)) {
        return stringToPointSize(margin.toString());
    } else {
        return 0;
    }
}
