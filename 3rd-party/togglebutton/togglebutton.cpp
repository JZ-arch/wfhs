#include "togglebutton.h"

#include <QPainter>
#include <QMouseEvent>
#include <QDebug>

ToggleButton::ToggleButton(QWidget *parent) :
    QPushButton(parent), m_isPressed(false)
{
    this->setCheckable(true);
    //this->setGeometry(0, 0, 50, 50);
    qDebug() << this->size();
}

ToggleButton::~ToggleButton()
{
}

void ToggleButton::setNormalImage(QString strRes)
{
    m_strImgNormal = strRes;
}

void ToggleButton::setPressedImage(QString strRes)
{
    m_strImgPressed = strRes;
}

void ToggleButton::paintEvent(QPaintEvent *event)
{
    QPixmap pixmap;
    if (m_isPressed)
    {
        pixmap.load(m_strImgPressed);
    }
    else
    {
        pixmap.load(m_strImgNormal);
    }

    QPainter painter(this);
    painter.drawPixmap(this->rect(), pixmap);
}

void ToggleButton::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        m_isPressed = !m_isPressed;   
    }
    QPushButton::mousePressEvent(event);
}

