#ifndef TOGGLEBUTTON_H
#define TOGGLEBUTTON_H

#include <QPushButton>
#include <QString>

class ToggleButton : public QPushButton
{
    Q_OBJECT

public:
    ToggleButton(QWidget * parent = 0);
    ~ToggleButton();
    void setNormalImage(QString strRes);
    void setPressedImage(QString strRes);

protected:
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void paintEvent(QPaintEvent *event);

private:
    bool m_isPressed;
    QString m_strImgNormal, m_strImgPressed;
}; // ToggleButton

#endif // TOGGLEBUTTON_H

