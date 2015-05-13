#ifndef DIGIT_H
#define DIGIT_H

#include <QWidget>
#include <QTimeLine>
#include <QPainter>

class Digit : public QWidget
{
    Q_OBJECT
public:
    typedef enum
    {
        Slide,
        Flip,
        Rotate
    }transition_t;//Define three transition modes of the digital clock

    explicit Digit(int transition, int number = 0, QWidget *parent = 0);

    void setTransition(int transition);//Set transition when time changed
    int transition() const;//Get transition mode
    void setNumber(int number);//Set hours and minutes
    void transitTo(int number);//Transit to next state

protected:
    void drawFrame(QPainter *painter, const QRect &rect);//Draw the main frame of the digits area
    QPixmap drawDigits(int number, const QRect &rect);//Draw the digits
    void preparePixmap();//prepare the pixmap
    void paintStatic();//Paint the static state
    void paintSlide();//Paint the slide state
    void paintFlip();//Paint the flip state
    void paintRotate();//Paint the rotate state
    void resizeEvent(QResizeEvent *);//define a resize event
    void paintEvent(QPaintEvent *event);//define a paint event

private:
    int m_number;//number to set to digits
    int m_transition;//transition mode(change effect)
    QPixmap m_pixmap;//current time pixmap
    QPixmap m_lastPixmap;//next state time pixmap
    QTimeLine m_animator;
    //used to animate a GUI control by calling a slot periodically
    //The timeline's duration describes for how long the animation will run
    //connect the frameChanged() signal to a suitable slot in the widget you wish to animate
};

#endif // DIGIT_H
