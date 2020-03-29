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

///gaming组件
class GamingWidget : public QWidget
{
    Q_OBJECT
public:
    enum class Status
    {
        Running,//运行
        Pause,//暂停
        Over//结束
    };

public:
    Status status{Status::Running};//运行状态

    Integer num_updates{0};//总画面更新
    Integer cooldown_next_data_update{0};//下一次数据更新
    Integer time_consumption_total{0};//总时间消耗

    QGridLayout * layout_gird_main{nullptr};//主布局

    GraphicsView * view_main;//视图
    //运行时数据
//    RunTimeData data_runtime;

    //覆盖信息显示层
    QLabel * label_overlay{nullptr};
    QLabel * label_info_esc_menu{nullptr};

    QWidget * widget_menu{nullptr};
    QGridLayout * layout_widget_menu{nullptr};

    Button * button_resume{nullptr};
    Button * button_exit{nullptr};

    //线程对象
    ObjectsControl * object_thread_data_process{nullptr};
    ObjectsControl * object_thread_objects_management{nullptr};//废弃, 不使用

    //线程
    QThread thread_data_process{};//数据处理线程
//    QThread thread_objects_management{};//对象管理线程

    QTimer timer;//定时器

    StatusBarInfo info_status_bar{};

    MainWindow * main_window{nullptr};

    int mouse_delay{0};//鼠标延迟

public:

public:
    //构造函数
    explicit GamingWidget(MainWindow *_main_window = nullptr);
    ~GamingWidget() override;

private:
    //初始化组件
    void init_components();
    //初始化界面
    void init_UI();
    //初始化信号槽
    void init_signal_slots();
    //初始化线程
    void init_threads();

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

protected:
    //按键按下事件
    void keyPressEvent(QKeyEvent *event) override;
    //按键释放事件
    void keyReleaseEvent(QKeyEvent *event) override;
    //鼠标按下事件
    void mousePressEvent(QMouseEvent *event) override;
    //鼠标释放事件
    void mouseReleaseEvent(QMouseEvent *event) override;

};



#endif // GAMINGWIDGET_H
