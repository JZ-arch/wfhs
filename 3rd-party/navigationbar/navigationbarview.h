#ifndef NAVIGATIONBARVIEW_H
#define NAVIGATIONBARVIEW_H

#include <QGraphicsView>
#include <QPropertyAnimation>
#include "navigationitem.h"

#define ITEM_MIN_WIDTH 10
#define ITEM_MIN_HEIGHT 10
#define ITEM_MAX_NUM 5

typedef struct
{
    qreal width;
    qreal height;
}Size;

class NavigationBarView : public QGraphicsView
{
    Q_OBJECT
    Q_PROPERTY(int index READ index WRITE setIndex)

public:
    NavigationBarView(QWidget *parent = 0);
    void AddItem(QString name, int index);
    void RemoveItem(int index);
    void UpdateItem(int index, QString name);
    void SelectItem(QString name);
    QList<NavigationItem*> Items() const;
    NavigationItem* ItemAt(int index);

signals:
    void itemSelected(int index);

public slots:
    void SlotLeftMove();
    void SlotRightMove();
    void SlotItemSelected();

protected:
    void resizeEvent(QResizeEvent *event);
    void drawBackground(QPainter *painter, const QRectF &rect);

private:
    void setupScene();
    void setIndex(int);
    int index() const;
    void update();

private:
    int m_index;
    Size m_ItemSize;
    QList<NavigationItem*> m_Items;
    QPropertyAnimation m_animation;
};

#endif // NAVIGATIONBARVIEW_H
