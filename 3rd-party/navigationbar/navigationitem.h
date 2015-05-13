#ifndef NAVIGATIONITEM_H
#define NAVIGATIONITEM_H

#include <QtGui>
#include <QGraphicsObject>

class NavigationItem : public QGraphicsObject
{
    Q_OBJECT
public:
    explicit NavigationItem(const QPixmap &pixmap, QGraphicsItem *parent = 0);
    explicit NavigationItem(const QString &text, const QPixmap &pixmap, const int index, QGraphicsItem *parent = 0);

public:
    void SetBoundingRect(QRectF &boundRect);
    QPixmap Pixmap() const;
    void SetPixmap(const QPixmap &pixmap);

    QString Text() const;
    void SetText(QString &text);

    int Index() const;
    void SetIndex(int Index);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    QRectF boundingRect() const;
    QPainterPath shape() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

private:
    QPixmap m_Pixmap;
    QRectF m_BoundRect;
    QString m_Text;
    int m_Index;
};

#endif // NAVIGATIONITEM_H
