#include "navigationitem.h"
#include <QDebug>
#include <QStyle>
#include <QStyleOptionGraphicsItem>

#define TEXT_RECT_WIDTH 80
#define TEXT_RECT_HEIGHT 30

NavigationItem::NavigationItem(const QString &text, const QPixmap &pixmap, const int index, QGraphicsItem *parent):
    QGraphicsObject(parent), \
    m_Pixmap(pixmap), \
    m_BoundRect(0, 0, 30, 30), \
    m_Text(QString("#%1%2").arg(index).arg(text)), \
    m_Index(index)
{
    setAcceptHoverEvents(true);
    setFlag(ItemIsSelectable);
    setSelected(false);
}

QRectF NavigationItem::boundingRect() const
{
    qreal penWidth = 1;
    return QRectF(-((m_BoundRect.width() + penWidth) / 2),
                  -((m_BoundRect.height() + penWidth) / 2),
                  m_BoundRect.width() + penWidth,
                  m_BoundRect.height() + penWidth);

    //return QGraphicsProxyWidget::boundingRect();
}

QPainterPath NavigationItem::shape() const
{
    QPainterPath path;
    path.addRect(QRectF(boundingRect().left() + 5,
                        boundingRect().top() + 5,
                        m_BoundRect.width() - 10 ,
                        m_BoundRect.height() - 10));
    return path;
}

void NavigationItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(widget);

    //纵坐标减10将图片上移否则图片会位于Item区域中间
    painter->drawPixmap(
                boundingRect().center().x() - Pixmap().width() / 2, \
                boundingRect().center().y() - 10 - Pixmap().height() / 2, \
                Pixmap());

    //绘制鼠标悬停时矩形框
    if (option->state & QStyle::State_MouseOver)
    {
        painter->drawRect( \
                    boundingRect().center().x() - Pixmap().width() / 2 - 5, \
                    boundingRect().center().y() - 10 - Pixmap().height() / 2 - 5, \
                    Pixmap().width() + 10, Pixmap().height() + 10);
    }

    //绘制字体，选中为红色加粗
    if (option->state & QStyle::State_Selected)
    {
        QPen oldPen = painter->pen();
        painter->setPen(QColor(255, 0, 0));

        QFont oldFont = painter->font();
        QFont newFont("unifont", 16);
        newFont.setBold(true);
        newFont.setItalic(true);
        painter->setFont(newFont);

        painter->drawText( \
                    boundingRect().center().x() - TEXT_RECT_WIDTH / 2, \
                    boundingRect().center().y() - 7 + Pixmap().height() / 2, \
                    TEXT_RECT_WIDTH, TEXT_RECT_HEIGHT, \
                    Qt::AlignHCenter | Qt::AlignVCenter, \
                    m_Text);

       painter->setFont(oldFont);
       painter->setPen(oldPen);
    }
    else
    {
        painter->drawText( \
                    boundingRect().center().x() - TEXT_RECT_WIDTH / 2, \
                    boundingRect().center().y() - 7 + Pixmap().height() / 2, \
                    TEXT_RECT_WIDTH, TEXT_RECT_HEIGHT,
                    Qt::AlignHCenter | Qt::AlignVCenter, m_Text);
    }
}

void NavigationItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mousePressEvent(event);
    update();
}

void NavigationItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mouseReleaseEvent(event);
    update();
}

QString NavigationItem::Text() const
{
    return m_Text;
}

void NavigationItem::SetText(QString &text)
{
    m_Text = text;
}

QPixmap NavigationItem::Pixmap() const
{
    return m_Pixmap;
}

void NavigationItem::SetPixmap(const QPixmap &pixmap)
{
    m_Pixmap = pixmap;
}

void NavigationItem::SetBoundingRect(QRectF &boundRect)
{
    prepareGeometryChange();
    m_BoundRect = boundRect;
}

int NavigationItem::Index() const
{
    return m_Index;
}

void NavigationItem::SetIndex(int Index)
{
    m_Index = Index;
}
