#ifndef CORE_H
#define CORE_H

//标准库
#include <iostream>
//QT核心库
#include <QString>
#include <QVector>
#include <QGraphicsItem>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QPointF>
#include <QDebug>
#include <QThread>
#include <QTimer>
#include <QEventLoop>
#include <QtCore/qmath.h>
#include <cmath>
#include <chrono>
#include <QMetaType>
#include <QLabel>
#include <QGridLayout>
#include <QTime>
#include <QMutex>//互斥量
#include <QMutexLocker>//RAII
#include <QReadWriteLock>//读写锁
#include <QReadLocker>//RAII
#include <QWriteLocker>//RAII
//json
#include <QJsonArray>
#include <QJsonObject>

#include <QOpenGLContext>
#include <QOpenGLBuffer>
#include <QGL>


#if defined(_WIN32)
#if defined(_MSC_VER)
// Windows平台  Visual Statuio 编译器特定代码
#endif
#if defined(__GNUC__)
// Windows平台 GCC编译器特定的代码
#define DBL_EPSILON __DBL_EPSILON__
#endif
#endif




/*
 *  核心头文件
 */


//弹幕模拟器
namespace Core
{
    //前导声明
    namespace Objects
    {
        class FlyingObject;
        class HitObject;
    }


    ///定义集合
    namespace Definition
    {
    using Decimal = double;//浮点数
    using Integer = long long;//整数
//    using BinaryVector =std::pair<Decimal,Decimal>;//二元向量

    template<typename type>
    using BinaryVector =std::pair<type,type>;
//    using ptr=type*;


    constexpr Decimal PI=3.1415926535897;
    constexpr Decimal R2D=180/PI;//弧度转角度
    constexpr Decimal D2R=PI/180;//角度转弧度

    ///逻辑按键
    enum Key
    {
        //Player0
        P0_UP,
        P0_DOWN,
        P0_LEFT,
        P0_RIGHT,
        P0_FIRE,//fire
        P0_SP,//特殊
        P0_IACT,//交互(interaction)
        //Player1
        P1_UP,
        P1_DOWN,
        P1_LEFT,
        P1_RIGHT,
        P1_FIRE,//fire
        P1_SP,//特殊
        P1_IACT,//交互
        //其它按键
        ML,//鼠标左键
        MR,//鼠标右键
        ESC,//退出键
        None,//无键位
        End_Key=None//哨兵
    };

    ///位移模式
    enum class MovementMode
    {
        Stop,//自动停止(立即消除加速度大小, 自动速度衰减)
        TowardsTarget,//朝向目标(自动位移至目标位置)
        Unlimited//无限制(使用加速度, 速度等物理量计算位置)
    };

    ///旋转模式
    enum class RotationMode
    {
        Fixed,//固定方向(不能旋转)
        Stop,//自动停止(立即消除角加速度大小, 自动角速度衰减)
        FollowSpeed,//跟随速度(始终保持与速度方向一致)
        FollwoAcceleration,//跟随加速度(始终保持与加速度方向一致)
        TowardsMouse,//指向鼠标(自动朝向鼠标方向)
        TowardsTarget,//指向目标(自动指向目标坐标)
        Unlimited//无限制(使用角加速度, 角速度等物理量计算旋转角度)
    };

    class DeriveRule;

    ///对象控制属性
    class ObjectControlProperty//聚合类
    {
    public:
        Integer lifetime{-1};//生命周期(小于0时持续存在, 等于0时会被释放)
        bool flag_end{false};//生命周期结束标记
        bool flag_delete_outside_scene{true};//是否在场景外应该被释放的标记
        bool flag_boundary_restriction{false};//边界限制
        bool flag_collision{true};//是否考虑碰撞的标记
        bool flag_drive{false};//是否处于可以派生的状态
        int cooldown_highlight{0};//高亮冷却, 到0的时候取消高亮效果
        int cooldown_drive{0};//派生冷却, 到0的时候才能继续派生
        DeriveRule * rule{nullptr};//生成规则

        Integer count_drive{0};//派生次数

        Decimal mass{1};//质量(用于计算碰撞后速度)
        BinaryVector<Decimal> coordinate{0,0};//坐标(横坐标, 纵坐标)
        BinaryVector<Decimal> target_aming{0,0};//指向目标
        BinaryVector<Decimal> target_moving{0,0};//移动目标
        ///位移运动相关
        MovementMode mode_movement{MovementMode::Stop};//默认自动停止

