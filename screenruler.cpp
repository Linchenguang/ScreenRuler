#include "screenruler.h"
#include <QPainter>
#include "EDIDReader.h"
#include <QMessageBox>

//直尺的左侧的空白大小
static const qreal RULER_EDGE = 15;
static const qreal HOR_RULER_WIDTH = 1024;
static const qreal HOR_RULER_HEIGHT = 80;
static const qreal VER_RULER_WIDTH = 80;
static const qreal VER_RULER_HEIGHT = 700;

ScreenRuler::ScreenRuler(QWidget *parent) :
    QMainWindow(parent, Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint),
    m_leftMouseButtonPressed(false),
    m_horDotsPerCM(44),
    m_verDotsPerCM(44),
    m_bHorizontalRuler(false)
{
    UpdateDisplayPixelsPerCM();

    ResizeRuler();
    
    BuildPath();    
}

ScreenRuler::~ScreenRuler()
{

}

void ScreenRuler::mouseMoveEvent(QMouseEvent* event)
{
    if (m_leftMouseButtonPressed)
    {
        QPoint newMouseGloblePos = event->globalPos();
        move(newMouseGloblePos - m_originMouseGloblePos + m_originWidgetPos);
    }
}

void ScreenRuler::mousePressEvent(QMouseEvent* event)
{
    if (Qt::LeftButton == event->button())
    {
        m_leftMouseButtonPressed  = true;
        m_originMouseGloblePos = event->globalPos();
        m_originWidgetPos = pos();
    }
    else if (Qt::RightButton == event->button())
    {
        m_bHorizontalRuler = !m_bHorizontalRuler;
        ResizeRuler();
        BuildPath();
        update();
    }
}

void ScreenRuler::mouseReleaseEvent(QMouseEvent* event)
{
    if (Qt::LeftButton == event->button())
    {
        m_leftMouseButtonPressed  = false;
    }
    else if (Qt::LeftButton == event->button())
    {
    }
}


void ScreenRuler::paintEvent(QPaintEvent * event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // --- brush 1 ----
    QBrush brush(painter.brush());
    brush.setColor(QColor(Qt::yellow));

    // --- brush 2 ----
    //QRadialGradient gradient(50, 50, 50, 50, 50);
    //gradient.setColorAt(0, QColor::fromRgbF(0, 1, 0, 1));
    //gradient.setColorAt(1, QColor::fromRgbF(0, 0, 0, 0));
    //QBrush brush(gradient);

    painter.setBrush(brush);
    painter.fillRect(0, 0, width(),height(), QColor(Qt::yellow));

    //画外框
    QPen pen(painter.pen());
    pen.setStyle(Qt::SolidLine);
    pen.setWidth(2);
    pen.setBrush(Qt::darkGreen);
    painter.setPen(pen);
    painter.drawPath(m_framePath);


    //画刻度线
    pen.setWidth(1);
    pen.setBrush(Qt::black);
    painter.setPen(pen);
    painter.drawPath(m_graduationPath);

    //画刻度文字
    QRectF rectText = QRectF(0, 0, 1000, 1000);
    for (int i=0; i<m_textPos.size(); i++)
    {
        rectText.moveCenter(m_textPos[i]);
        painter.drawText(rectText, Qt::AlignCenter, QString(tr("%1").arg(i)));
    }

    //
    if (m_bHorizontalRuler)
    {
        rectText.moveCenter(QPointF(width()/2, height()*3/4));
        painter.drawText(rectText, Qt::AlignCenter, m_rulerDescription + tr(" Unit: cm"));
    }
    else
    {
        rectText.moveCenter(QPointF(width()/2, height()*3/4));
        painter.drawText(rectText, Qt::AlignCenter, tr("Unit: cm"));
    }
}

void ScreenRuler::ResizeRuler()
{
    if (m_bHorizontalRuler)
    {
        resize(HOR_RULER_WIDTH, HOR_RULER_HEIGHT);
    }
    else
    {
        resize(VER_RULER_WIDTH, VER_RULER_HEIGHT);
    }
}

void ScreenRuler::BuildPath()
{
    //先清空
    m_textPos.clear();
    m_framePath = QPainterPath();
    m_graduationPath = QPainterPath();

    BuildFramePath();
    BuildGraduation();
}

void ScreenRuler::BuildFramePath()
{
    const qreal WIDTH = width();
    const qreal HEIGHT = height();

    //画边框
    QPainterPath& framePath = m_framePath;
    framePath.addRect(0, 0, WIDTH, HEIGHT);
}

