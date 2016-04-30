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

    bool m_bHorizontalRuler;//ˮƽ�ߡ���ֱ��

    QPainterPath m_framePath; //�������
    QPainterPath m_graduationPath;//���ӿ̶�
    QVector<QPointF> m_textPos;//���������ֵ�λ��

    QString m_rulerDescription;
};

#endif // SCREENRULER_H
