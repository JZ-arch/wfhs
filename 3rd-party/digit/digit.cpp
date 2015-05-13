#include "digit.h"

Digit::Digit(int transition, int number, QWidget *parent) :
    QWidget(parent), \
    m_number(number), \
    m_transition(transition)
{
    setAttribute(Qt::WA_OpaquePaintEvent, true);
    //Widget paints all its pixels when it receives a paint event

    setAttribute(Qt::WA_NoSystemBackground, true);
    //Indicates that the widget has no background, i.e. when the widget receives paint events, the background is not automatically repainted.

    connect(&m_animator, SIGNAL(frameChanged(int)), SLOT(update()));
    //start animation

    m_animator.setFrameRange(0, 100);
    m_animator.setDuration(600);
    //Construct a 0.6-second timeline with a frame range of 0 - 100

    m_animator.setCurveShape(QTimeLine::EaseInOutCurve);
    //starts growing slowly, then runs steadily, then grows slowly again
}

void Digit::setTransition(int transition)
{
    m_transition = transition;
}

int Digit::transition() const
{
    return m_transition;
}

void Digit::setNumber(int number)
{
    if (m_number != number)
    {
        m_number = qBound(0, number, 99);
        preparePixmap();
        update();
    }
}

void Digit::transitTo(int number)
{
    if (m_number != number)
    {
        m_number = qBound(0, number, 99);
        m_lastPixmap = m_pixmap;
        preparePixmap();
        m_animator.stop();
        m_animator.start();
    }
}

void Digit::drawFrame(QPainter *painter, const QRect &rect)
{
    painter->setPen(Qt::NoPen);
    QLinearGradient gradient(rect.topLeft(), rect.bottomLeft());
    //Set linear gradient area

    gradient.setColorAt(0.00, QColor(245, 245, 245));
    gradient.setColorAt(0.49, QColor(192, 192, 192));
    gradient.setColorAt(0.51, QColor(245, 245, 245));
    gradient.setColorAt(1.00, QColor(192, 192, 192));
    //Creates stop points at the given position with the given color

    painter->setBrush(gradient);
    QRect r = rect;
    painter->drawRoundedRect(r, 15, 15, Qt::RelativeSize);
    /*
            Draws outer rectangle rect with rounded corners.
            Qt::RelativeSize specifies the size relative to the bounding rectangle,
            typically using percentage measurements.
        */

    r.adjust(1, 4, -1, -4);
    //Adds 1, 4, -1 and -4 respectively to the existing coordinates of the rectangle

    painter->setPen(QColor(181, 181, 181));
    painter->setBrush(Qt::NoBrush);
    painter->drawRoundedRect(r, 15, 15, Qt::RelativeSize);
    //Draws inner rectangle rect with rounded corners.

    painter->setPen(QColor(159, 159, 159));
    int y = rect.top() + rect.height() / 2 - 1;
    painter->drawLine(rect.left(), y, rect.right(), y);
    //Draws the mid-line from (rect.left(), y) to (rect.right(), y) and sets the current pen position to (rect.right(), y)
}

QPixmap Digit::drawDigits(int number, const QRect &rect)
{
    int scaleFactor = 2;

#if defined(Q_OS_SYMBIAN) || defined(Q_OS_WINCE_WM)
    if (rect.height() > 240)
    {
        scaleFactor = 1;
    }
#endif

    QString str = QString::number(number);
    if (str.length() == 1)
        str.prepend("0");
    //Ensure it is double-digit

    QFont font;
    font.setFamily("helvetica");
    font.setBold(true);
    font.setPixelSize(0.5 * scaleFactor * rect.height());
    //Sets the font size to pixelSize pixels

    QPixmap pixmap(rect.size() * scaleFactor);
    pixmap.fill(Qt::transparent);

    QLinearGradient gradient(QPoint(0, 0), QPoint(0, pixmap.height()));
    //Constructs a linear gradient with interpolation area between (0,0) and (0,pixmap.height())

    gradient.setColorAt(0.00, QColor(128, 128, 128));
    gradient.setColorAt(0.49, QColor(64, 64, 64));
    gradient.setColorAt(0.51, QColor(128, 128, 128));
    gradient.setColorAt(1.00, QColor(16, 16, 16));
    //Creates stop points at the given position with the given color

    QPainter painter;
    painter.begin(&pixmap);
    painter.setFont(font);

    QPen pen;
    pen.setBrush(QBrush(gradient));
    //Set penbrush with linergrident

    painter.setPen(pen);
    painter.drawText(pixmap.rect(), Qt::AlignCenter, str);
    //Draws the digit number(str here) within the provided rectangle

    painter.end();

    return pixmap.scaledToWidth(width(), Qt::SmoothTransformation);
    //Returns a scaled copy of the image which is transformed using bilinear filtering
}

