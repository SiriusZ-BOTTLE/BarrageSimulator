#include "universal.h"



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
}