void ScreenRuler::BuildGraduation()
{
    const qreal WIDTH = width();
    const qreal HEIGHT = height();

    int centimetersToDraw = (WIDTH - RULER_EDGE*2) / m_horDotsPerCM; //厘米数
    QPainterPath& path = m_graduationPath;
    if (m_bHorizontalRuler)
    {
        for (int i=0; i<=centimetersToDraw*10; i++)
        {
            //刻度线
            qreal x1 = RULER_EDGE+i*m_horDotsPerCM/10;
            qreal y1 = 0;
            qreal x2 = x1;
            qreal y2 = 10;
            if (i%10 == 0)
            {
                y2 = 20;
            }
            else if(i%5 == 0)
            {
                y2 = 15;
            }
            path.moveTo(x1, y1);
            path.lineTo(x2, y2);

            //刻度
            if (i%10 == 0)
            {
                qreal x3 = x2;
                qreal y3 = y2 + 10;
                m_textPos.push_back(QPointF(x3, y3));
            }
        }
    }
    else
    {
        centimetersToDraw = (HEIGHT - RULER_EDGE*2) / m_verDotsPerCM;
        for (int i=0; i<=centimetersToDraw*10; i++)
        {
            qreal x1 = 0;
            qreal y1 = RULER_EDGE+i*m_verDotsPerCM/10;
            qreal x2 = 10;
            qreal y2 = y1;
            if (i%10 == 0)
            {
                x2 = 20;
            }
            else if (i%5 == 5)
            {
                x2 = 15;
            }
            path.moveTo(x1, y1);
            path.lineTo(x2, y2);

            //刻度
            if (i%10 == 0)
            {
                qreal x3 = x2 + 10;
                qreal y3 = y2;
                m_textPos.push_back(QPointF(x3, y3));
            }
        }
    }
}


void ScreenRuler::UpdateDisplayPixelsPerCM()
{
    qreal horDotsPerCM = 44;
    qreal verDotsPerCM = 44;

#ifdef Q_OS_WIN
    EDIDReader reader;
    QString strManufacture = reader.GetManufactureName();
    horDotsPerCM = 10.0 * reader.GetScreenWidth() / reader.GetScreenWidthMM();
    verDotsPerCM = 10.0 * reader.GetScreenHeight() / reader.GetScreenHeightMM();

    if (reader.GetScreenWidthMM() == 0 || reader.GetScreenHeightMM() == 0)
    {
        QMessageBox msgBox(QMessageBox::Warning, 
            "Display Information", 
            "Read display information failed",
            QMessageBox::Ok, 
            this);
        msgBox.exec();
    }
    else
    {
        m_horDotsPerCM = horDotsPerCM;
        m_verDotsPerCM = verDotsPerCM;

        m_rulerDescription = QString("Manufacture: %1, Size: %2mm * %3mm, Resolution: %4 * %5.")
            .arg(reader.GetManufactureName())
            .arg(reader.GetScreenWidthMM())
            .arg(reader.GetScreenHeightMM())
            .arg(reader.GetScreenWidth())
            .arg(reader.GetScreenHeight());

        QString msgBoxTitle = tr("Display Information");
        QString msgBoxText;

        if (reader.HasMultiScreen())
        {
            msgBoxText = QString("This computer has multi-screen. For 1mV ECG wave draw 1cm.\n Please use this display:\n\n\tManufacture: %1\n\tSceen size: %2mm * %3mm\n\tResolution: %4 * %5")
                .arg(strManufacture)
                .arg(reader.GetScreenWidthMM())
                .arg(reader.GetScreenHeightMM())
                .arg(reader.GetScreenWidth())
                .arg(reader.GetScreenHeight());
        }
        else
        {
            msgBoxText = QString("This PC has only one screen. Display information:\n\n\tManufacture: %1\n\tSceen size: %2mm * %3mm\n\tResolution: %4 * %5")
                .arg(strManufacture)
                .arg(reader.GetScreenWidthMM())
                .arg(reader.GetScreenHeightMM())
                .arg(reader.GetScreenWidth())
                .arg(reader.GetScreenHeight());
        }

        QMessageBox msgBox(QMessageBox::Information, msgBoxTitle, msgBoxText, QMessageBox::Ok, this);
        //msgBox.exec();
    }
#else
    horDotsPerCM = width();
    horDotsPerCM = 10.0 * horDotsPerCM / widthMM();

    verDotsPerCM = height();
    verDotsPerCM = 10.0 * verDotsPerCM / heightMM();
#endif
}