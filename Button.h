#ifndef BUTTON_H
#define BUTTON_H

#include <QPushButton>
#include <QDebug>

#include <QSound>
#include <QSoundEffect>


class Button : public QPushButton
{
//    using QPushButton::QPushButton;
    Q_OBJECT
private:
    //全局音效, 自动连接信号槽
    static QSoundEffect * sound_hover;
    static QSoundEffect * sound_click;


public:
    Button(const QIcon &icon, const QString &text, QWidget *parent = nullptr);
    Button(const QString &text, QWidget *parent = nullptr);
    explicit Button(QWidget *parent = nullptr);

    //初始化
    void initialize();

signals:
    void hovered();//hover信号

public:
    //鼠标进入事件
    void enterEvent(QEvent *event)override;

    //设置音效(设置后按钮在构造时会自动连接信号)
    static void set_hover_sound(QSoundEffect *sound_hover);
    //设置音效(设置后按钮在构造时会自动连接信号)
    static void set_click_sound(QSoundEffect *sound_click);


};

#endif // BUTTON_H






