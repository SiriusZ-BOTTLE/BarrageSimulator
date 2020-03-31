#ifndef GAMINGWIDGET_H
#define GAMINGWIDGET_H


#include "Core.h"
#include "InnerData.h"
#include "Graphics.h"
#include "Button.h"


using namespace Core;
using namespace InnerData;

///全局变量, 线程共享
//运行时数据(包括场景中的所有对象, 按键状态等信息)
extern RunTimeData data_runtime;
extern GraphicsScene * scene_main;//主场景
extern GraphicsView * view_main;//视图

//前导声明
class MainWindow;

///状态栏信息
struct StatusBarInfo
{
    QPoint pos_mouse{0,0};//鼠标位置
    Integer num_updates{0};//刷新数
    Integer num_objects{0};//对象数
    Integer time_consumption{0};//时间消耗(ns为单位)
    Integer time_consumption_average{1};//平均时间消耗(ns为单位)
};


///对象控制线程
class ObjectsControl: public QObject
{
    Q_OBJECT
public:
    int count{0};

    enum Status//!废弃
    {
        Running,//运行
        Pause,//暂停
        Over,//结束
        End_Status//哨兵
    };


public:
    Status status{Pause};
    //处理数据
    static void process_data();
    //管理对象对象
    static void manage_objects();
    //更新属性
    static void update_property();

private:
    //派生对象
    static void derive_object(const ObjectControlProperty &pro, DeriveRule *rule);
//    static void destroy_object(FlyingObject * ptr);//销毁对象

signals:
    void updated();//更新信号
    void push_info(const QString &info);

};

///主组件
class GameWidget : public QWidget
{
    Q_OBJECT
public:
    enum class Status
    {
        Running,//运行
        Pause,//暂停
        Over//结束
    };

    enum Page{
        TitlePage,//标题页
        StartPage,//开始页
        GamePage,//游戏页
        OptionPage,//选项页
    };

public:
    Status status{Status::Running};//运行状态

    Integer num_updates{0};//总画面更新
    Integer cooldown_next_data_update{0};//下一次数据更新
    Integer time_consumption_total{0};//总时间消耗


    ///主要widget
    QStackedWidget *widget_main{nullptr};//主组件
    QWidget * widget_title{nullptr};//标题页
    QWidget * panel_title{nullptr};//标题页面板
    QWidget * widget_start{nullptr};//开始页
    QWidget * panel_start{nullptr};//开始页面板
    QWidget * widget_menu{nullptr};//暂停菜单页

    QLabel * label_overlay{nullptr};//覆盖信息显示层
    QLabel * label_info_esc_menu{nullptr};
    QLabel * label_title{nullptr};//标题
    QLabel * label_bottom_info_title{nullptr};//标题页底部信息

    GraphicsScene * scene_title{nullptr};//标题页背景场景
    GraphicsView * view_title{nullptr};//标题页背景视图
    QGraphicsPixmapItem bg;//图片项

    QListWidget *list_widget_start{nullptr};//开始页列表组件

    ///布局
    QGridLayout * layout_widget_menu{nullptr};//暂停菜单布局
    QGridLayout * layout_main{nullptr};//主布局
    QGridLayout * layout_title{nullptr};//标题页布局
    QGridLayout * layout_start{nullptr};//开始页布局
    QGridLayout * layout_panel_title{nullptr};//标题页面板布局
    QGridLayout * layout_panel_start{nullptr};//开始页面板布局

    ///标题页按钮
    Button * button_start{nullptr};//开始
    Button * button_load{nullptr};//加载
    Button * button_options{nullptr};//选项
    Button * button_exit{nullptr};//退出

    Button * button_back_start{nullptr};//开始页返回按钮


    ///暂停菜单按钮
    Button * button_resume_pause_menu{nullptr};
    Button * button_exit_pause_menu{nullptr};

    //线程对象
    ObjectsControl * object_thread_data_process{nullptr};
//    ObjectsControl * object_thread_objects_management{nullptr};//废弃, 不使用

    //线程
    QThread thread_data_process{};//数据处理线程
//    QThread thread_objects_management{};//对象管理线程

    QTimer timer;//定时器
    QTimer timer_title;//标题页定时器

    StatusBarInfo info_status_bar{};

    //背景图片集合
    QVector<QPixmap> images_title{};
    //速度
    BinaryVector<Decimal> speed_bg_moving{0,0};

    QGraphicsBlurEffect effect_blur;

    //指向控制台widget
    MainWindow * main_window{nullptr};

    int mouse_delay{0};//鼠标延迟

public:

public:
    //构造函数
    explicit GameWidget(MainWindow *_main_window = nullptr);
    ~GameWidget() override;

private:
    //初始化组件
    void init_components();
    //初始化界面
    void init_UI();
    //初始化信号槽
    void init_signal_slots();
    //初始化线程
    void init_threads();
    //加载标题页背景图片
    void load_title_images();

    //按键处理
    void key_process();
    //更新基本属性
    void update_property();
    //退出键
    void esc();

    //清除
    void clear();
    //派生对象
//    void derive_objects_self();
    //计算属性
//    void process_data_self();

public:
    //加载场景
    void load_scene();
    //初始化
    void initialize();
    //全部重置
    void reset();

    //启动
    void start();

    //测试用函数
    void test();
    void exec();

signals:
    void signal_push_info(const QString &string);//推送信息信号
    void signal_send_status_bar_info(StatusBarInfo *);//更新状态栏信息信号

    void signal_process_data();//更新数据 信号 (不使用)
    void signal_manage_objects();//管理对象(派生, 删除) 信号(不使用)

public slots:
    //刷新所有内容, 页面刷新
    void update();
    //更新背景图位置
    void update_bg_image_position();



    //按钮槽
    void goto_start_page();//跳转到start页
    void goto_title_page();//跳转到title页



protected:
    //按键按下事件
    void keyPressEvent(QKeyEvent *event) override;
    //按键释放事件
    void keyReleaseEvent(QKeyEvent *event) override;
    //鼠标按下事件
    void mousePressEvent(QMouseEvent *event) override;
    //鼠标释放事件
    void mouseReleaseEvent(QMouseEvent *event) override;
    //窗体大小变更事件
    void resizeEvent(QResizeEvent *event) override;
    //窗口移动事件
//    void moveEvent(QMoveEvent *event) override;

};



#endif // GAMINGWIDGET_H
