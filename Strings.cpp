#include "Strings.h"


namespace Strings
{
    QString qss_main=
    //全部
    "QWidget { background-color:rgba(0,0,0,0.0); color: #ffffff; font-size: 20px; font-family: consolas; border-radius:6px; }"
    "QWidget#top { background-color:#ffffff; }"
    //暂停菜单
    "QWidget#widget_menu { background-color: rgba(0,0,0,0.6); }"
    //顶层信息面板
    "QWidget#widget_game_info { font-size: 30px; border: 3px solid #ffffff; background-color: rgba(0,0,0,0.9); padding:5px; }"
    "QWidget#widget_game_info > * { font-size: 30px; background-color: rgba(0,0,0,0.0); }"
    //面板
    "QWidget#panel { background-color: rgba(0,0,0,0.8); }"
    //按钮
    "QPushButton { background-color: rgba(0,0,0,0.6); color: #ffffff; border: 2px solid #ffffff; width: 200px; height:50px; }"
    "QWidget:disabled { background-color: rgba(0,0,0,0.0); color: #999999; border: 2px solid #000000; }"
    //按钮hover
    "QPushButton::hover{ color:#000000; background-color: rgba(200,200,200,0.9); border: 2px solid #000000; }"
    //面板下
    "QWidget#panel > QListWidget { border: 2px solid #ffffff; }"
    "QWidget#panel > QListWidget::item { border: 2px rgba(0,0,0,0.0); border-bottom: 2px solid #555555; margin: 10px; padding: 10px; }"
    "QWidget#panel > QListWidget::item:selected { color: #ffffff; background-color: rgba(0,0,0,0.8); border-bottom: 2px solid #ffffff; }"
    "QWidget#panel > QTextBrowser { border: 2px solid #ffffff; }"
    "QWidget#panel > QPushButton { width: 400px; }"
    "QWidget#panel > QPushButton#refresh { width:200px; border: 2px solid #000000; }"
    //label背景色
    "QWidget QLabel#title{ background-color: rgba(0,0,0,0.8); color: #ffffff; padding:10px; }";
}
