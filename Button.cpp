#include "universal.h"


QSoundEffect * Button::sound_hover{nullptr};
QSoundEffect * Button::sound_click{nullptr};

Button::Button(const QIcon &icon, const QString &text, QWidget *parent)
    :QPushButton(icon,text,parent)
{
    format();
}

Button::Button(const QString &text, QWidget *parent)
    :QPushButton(text,parent)
{
    format();
}

Button::Button(QWidget *parent)
    :QPushButton(parent)
{
    format();
}

void Button::format()
{
    this->setFocusPolicy(Qt::FocusPolicy::NoFocus);

    //连接信号槽
    if(Button::sound_hover)
        connect(this,&Button::hovered,sound_hover,&QSoundEffect::play);
    if(Button::sound_click)
        connect(this,&Button::hovered,sound_click,&QSoundEffect::play);
}

void Button::enterEvent(QEvent *event)
{
    Q_UNUSED(event)
    emit hovered();//发出信号
}


