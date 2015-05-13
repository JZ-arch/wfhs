#include "navigationbarview.h"
#include "navigationitem.h"
#include <QDebug>

NavigationBarView::NavigationBarView(QWidget *parent) :
    QGraphicsView(parent), m_index(0), m_animation(this, "index")
{
    m_ItemSize.width = ITEM_MIN_WIDTH;
    m_ItemSize.height = ITEM_MIN_HEIGHT;
    setScene(new QGraphicsScene(this));
    //setupScene();
    setIndex(0);

    m_animation.setDuration(200);
    m_animation.setEasingCurve(QEasingCurve::InOutSine);
    //m_animation.setEasingCurve(QEasingCurve::Linear);
    //setViewportUpdateMode(FullViewportUpdate);
    //setViewportUpdateMode(NoViewportUpdate);
    setRenderHint(QPainter::Antialiasing, true);
    setFrameStyle(QFrame::NoFrame);

    connect(scene(), SIGNAL(selectionChanged()), this, SLOT(SlotItemSelected()));
}

void NavigationBarView::AddItem(QString name, int index)
{
    QString path = ":/navigationbar/" + name + ".png";
    NavigationItem *pItem = new NavigationItem(name, path, index);
    pItem->setZValue(1);
    scene()->addItem(pItem);
    m_Items.append(pItem);
    update();
}

void NavigationBarView::RemoveItem(int index)
{
    int len = m_Items.length();
    for (int i = 0; i != len; i++)
    {
        if (m_Items[i]->Index() == index)
        {
            //要先取消item的选中状态再删除
            //否则调用removeItem时，scene()会发出selectionChanged信号
            //此时如果被删除的item又是选中的，就会发送itemSelected信号给主界面
            if (m_Items[i]->isSelected())
            {
                m_Items[i]->setSelected(false);
            }

            scene()->removeItem(m_Items[i]);
            m_Items.removeAt(i);
            update();
            break;
        }
    }
    //scene()->update();
}

void NavigationBarView::UpdateItem(int index, QString name)
{
    foreach (NavigationItem *pItem, m_Items)
    {
        if (pItem->Index() == index)
        {
            pItem->SetPixmap(":/navigationbar/" + name + ".png");
            pItem->SetText(name);
            update();
            break;
        }
    }
}

void NavigationBarView::SelectItem(QString name)
{
    int size = m_Items.length();
    for (int i = 0; i < size; i++)
    {
        if (m_Items[i]->Text() == name)
        {
            m_Items[i]->setSelected(true);
        }
        else
        {
            m_Items[i]->setSelected(false);
        }
    }
    update();
}

int NavigationBarView::index() const
{
    return m_index;
}

void NavigationBarView::setIndex(int index)
{
    //qDebug() << "index" << index;
    if (index >= 0 && (m_Items.count() - index) >= ITEM_MAX_NUM)
    {
        m_index = index;
    }

    QPointF pos;
    for (int i = 0; i < m_Items.count(); ++i)
    {
        QGraphicsItem *item = m_Items[i];
        pos = QPointF((i - m_index) * m_ItemSize.width + (m_ItemSize.width / 2), m_ItemSize.height / 2);
        item->setPos(pos);
    }

    scene()->update();
}

void NavigationBarView::setupScene()
{
    QStringList paths;
    paths << ":/navigationbar/accessories-calculator.png";
    paths << ":/navigationbar/accessories-text-editor.png";
    paths << ":/navigationbar/help-browser.png";
    paths << ":/navigationbar/office-calendar.png";
    paths << ":/navigationbar/internet-group-chat.png";
    paths << ":/navigationbar/internet-mail.png";
    paths << ":/navigationbar/internet-web-browser.png";

    QStringList names;
    names << tr("玄关");
    names << tr("客厅");
    names << tr("主卧");
    names << tr("次卧");
    names << tr("书房");
    names << tr("餐厅");
    names << tr("客房");
    names << tr("主卧1");

    for (int i = 0; i < paths.count(); i++)
    {
        QPixmap pixmap(paths[i]);
        NavigationItem *item = new NavigationItem(names[i], pixmap, i);
        item->setZValue(1);
        scene()->addItem(item);
        m_Items.append(item);
    }
}

void NavigationBarView::SlotLeftMove()
{
    if (m_animation.state() == QAbstractAnimation::Stopped)
    {
        m_animation.setEndValue(m_index - 1);
        m_animation.start();
    }
}

void NavigationBarView::SlotRightMove()
{
    if (m_animation.state() == QAbstractAnimation::Stopped)
    {
        m_animation.setEndValue(m_index + 1);
        m_animation.start();
    }
}

QList<NavigationItem*> NavigationBarView::Items() const
{
    return m_Items;
}

NavigationItem* NavigationBarView::ItemAt(int index)
{
    return m_Items.at(index);
}

void NavigationBarView::drawBackground(QPainter *painter, const QRectF &)
{
    painter->drawPixmap(int(sceneRect().left()), int(sceneRect().top()), QPixmap(":/navigationbar/background.png"));
}

void NavigationBarView::resizeEvent(QResizeEvent *event)
{
    QGraphicsView::resizeEvent(event);
    scene()->setSceneRect(0, 0, size().width(), size().height());
    update();
/*
    //fitInView(scene()->sceneRect(), Qt::KeepAspectRatio);
    QColor colorLeft(198, 162, 201);
    QColor colorRight(149, 146, 188);
    QLinearGradient gradient;
    gradient.setStart(scene()->sceneRect().topLeft());
    gradient.setFinalStop(scene()->sceneRect().bottomRight());
    gradient.setColorAt(0, colorLeft);
    gradient.setColorAt(1, colorRight);
    scene()->setBackgroundBrush(gradient);
    //fitInView(scene()->sceneRect(), Qt::KeepAspectRatio);
*/
}

void NavigationBarView::update()
{
    int viewCount = (ITEM_MAX_NUM > m_Items.count()) ? m_Items.count() : ITEM_MAX_NUM;
    qreal width = size().width() / (viewCount * 1.0) ;
    qreal height = size().height();
    if (width > ITEM_MIN_WIDTH || height > ITEM_MIN_HEIGHT)
    {
        m_ItemSize.width = (width > ITEM_MIN_WIDTH ? width : ITEM_MIN_WIDTH);
        m_ItemSize.height = (height > ITEM_MIN_HEIGHT ? height : ITEM_MIN_HEIGHT);

        for (int i = 0; i < m_Items.count(); i++)
        {
            NavigationItem *icon = m_Items[i];
            QRectF iconRect(0, 0, m_ItemSize.width, m_ItemSize.height);
            icon->SetBoundingRect(iconRect);
        }
        setIndex(m_index);
    }
}
void NavigationBarView::SlotItemSelected()
{
    for (int i = 0; i < m_Items.count(); i++)
    {
        if (m_Items[i]->isSelected())
        {
            emit itemSelected(i);
        }
    }
}
