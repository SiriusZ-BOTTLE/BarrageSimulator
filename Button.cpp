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
//    this->setFixedSize(Settings::width_button,Settings::height_button);
//    this->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


