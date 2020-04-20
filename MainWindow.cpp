#include "universal.h"



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    //初始化组件
    init_components();
    //初始化界面
    init_UI();
    //初始化信号槽
    init_signal_slots();

    //初始化程序内置数据
//    InnerData::init_inner_data();
}

MainWindow::~MainWindow()
{
    delete widget_gaming;//释放gaming widget

    release_inner_data();//释放内部数据
}

void MainWindow::init_components()
{
    //创建, 获取并添加菜单栏
    menu_bar=this->menuBar();
    //创建菜单
    menu_control=new QMenu("Control(&C)");
    menu_option=new QMenu("Option(&O)");
    menu_other=new QMenu("Other(&T)");

    //创建action
    action_start=new QAction("Start");
    action_quit=new QAction("Quit");
    action_debug=new QAction("TEST");


    widget_main_stacked=new QStackedWidget();

    widget_page0=new QWidget();
    layout_grid_widget_page0=new QGridLayout();

    //创建, 获取并添加状态栏
    status_bar=this->statusBar();
    label_mouse_pos=new QLabel();
    label_num_updates=new QLabel();
    label_num_objects=new QLabel();
    label_time_cosumption=new QLabel();

    text_browser=new QTextBrowser();

    widget_gaming=new GameWidget(this);
//    widget_gaming->show();

}

void MainWindow::init_UI()
{
    this->setWindowTitle("Console");
    this->setMinimumSize(Settings::width_interface,Settings::height_interface);
    //设置中心组件
    this->setCentralWidget(widget_main_stacked);


    widget_main_stacked->addWidget(widget_page0);
    widget_page0->setLayout(layout_grid_widget_page0);

    layout_grid_widget_page0->addWidget(text_browser);

    //菜单栏添加菜单
    menu_bar->addMenu(menu_control);
    menu_bar->addMenu(menu_option);
    menu_bar->addMenu(menu_other);

    //菜单添加项
    menu_control->addAction(action_start);
    menu_control->addSeparator();
    menu_control->addAction(action_quit);

    menu_option->addAction(action_debug);

    text_browser->append("hallo world!");

    //状态栏添加组件
    status_bar->addWidget(label_mouse_pos);
    status_bar->addWidget(label_num_updates);
    status_bar->addWidget(label_num_objects);
    status_bar->addWidget(label_time_cosumption);

    this->setStyleSheet
    (
        "QWidget > QStatusBar > QLabel { font-family:consolas; font-style: bold }"
        "QWidget > QTextBrowser { color:#EEEEEE; background-color:#000000; }"
    );

}

void MainWindow::init_signal_slots()
{
    //action
    connect(action_debug,&QAction::triggered,this->widget_gaming,&GameWidget::test);
    connect(action_start,&QAction::triggered,this->widget_gaming,&GameWidget::start);
    //鼠标位置更新
    connect(widget_gaming,&GameWidget::signal_send_status_bar_info,this,&MainWindow::update_status_info);

}

void MainWindow::closeEvent(QCloseEvent * event)
{
    //退出应用程序
    auto result=QMessageBox::question(this,"EXIT...","Are you sure to EXIT?   ");
    if(result==QMessageBox::Yes)
        QApplication::quit();//退出程序
    else
        event->ignore();//忽略事件
}

void MainWindow::push_info(const QString &message)
{
//    qDebug()<<"push_info()";
    static long long count;
    //推送消息
    this->text_browser->append(message);
    count++;
    if(count%1000==0)
        text_browser->clear();
}

void MainWindow::update_status_info(StatusBarInfo *info)
{
    //显示坐标
    label_mouse_pos->setText(QString::asprintf("(%4.0f, %4.0f)",info->pos_mouse.x(),info->pos_mouse.y()));
    //显示更新数
    label_num_updates->setText(QString::asprintf("updates:%-5lld",info->num_updates));
    //显示对象数
    label_num_objects->setText(QString::asprintf("objects:%-4lld",info->num_objects));
    //时间消耗
    label_time_cosumption->setText
            (QString::asprintf("%5lld/%5lld/%4.1f",info->time_consumption,info->time_consumption_average,1000000.0/info->time_consumption_average));
}