/*prepare the pixmap  */
void Digit::preparePixmap()
{
    m_pixmap = QPixmap(size());
    m_pixmap.fill(Qt::transparent);
    //Fills the pixmap with the given transparent black value (i.e., QColor(0, 0, 0, 0))

    QPainter painter;
    painter.begin(&m_pixmap);
    painter.drawPixmap(0, 0, drawDigits(m_number, rect()));
    //Draws the given digits-pixmap at position (0, 0)

    painter.end();
}

void Digit::resizeEvent(QResizeEvent *)
{
    preparePixmap();
    update();//Causes a paintEvent() call
}

void Digit::paintStatic()
{
    QPainter painter(this);
    painter.fillRect(rect(), Qt::black);
    //Fill the widget rec with black color

    int pad = width() / 10;
    drawFrame(&painter, rect().adjusted(pad, pad, -pad, -pad));
    painter.drawPixmap(0, 0, m_pixmap);
}

void Digit::paintSlide()
{
    QPainter painter(this);
    painter.fillRect(rect(), Qt::black);

    int pad = width() / 10;
    QRect fr = rect().adjusted(pad, pad, -pad, -pad);
    drawFrame(&painter, fr);
    painter.setClipRect(fr);
    //sets the clip region to the given rectangle using the given clip operation

    int y = height() * m_animator.currentFrame() / 100;
    painter.drawPixmap(0, y, m_lastPixmap);
    //Draw last-time state pixmap from 0 to height()(Y Coordinate)  in 0.6 second

    painter.drawPixmap(0, y - height(), m_pixmap);
    //Draw current-time state pixmap from -height() to 0 (Y Coordinate) in 0.6 second
}

void Digit::paintFlip()
{
    QPainter painter(this);
#if !defined(Q_OS_SYMBIAN) && !defined(Q_OS_WINCE_WM)
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    painter.setRenderHint(QPainter::Antialiasing, true);
#endif
    painter.fillRect(rect(), Qt::black);

    int hw = width() / 2;
    int hh = height() / 2;

    // behind is the new pixmap
    int pad = width() / 10;
    QRect fr = rect().adjusted(pad, pad, -pad, -pad);
    drawFrame(&painter, fr);
    painter.drawPixmap(0, 0, m_pixmap);

    int index = m_animator.currentFrame();

    if (index <= 50)
    {
        // the top part of the old pixmap is flipping
        int angle = -180 * index / 100;
        QTransform transform;
        transform.translate(hw, hh);
        //Moves the coordinate system to the center of widget

        transform.rotate(angle, Qt::XAxis);
        //Rotates the coordinate system counterclockwise by angle about the X axis

        painter.setTransform(transform);
        drawFrame(&painter, fr.adjusted(-hw, -hh, -hw, -hh));
        painter.drawPixmap(-hw, -hh, m_lastPixmap);

        // the bottom part is still the old pixmap
        painter.resetTransform();
        painter.setClipRect(0, hh, width(), hh);
        //Enables clipping, and sets the clip region to the rectangle beginning at (0, hh) with the given width and height

        drawFrame(&painter, fr);
        painter.drawPixmap(0, 0, m_lastPixmap);
    }
    else
    {
        painter.setClipRect(0, hh, width(), hh);

        // the bottom part is still the old pixmap
        drawFrame(&painter, fr);
        painter.drawPixmap(0, 0, m_lastPixmap);

        // the bottom part of the new pixmap is flipping
        int angle = 180 - 180 * m_animator.currentFrame() / 100;
        QTransform transform;
        transform.translate(hw, hh);
        transform.rotate(angle, Qt::XAxis);
        painter.setTransform(transform);
        drawFrame(&painter, fr.adjusted(-hw, -hh, -hw, -hh));
        painter.drawPixmap(-hw, -hh, m_pixmap);
    }
}

void Digit::paintRotate()
{
    QPainter painter(this);

    int pad = width() / 10;
    QRect fr = rect().adjusted(pad, pad, -pad, -pad);
    drawFrame(&painter, fr);
    painter.setClipRect(fr);

    int angle1 = -180 * m_animator.currentFrame() / 100;
    int angle2 = 180 - 180 * m_animator.currentFrame() / 100;
    int angle = (m_animator.currentFrame() <= 50) ? angle1 : angle2;
    QPixmap pix = (m_animator.currentFrame() <= 50) ? m_lastPixmap : m_pixmap;

    QTransform transform;
    transform.translate(width() / 2, height() / 2);
    transform.rotate(angle, Qt::XAxis);

    painter.setTransform(transform);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    painter.drawPixmap(-width() / 2, -height() / 2, pix);
}

void Digit::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    if (m_animator.state() == QTimeLine::Running)
    {
        if (m_transition == Slide)
            paintSlide();
        if (m_transition == Flip)
            paintFlip();
        if (m_transition == Rotate)
            paintRotate();
    }
    else
    {
        paintStatic();
    }
}
