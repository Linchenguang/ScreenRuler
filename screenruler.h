#ifndef SCREENRULER_H
#define SCREENRULER_H

#include <QMainWindow>
#include <QPoint>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QPainterPath>
#include <QVector>

class ScreenRuler : public QMainWindow
{
    Q_OBJECT

public:
    explicit ScreenRuler(QWidget *parent = 0);
    ~ScreenRuler();

protected:
    virtual void mouseMoveEvent(QMouseEvent* event);
    virtual void mousePressEvent(QMouseEvent* event);
    virtual void mouseReleaseEvent(QMouseEvent* event);
    virtual void paintEvent(QPaintEvent* event);

private:
    void UpdateDisplayPixelsPerCM();
    void ResizeRuler();
    void BuildPath();
    void BuildFramePath();
    void BuildGraduation();

private:
    qreal m_horDotsPerCM;
    qreal m_verDotsPerCM;
    QPoint m_originWidgetPos;
    QPoint m_originMouseGloblePos;
    bool   m_leftMouseButtonPressed;

    bool m_bHorizontalRuler;//水平尺、竖直尺

    QPainterPath m_framePath; //尺子外框
    QPainterPath m_graduationPath;//尺子刻度
    QVector<QPointF> m_textPos;//尺子上文字的位置

    QString m_rulerDescription;
};

#endif // SCREENRULER_H
