#ifndef BUTTON_H
#define BUTTON_H

#include <QPushButton>
#include <QDebug>

#include "Core.h"

class Button : public QPushButton
{
//    using QPushButton::QPushButton;
public:
    Button(const QIcon &icon, const QString &text, QWidget *parent = nullptr);
    Button(const QString &text, QWidget *parent = nullptr);
    explicit Button(QWidget *parent = nullptr);


    void format();
};

#endif // BUTTON_H