        //速度
        Decimal velocity_max{0};//最大速率限制
        BinaryVector<Decimal> speed_polar{0,0};//极坐标(速度大小, 速度方向)(弧度制)
        BinaryVector<Decimal> speed_axis{0,0};//轴坐标(水平速度, 垂直速度)
        Decimal acceleration_max{0};//最大加速度限制(player操纵对象运动时默认使用最大加速度进行加速)
        Decimal attenuation_velocity{0};//速度衰减(减速时的加速度大小)
        //加速度
        BinaryVector<Decimal> acceleration_polar{0,0};//极坐标(加速度大小, 加速度方向)(弧度制)
        BinaryVector<Decimal> acceleration_axis{0,0};//轴坐标(水平加速度, 垂直加速度)

        ///刚体(定轴)旋转相关(角度制)
        RotationMode mode_rotation{RotationMode::Fixed};//旋转模式(默认固定位置不旋转)

        Decimal coefficient_friction{1};//摩擦系数
        BinaryVector<Decimal> angular_initial_target{0,0};//初始角度, 目标旋转角度
        BinaryVector<Decimal> angular_speed_max{0,0};//角速度, 最大角速度
        BinaryVector<Decimal> angular_acc_max{0,0};//角加速度, 最大角加速度
        BinaryVector<Decimal> rotation{0,0};//当前旋转角度, 惯性旋转角度
    public:
        //构造函数
        ObjectControlProperty()=default;
        //复制构造函数
        ObjectControlProperty(const ObjectControlProperty&)=default;
        //初始化, 设置数据后必须手动初始化
        void init();
        //计算惯性旋转角(全力减速时仍然会滑行的角度)
        void get_inertial_rotation_angle();

    };
    using OBP=ObjectControlProperty;

    ///gaming数值属性
    class ObjectGamingProperty//聚合类
    {
    public:
        bool flag_team_kill{false};//同队伍伤害
        int team{0};//队伍号
        int index_bullet_series{0};//系列索引
        int index_bullet_level{0};//等级索引
        BinaryVector<Decimal> endurance{0,0};//耐久/最大耐久
        Decimal damage{0};//伤害
        Decimal resist{0};//抵抗/阻挡
        Decimal penetrability{0};//穿透

    };
    using OGP=ObjectGamingProperty;

    ///方向参照
    enum class DirectionReference
    {
        RelativeToParentRotation,//相对父对象方向
        RelativeToParentSpeed,//相对父对象速度方向
        RelativeToParentAcc,//相对父对象加速度方向
        Absolute//绝对方向
    };
    using DR=DirectionReference;


    ///派生单元
    //描述一次物体生成相对于父对象的具体细节
    class DeriveUnit
    {
    public:
        Objects::FlyingObject * p{nullptr};//欲放置的对象指针
        //方向参照(该模式定义了放置时的初始角度, 速度, 加速度的基本参照)
        DR ref_direction{DR::RelativeToParentRotation};//(默认以基对象的朝向为参照)
        bool flag_inherit_speed{true};//继承父对象速度(继承的速度是相对于场景的)
        bool flag_relative_position{true};//相对位置
        BinaryVector<Decimal> pos{0,0};//初始位置
        BinaryVector<Decimal> speed{0,0};//初始速度(极坐标, 若大小小于0, 则使用最大速度)
        BinaryVector<Decimal> acceleration{0,0};//初始加速度(极坐标, 若大小小于0, 则使用最大加速度)
        Decimal rotation{0.0};//初始角度
        Decimal rotation_float{0};//方向浮动百分比
    };

    ///派生规则
    class DeriveRule
    {
    public:
        int period;//派生最速周期
        QVector<DeriveUnit> units;
    };

    //对象类型
    //一共三种类型
    enum class ObjectType
    {
        FlyingObject,//飞行对象
        HitObject,//碰撞对象
        ManipulableObject,//可操纵对象
        End_ObjectType//哨兵
    };
    }

    ///工具函数
    namespace ToolFunctions
    {
    using namespace Definition;

        //返回一个[0,1]区间内的浮点数
        Definition::Decimal get_random_decimal_0_1();
        //极坐标到轴坐标
        void polar_to_axis(const BinaryVector<Decimal>& polar,BinaryVector<Decimal>& axis);
        //轴坐标到极坐标
        void axis_to_polar(const BinaryVector<Decimal>& axis,BinaryVector<Decimal>& polar);
    }

