#ifndef BUTTON_H
#define BUTTON_H

#include <QPushButton>
#include <QDebug>

#include <QSoundEffect>


class Button : public QPushButton
{
//    using QPushButton::QPushButton;

public:
    //全局音效, 自动连接信号槽
    static QSoundEffect * sound_hover;
    static QSoundEffect * sound_click;


public:
    Button(const QIcon &icon, const QString &text, QWidget *parent = nullptr);
    Button(const QString &text, QWidget *parent = nullptr);
    explicit Button(QWidget *parent = nullptr);


    void format();

signals:
    void hovered();

public:
    //鼠标进入事件
    void enterEvent(QEvent *event)override;



};

#endif // BUTTON_H
