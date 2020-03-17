#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QString>
#include <QLabel>
#include <QtWidgets>
#include <QPushButton>
#include <QMessageBox>

#include "GamingWidget.h"

using namespace Core::Objects;

class GamingWidget;//前导声明
class StatusBarInfo;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

public:
    //菜单栏
    QMenuBar * menu_bar{nullptr};

    QMenu * menu_control{nullptr};//控制菜单
    QMenu * menu_option{nullptr};//选项菜单
    QMenu * menu_other{nullptr};//帮助菜单

    QAction * action_start{nullptr};//开始
    QAction * action_quit{nullptr};//退出
    QAction * action_debug{nullptr};//调试

    QAction * action_toggle_display_gaming_area{nullptr};//切换显示


    QStackedWidget * widget_main_stacked{nullptr};//主组件

    QWidget * widget_page0{nullptr};
    QGridLayout * layout_grid_widget_page0{nullptr};

    QStatusBar * status_bar{nullptr};//状态栏

    QLabel * label_mouse_pos{nullptr};//鼠标坐标
    QLabel * label_num_updates{nullptr};//更新数量
    QLabel * label_num_objects{nullptr};//对象数量
    QLabel * label_time_cosumption{nullptr};//时间消耗

    QTextBrowser * text_browser{nullptr};//文本浏览器(显示控制台信息)

    QString text{"Hallo World!"};//文本
//    QTextEdit * text_edit{nullptr};

    GamingWidget * widget_gaming{nullptr};//游戏组件

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

    //初始化组件
    void init_components();
    //初始化界面
    void init_UI();
    //初始化信号槽
    void init_signal_slots();

public:
    void closeEvent(QCloseEvent *)override;

public slots:
    void push_info(const QString &message);//推送信息
    void update_status_info(StatusBarInfo *info);//更新状态栏信息

};





#endif // MAINWINDOW_H