    using namespace Definition;

    namespace Objects
    {
//    using namespace Definition;
    using Definition::ObjectType;
    using Definition::ObjectControlProperty;
    using Definition::ObjectGamingProperty;

    class FlyingObject;
    ///元素, 屏幕上的基本显示单元
    class Element: public QGraphicsPixmapItem
    {
    public:
        FlyingObject * parent{nullptr};//指向本对象的管理对象
        int number_frame{0};//帧数
        int period_frame{0};
        int cooldown_next_frame{0};//下一帧冷却
        int frame_current{0};//当前帧
        QVector<QPixmap> frames{};//从原图上剪切下来的不同帧, 作为动画播放
    public:
        //构造函数, 指定图片, 指定帧数, 周期
        Element(const QPixmap& pixmap ,const int& number_frame=1,const int &period_frame=2);
        //复制构造函数(复制构造时不会复制父对象指针)
        Element(const Element&ano);
        void next_frame();//显示下一帧
    };

    ///飞行对象
    //可以是组, 可以是单个元素, 支持嵌套
    class FlyingObject
    {
    public:
        QString name{"Undefined"};//名称
        QGraphicsItem * item{nullptr};//项指针或者组指针
        ObjectType type{ObjectType::FlyingObject};//飞行对象类型
        bool flag_group{false};//是否为群组的标记

        ObjectControlProperty property{};//基本属性
        QList<FlyingObject*> items_sub{};//次级对象

    public:
        //默认构造函数
        FlyingObject();
        //构造函数, 指定名称
        explicit FlyingObject(const QString& name);
        //析构函数(虚)
        virtual ~FlyingObject();
        //复制构造函数
        FlyingObject(const FlyingObject&ano);
    public:
        //添加到场景
        void add_to_scene(QGraphicsScene *scene);
        //设置元素
        void set_element(Element*ele);
        //添加次级对象
        void add_sub_objects(FlyingObject *obj_sub);
        //设置

        void set_derive_rule(DeriveRule * rule);
    };

    ///碰撞对象: 公有继承 飞行对象
    class HitObject: public FlyingObject
    {
    public:
        using FlyingObject::FlyingObject;//使用基类的全部构造函数
        ObjectGamingProperty property_gaming{};//gaming状态属性
    public:
        //构造函数
        HitObject();
        //复制构造函数
        HitObject(const HitObject&ano);
    };

    //可操纵对象
    using ManipulableObject=HitObject;

    }

    using namespace Core::Objects;

    ///运行时数据
    class RunTimeData
    {
    public:
        ManipulableObject *p1{nullptr};//player1
        ManipulableObject *p2{nullptr};//player2
//        ManipulableObject *p3{nullptr};//player3
//        ManipulableObject *p4{nullptr};//player3

        QPointF pos_mouse_scene{0,0};//场景鼠标坐标

        QList<Objects::FlyingObject*> list_objects{};//对象列表, 维护所有飞行对象

        QMutex mutex{};//互斥量

        QReadWriteLock lock{};//读写锁


        //按键状态
        bool status_keys[End_Key]{false};
    public:
        //构造函数
        RunTimeData();

        RunTimeData(const RunTimeData&ano)=delete;
        //析构函数
        ~RunTimeData()=default;
    };


}

//全局设置
namespace Settings
{
using namespace Core;

    extern int interval;//两次画面刷新之间的间隔(ms做单位)
    extern Definition::Integer period_data_update;//周期(两次数据更新之间的间隔数)
    extern bool flag_highlight;//受击高亮
    extern Definition::Integer time_highlight;//高亮持续刻数
    extern bool flag_music;//音乐开关
    extern bool flag_sound;//音效开关

    extern int width_interface;//界面宽度
    extern int height_interface;//界面高度

    extern int width_gaming;//游戏区域宽度
    extern int height_gaming;//游戏区域高度

    extern int width_button;//按钮统一宽度
    extern int height_button;//按钮统一高度

    extern int width_gaming_menu;//菜单宽度

    extern int count_frames;//统计平均时统计的帧数

    extern int size_font;//字体大小

    //键盘映射(键码映射), 通过映射到功能键位上
    extern QMap<unsigned,Definition::Key> map_keys;

    //重置键位
    extern void reset_key_map();


}








#endif // CORE_H
