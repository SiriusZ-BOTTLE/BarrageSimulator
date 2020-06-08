#include "universal.h"


QSoundEffect * Button::sound_hover{nullptr};
QSoundEffect * Button::sound_click{nullptr};

Button::Button(const QIcon &icon, const QString &text, QWidget *parent)
    :QPushButton(icon,text,parent)
{
    initialize();
}

Button::Button(const QString &text, QWidget *parent)
    :QPushButton(text,parent)
{
    initialize();
}

Button::Button(QWidget *parent)
    :QPushButton(parent)
{
    initialize();
}

void Button::initialize()
{
    this->setFocusPolicy(Qt::FocusPolicy::NoFocus);

    void (*p_f)(const QString &);
    p_f=&QSound::play;

    void (QSound::*p_f2)();
    p_f2=&QSound::play;

    //连接信号槽
    if(Button::sound_hover)
        connect(this,&Button::hovered,sound_hover,static_cast<void (QSoundEffect::*)()>(&QSoundEffect::play));
    if(Button::sound_click)
        connect(this,&Button::clicked,sound_click,static_cast<void (QSoundEffect::*)()>(&QSoundEffect::play));
}

void Button::enterEvent(QEvent *event)
{
    Q_UNUSED(event)
    emit hovered();//发出信号
}

void Button::set_hover_sound(QSoundEffect *sound_hover)
{
    Button::sound_hover=sound_hover;
}

void Button::set_click_sound(QSoundEffect *sound_click)
{
    Button::sound_click=sound_click;
}







