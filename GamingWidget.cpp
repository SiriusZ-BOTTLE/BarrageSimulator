#include "GamingWidget.h"

///全局变量定义
RunTimeData data_runtime{};
GraphicsScene *scene_main{nullptr};

void ObjectsControl::process_data()
{
    //    QReadLocker(&data_runtime.lock);//读锁定

    QWriteLocker(&data_runtime.lock); //写锁定

    QMap<FlyingObject *, bool> map;//存储已经被处理过碰撞的对象指针

    for (int index = 0; index < data_runtime.list_objects.size(); ++index)
    {
        //        bool flag_max_velocity{false};//满速标记
        ///坐标计算
        FlyingObject *p_object = data_runtime.list_objects[index];
        auto &pro = p_object->property;         //物理属性
        auto &acc_p = pro.acceleration_polar;   //加速度_极坐标
        auto &acc_a = pro.acceleration_axis;    //加速度_轴坐标
        auto &v_p = pro.speed_polar;            //速度_极坐标
        auto &v_a = pro.speed_axis;             //加速度_轴坐标
        auto &pos = pro.coordinate;             //位置
        auto &att_v = pro.attenuation_velocity; //速度衰减
        auto &pos_mouse = data_runtime.pos_mouse_scene;

        //        emit push_info(QString::asprintf("<acc_p> %.0f %.0f <acc_a> %.0f %.0f <v_p> %.0f %.0f <v_a> %.0f %.0f",acc_p.first,acc_p.second,acc_a.first,acc_a.second,v_p.first,v_p.second,v_a.first,v_a.second));
        //        qDebug()<<QString::asprintf("<acc_p> %f %f <acc_a> %f %f <v_p> %f %f <v_a> %f %f",acc_p.first,acc_p.second,acc_a.first,acc_a.second,v_p.first,v_p.second,v_a.first,v_a.second);

        if (pro.mode_movement == MovementMode::Stop) //自动停止
        {
            acc_a.first = acc_a.second = acc_p.first = acc_p.second = 0; //清除加速度
            v_p.second -= att_v;                                         //获取当前速度大小减去速度衰减
            if (v_p.second < 0)                                          //如果速度大小为负数
                v_p.second = 0;                                          //设为0
            //根据极坐标计算轴坐标速度
            ToolFunctions::polar_to_axis(v_p, v_a);

            //根据速度更新位置
            pos.first += v_a.first;   //水平位移
            pos.second += v_a.second; //垂直位移
        }
        else if (pro.mode_movement == MovementMode::Unlimited) //无限制
        {
            //根据极坐标计算轴坐标加速度
            ToolFunctions::polar_to_axis(acc_p, acc_a);

            //根据极坐标计算轴坐标速度
            ToolFunctions::polar_to_axis(v_p, v_a);

            //计算加速后的轴坐标速度
            v_a.first += acc_a.first;
            v_a.second += acc_a.second;

            //根据轴坐标更新极坐标速度
            ToolFunctions::axis_to_polar(v_a, v_p);

            if (pro.velocity_max > 0 && v_p.second > pro.velocity_max) //最大速度限制
            {
                v_p.second = pro.velocity_max;
                //重新计算轴坐标
                ToolFunctions::polar_to_axis(v_p, v_a);
            }

            //根据速度更新位置
            pos.first += v_a.first;   //水平位移
            pos.second += v_a.second; //垂直位移
        }
        if (pro.flag_boundary_restriction) //边界检查
        {
            if (pos.first < 0)
            {
                pos.first = 0;
                v_a.first = 0;
            }
            if (pos.first > Settings::width_gaming)
            {
                pos.first = Settings::width_gaming;
                v_a.first = 0;
            }
            if (pos.second < 0)
            {
                pos.second = 0;
                v_a.second = 0;
            }
            if (pos.second > Settings::height_gaming)
            {
                pos.second = Settings::height_gaming;
                v_a.second = 0;
            }
        }
        ToolFunctions::axis_to_polar(v_a, v_p);

        ///角度计算
        switch (pro.mode_rotation)
        {
        case RotationMode::Fixed: //固定无法旋转
            break;
        case RotationMode::FollowSpeed: //跟随速度
        {
            if (pro.speed_polar.second > DBL_EPSILON || pro.speed_polar.second < -DBL_EPSILON)
                pro.rotation.first = (pro.speed_polar.first) * R2D + pro.angular_initial_target.first;
            break;
        }
        case RotationMode::FollwoAcceleration: //跟随加速度
        {
            if (pro.acceleration_polar.second > DBL_EPSILON || pro.acceleration_polar.second < -DBL_EPSILON)
                pro.rotation.first = pro.acceleration_polar.first * R2D + pro.angular_initial_target.first;
            break;
        }
        case RotationMode::TowardsMouse:
        {
            //将目标点设为鼠标位置
            pro.target_aming.first = pos_mouse.x();
            pro.target_aming.second = pos_mouse.y();
        }
            [[clang::fallthrough]];
        case RotationMode::TowardsTarget: //指向目标
        {
            //根据目标坐标位置计算目标角度
            pro.angular_initial_target.second = qAtan2(pro.target_aming.second - pos.second, pro.target_aming.first - pos.first) * R2D + pro.angular_initial_target.first;

            //获取当前角度
            pro.rotation.first = p_object->item->rotation();

            //将两个角度转换为区间[0,360]内的等效值
            while (pro.angular_initial_target.second > 360)
                pro.angular_initial_target.second -= 360;
            while (pro.angular_initial_target.second < 0)
                pro.angular_initial_target.second += 360;
            while (pro.rotation.first > 360)
                pro.rotation.first -= 360;
            while (pro.rotation.first < 0)
                pro.rotation.first += 360;

            //获取正向旋转时的差值
            Decimal angle_forward = pro.angular_initial_target.second - pro.rotation.first;
            Decimal angle_backward;
            //获取正向负向的偏移角度
            if (angle_forward > 0)
                angle_backward = 360 - angle_forward;
            else
            {
                angle_backward = -angle_forward;
                angle_forward = 360 - angle_backward;
            }
            //剩余最小角距离
            Decimal offsest_min = angle_forward < angle_backward ? angle_forward : angle_backward;

            bool flag_forward = false;

            if (angle_forward < angle_backward)
            {
                flag_forward = true;
                offsest_min = angle_forward;
            }
            else
                offsest_min = angle_backward;

            //在惯性角度内
            if (offsest_min < pro.rotation.second) //持续减速直到抵达目标
            {
                //计算剩余时间
                auto t = qSqrt(2 * offsest_min / pro.angular_acc_max.second);
                pro.angular_speed_max.first = pro.angular_acc_max.second * t;

                if (offsest_min < pro.angular_acc_max.second) //达到临界值
                {
                    pro.angular_speed_max.first = 0; //设置角速度=0
                }
                if (!flag_forward)
                    pro.angular_speed_max.first = -pro.angular_speed_max.first;
            }
            else //持续加速直到满速
            {
                pro.angular_speed_max.first += pro.angular_acc_max.second;      //根据角加速度更新角速度
                if (pro.angular_speed_max.first > pro.angular_speed_max.second) //限制角速度
                    pro.angular_speed_max.first = pro.angular_speed_max.second;
                else if (pro.angular_speed_max.first < -pro.angular_speed_max.second)
                    pro.angular_speed_max.first = -pro.angular_speed_max.second;
            }
            //            qDebug()<<pro.angular_speed_max.first;
            pro.rotation.first += pro.angular_speed_max.first; //根据角速度更新角度
                                                               //            qDebug()<<pro.rotation.first;
            break;
        }
        case RotationMode::Stop: //自动停止旋转
        {
            if(pro.angular_speed_max.first<pro.angular_acc_max.second||pro.angular_speed_max.first>-pro.angular_acc_max.second)
                pro.angular_speed_max.first=0;
            else if(pro.angular_speed_max.first<0)
                pro.angular_speed_max.first+=pro.angular_acc_max.second;
            else if(pro.angular_speed_max.first>0)
                pro.angular_speed_max.first-=pro.angular_acc_max.second;
            break;
        }
        case RotationMode::Unlimited: //无限制
        {
            pro.angular_speed_max.first += pro.angular_acc_max.first;       //根据角加速度更新角速度
            if (pro.angular_speed_max.first > pro.angular_speed_max.second) //限制角速度
                pro.angular_speed_max.first = pro.angular_speed_max.second;
            else if (pro.angular_speed_max.first < -pro.angular_speed_max.second)
                pro.angular_speed_max.first = -pro.angular_speed_max.second;
            pro.rotation.first += pro.angular_speed_max.first; //更新角度
            break;
        }
        }


        ///碰撞检测

        continue;

        auto list= scene_main->collidingItems(p_object->item);

        map[p_object]=true;//添加到map

        //遍历每个碰撞元素
        for(auto item:list)
        {
            //获取元素的管理对象
            auto p_fo=dynamic_cast<Element *>(item)->parent;

            if(!p_fo->property.flag_collision)//查看是否开启碰撞标记
                continue;

            if(p_object->type==ObjectType::HitObject&&p_fo->type==ObjectType::HitObject)
            {
                //都是碰撞对象
            }

            //计算轴向角度

            //计算轴向速度

            //计算非轴向速度
        }

    }




}

void ObjectsControl::manage_objects()
{
    QWriteLocker(&data_runtime.lock); //写锁定

    FlyingObject *obj_new{nullptr};

    //逐个遍历
    for (int index = 0; index < data_runtime.list_objects.size(); ++index)
    {

        FlyingObject *p_object = data_runtime.list_objects[index];
        auto &pro = p_object->property; //获取属性引用

        if (p_object == nullptr)
        {
            qDebug() << "<nullptr> int data_runtime.list_objects";
            continue;
        }

        //检查是否应该删除出界元素
        if (pro.flag_delete_outside_scene)
        {
            auto &pos = pro.coordinate;
            if (pos.first < 0 || pos.first > Settings::width_gaming || pos.second < 0 || pos.second > Settings::height_gaming)
            {
                data_runtime.list_objects.removeAt(index); //删除元素
                delete p_object;                           //释放
                p_object = nullptr;
                --index;
                continue;
            }
        }

        //检查寿命
        if (pro.lifetime == 0)
        {
            data_runtime.list_objects.removeAt(index); //删除元素
            delete p_object;                           //释放
            p_object = nullptr;
            --index;
            continue;
        }
        else if (pro.lifetime > 0)
            --pro.lifetime; //--寿命

        if (!pro.rule) //派生规则不存在
            continue;  //跳过

        if (pro.cooldown_drive > 0)
        {
            --pro.cooldown_drive;
            continue;
        }
        else
            pro.cooldown_drive = pro.rule->period;
        --pro.cooldown_drive;

        //! 没有打开标记也应该降低冷却值, 之前这是个BUG
        if (!pro.flag_drive) //派生规则为空, 或标记未打开
            continue;

        ++pro.count_drive; //派生计数+1

        //逐个遍历派生单元
        for (const DeriveUnit &unit : pro.rule->units)
        {

            try
            {

                switch (unit.p->type)
                {
                case ObjectType::FlyingObject:
                {
                    obj_new = new FlyingObject(*unit.p); //申请新对象
                    break;
                }
                case ObjectType::ManipulableObject:
                case ObjectType::HitObject:
                {
                    obj_new = new HitObject(*dynamic_cast<HitObject *>(unit.p)); //申请新对象
                    break;
                }
                case ObjectType::End_ObjectType:
                    break;
                }
            }
            catch (std::exception &e)
            {
                qDebug() << e.what();
                continue;
            }

            if (obj_new == nullptr)
            {
                qDebug() << "<fail> new a object";
                break;
            }

            ///处理旋转方向
            Decimal range_rotation_float = 360 * unit.rotation_float;                                         //获取浮动总范围
            Decimal offset_rotation = (ToolFunctions::get_random_decimal_0_1() - 0.5) * range_rotation_float; //获取浮动值
            obj_new->property.rotation.first = offset_rotation;

            switch (unit.ref_direction)
            {
            case DR::RelativeToParentRotation: //相对基对象
            {
                obj_new->property.rotation.first += pro.rotation.first - pro.angular_initial_target.first;
                break;
            }
            case DR::RelativeToParentSpeed: //相对基对象速度
            {
                obj_new->property.rotation.first += pro.speed_polar.first * R2D;
                break;
            }
            case DR::RelativeToParentAcc: //相对基对象加速度
            {
                obj_new->property.rotation.first += pro.acceleration_polar.first * R2D;
                break;
            }
            case DR::Absolute: //绝对旋转
                break;
            }

            BinaryVector<Decimal> bv_tmp{}; //临时速度变量(极坐标)

            ///处理坐标

            obj_new->property.coordinate = unit.pos;
            if (unit.flag_relative_position) //相对基对象坐标
            {
                //先将相对的放置坐标转为极坐标
                ToolFunctions::axis_to_polar(unit.pos, bv_tmp);
                bv_tmp.first += (obj_new->property.rotation.first + pro.angular_initial_target.first) * D2R;
                ToolFunctions::polar_to_axis(bv_tmp, obj_new->property.coordinate);

                obj_new->property.coordinate.first += p_object->property.coordinate.first;
                obj_new->property.coordinate.second += p_object->property.coordinate.second;
            }

            ///处理速度

            bv_tmp = unit.speed;
            if (unit.speed.second < 0)                         //如果速度大小为负
                bv_tmp.second = unit.p->property.velocity_max; //使用最大速率
            //速度方向修正(以派生对象当前方向为参照)
            bv_tmp.first = bv_tmp.first + obj_new->property.rotation.first * D2R;
            ToolFunctions::polar_to_axis(bv_tmp, obj_new->property.speed_axis); //转为轴坐标

            if (unit.flag_inherit_speed) //速度继承
            {
                //继承全局速度
                obj_new->property.speed_axis.first += p_object->property.speed_axis.first;
                obj_new->property.speed_axis.second += p_object->property.speed_axis.second;
            }
            //根据轴坐标转换为极坐标
            ToolFunctions::axis_to_polar(obj_new->property.speed_axis, obj_new->property.speed_polar);

            ///处理加速度
            bv_tmp = unit.acceleration;
            if (unit.acceleration.second < 0)                      //如果速度大小为负
                bv_tmp.second = unit.p->property.acceleration_max; //使用最大加速度
            //速度方向修正(以派生对象当前方向为参照)
            bv_tmp.first = bv_tmp.first + obj_new->property.rotation.first * D2R;
            ToolFunctions::polar_to_axis(bv_tmp, obj_new->property.acceleration_axis); //转为轴坐标

            //根据轴坐标转换为极坐标
            ToolFunctions::axis_to_polar(obj_new->property.acceleration_axis, obj_new->property.acceleration_polar);

            data_runtime.list_objects.push_back(obj_new); //添加到管理列表
                                                          //            scene_main->addItem(obj_new->item);//添加到场景

            obj_new->add_to_scene(scene_main);
        }
    }
}

GamingWidget::GamingWidget(MainWindow *_main_window)
    : QWidget(), main_window(_main_window)
{
    init_components();
    init_UI();
    init_threads();
    init_signal_slots();
}

GamingWidget::~GamingWidget()
{
    timer.stop();
    thread_data_process.quit(); //退出线程
//    thread_objects_management.quit();
    thread_data_process.wait(); //等待线程结束
//    thread_objects_management.wait();

    clear();
}

void GamingWidget::init_components()
{
    layout_gird_main = new QGridLayout();
    //scene_main并不是字段, 是全局变量
    scene_main = new GraphicsScene(0, 0, Settings::width_gaming, Settings::height_gaming);

    view_main = new GraphicsView(scene_main);

    widget_menu = new QWidget();

    layout_widget_menu = new QGridLayout();

    button_resume = new Button("RESUME");
    button_exit = new Button("EXIT");

    label_info_esc_menu = new QLabel();
}

void GamingWidget::init_UI()
{
    auto p =new QOpenGLWidget();
    view_main->setViewport(p);


    //设置标题
    this->setWindowTitle("Gaming");
    //设置尺寸
    //    this->setFixedSize(Settings::width_gaming, Settings::height_gaming);

    this->setLayout(layout_gird_main);
    layout_gird_main->setContentsMargins(2, 2, 2, 2);

    //主界面设置
    view_main->setFixedSize(Settings::width_gaming, Settings::height_gaming);
    view_main->setMouseTracking(true);

    layout_gird_main->addWidget(view_main);

    //关闭滚动条
    view_main->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff); //水平
    view_main->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);   //垂直

    widget_menu->setLayout(layout_widget_menu);

    layout_widget_menu->setSpacing(10);
    layout_widget_menu->addWidget(label_info_esc_menu, 0, 0, Qt::AlignTop | Qt::AlignTop);
    layout_widget_menu->setRowStretch(1, 10);
    layout_widget_menu->addWidget(button_resume, 2, 0, Qt::AlignLeft);
    layout_widget_menu->addWidget(button_exit, 3, 0, Qt::AlignLeft);

    label_info_esc_menu->setText("Barrage Simulator");

    widget_menu->setGeometry(0, 0, this->sizeHint().width(), this->sizeHint().height());

    widget_menu->setParent(this);
    widget_menu->setVisible(false); //不可见
    widget_menu->raise();           //置于顶层
    widget_menu->setObjectName("widget_menu");

    this->setStyleSheet(
        "QWidget#widget_menu { background-color: rgba(00,00,00,0.5); }"
        //按钮
        "QWidget QPushButton{ background-color: #000000; color: #ffffff; border: 4px solid #999999; }"
        //hover
        "QWidget QPushButton::hover{ background-color: #ffffff; color: #000000; }"
        //label背景色
        "QWidget QLabel{ background-color: #000000; color: #ffffff; }"
        //所有组件的字体和字族
        "QWidget {font-size: 20px; font-family: consolas}");

    this->setFixedSize(this->sizeHint());
}

void GamingWidget::init_signal_slots()
{
    //定时器更新信号
    connect(&timer, &QTimer::timeout, this, &GamingWidget::update);

    //发生消息给控制台
    connect(object_thread_data_process, &ObjectsControl::push_info, main_window, &MainWindow::push_info);


    //退出菜单按钮
    connect(button_resume, &QPushButton::clicked, this, &GamingWidget::esc);


}

void GamingWidget::init_threads()
{
    object_thread_data_process = new ObjectsControl();              //new一个线程对象
    object_thread_data_process->moveToThread(&thread_data_process); //转移至线程
    thread_data_process.start();                                    //开启线程

    //    object_thread_objects_management = new ObjectsControl();                    //new一个线程对象
    //    object_thread_objects_management->moveToThread(&thread_objects_management); //转移至线程
    //    thread_objects_management.start();                                          //开启线程

    ///连接线程信号

    //线程中的对象稍后释放
    connect(&thread_data_process, &QThread::finished, object_thread_data_process, &QObject::deleteLater);
    //    connect(&thread_objects_management, &QThread::finished, object_thread_objects_management, &QObject::deleteLater);
    //耗时操作
    connect(this, &GamingWidget::signal_process_data, object_thread_data_process, &ObjectsControl::process_data);
    //    connect(this, &GamingWidget::signal_manage_objects, object_thread_objects_management, &ObjectsControl::manage_objects);
}

void GamingWidget::key_process()
{
    data_runtime.p1->property.mode_movement = MovementMode::Unlimited; //无限制运动

    ///鼠标控制
    if (data_runtime.status_keys[Key::ML] || mouse_delay > 0) //鼠标左键
        data_runtime.p1->property.flag_drive = true;          //派生状态打开
    else
        data_runtime.p1->property.flag_drive = false; //派生状态关闭

    ///键盘控制
    //方向控制
    if (data_runtime.status_keys[Key::P0_UP] && !data_runtime.status_keys[Key::P0_DOWN]) //按住上键没有按住下键
    {
        if (data_runtime.status_keys[Key::P0_LEFT]) //左键
        {
            //左上
            data_runtime.p1->property.acceleration_polar.first = -PI * 3 / 4; //135度
            data_runtime.p1->property.acceleration_polar.second = data_runtime.p1->property.acceleration_max;
        }
        else if (data_runtime.status_keys[Key::P0_RIGHT]) //左键
        {
            //右上
            data_runtime.p1->property.acceleration_polar.first = -PI / 4; //45度
            data_runtime.p1->property.acceleration_polar.second = data_runtime.p1->property.acceleration_max;
        }
        else
        {
            //上
            data_runtime.p1->property.acceleration_polar.first = -PI / 2; //90度
            data_runtime.p1->property.acceleration_polar.second = data_runtime.p1->property.acceleration_max;
        }
    }
    else if (data_runtime.status_keys[Key::P0_DOWN] && !data_runtime.status_keys[Key::P0_UP]) //按住上键没有按住下键
    {
        if (data_runtime.status_keys[Key::P0_LEFT]) //左键
        {
            //左下
            data_runtime.p1->property.acceleration_polar.first = PI * 3 / 4; //135度
            data_runtime.p1->property.acceleration_polar.second = data_runtime.p1->property.acceleration_max;
        }
        else if (data_runtime.status_keys[Key::P0_RIGHT]) //左键
        {
            //右下
            data_runtime.p1->property.acceleration_polar.first = PI / 4; //45度
            data_runtime.p1->property.acceleration_polar.second = data_runtime.p1->property.acceleration_max;
        }
        else
        {
            //下
            data_runtime.p1->property.acceleration_polar.first = PI / 2; //90度
            data_runtime.p1->property.acceleration_polar.second = data_runtime.p1->property.acceleration_max;
        }
    }
    else if (data_runtime.status_keys[Key::P0_LEFT] && !data_runtime.status_keys[Key::P0_RIGHT]) //按住左键没有按住右键
    {
        if (data_runtime.status_keys[Key::P0_UP]) //上键
        {
            //左上
            data_runtime.p1->property.acceleration_polar.first = -PI * 3 / 4; //135度
            data_runtime.p1->property.acceleration_polar.second = data_runtime.p1->property.acceleration_max;
        }
        else if (data_runtime.status_keys[Key::P0_DOWN]) //下键
        {
            //左下
            data_runtime.p1->property.acceleration_polar.first = PI * 3 / 4; //45度
            data_runtime.p1->property.acceleration_polar.second = data_runtime.p1->property.acceleration_max;
        }
        else
        {
            //左
            data_runtime.p1->property.acceleration_polar.first = PI; //90度
            data_runtime.p1->property.acceleration_polar.second = data_runtime.p1->property.acceleration_max;
        }
    }
    else if (data_runtime.status_keys[Key::P0_RIGHT] && !data_runtime.status_keys[Key::P0_LEFT]) //按住左键没有按住右键
    {
        if (data_runtime.status_keys[Key::P0_UP]) //上键
        {
            //右上
            data_runtime.p1->property.acceleration_polar.first = -PI * 1 / 4; //45度
            data_runtime.p1->property.acceleration_polar.second = data_runtime.p1->property.acceleration_max;
        }
        else if (data_runtime.status_keys[Key::P0_DOWN]) //下键
        {
            //右下
            data_runtime.p1->property.acceleration_polar.first = PI * 1 / 4; //45度
            data_runtime.p1->property.acceleration_polar.second = data_runtime.p1->property.acceleration_max;
        }
        else
        {
            //右
            data_runtime.p1->property.acceleration_polar.first = 0; //0度
            data_runtime.p1->property.acceleration_polar.second = data_runtime.p1->property.acceleration_max;
        }
    }
    else
    {
        data_runtime.p1->property.mode_movement = MovementMode::Stop; //自动停止
    }

    //退出键
    if (data_runtime.status_keys[Key::ESC])
    {
        data_runtime.status_keys[Key::ESC] = false; //重置按键状态
        esc();                                      //退出键
    }
}

void GamingWidget::esc()
{
    static bool active = false;

    if (active) //当前处于暂停状态
    {
        this->timer.start(); //开启计时器
        this->widget_menu->setVisible(false);
    }
    else
    {
        this->timer.stop(); //停止计时器
        this->widget_menu->setVisible(true);
    }
    active = !active;
}

void GamingWidget::clear()
{
    qDebug()<<"<called> clear()";
    for(auto p_object:data_runtime.list_objects)
        delete p_object;
}

void ObjectsControl::update_property()
{

    QReadLocker(&data_runtime.lock); //读锁定

    //    QWriteLocker(&data_runtime.lock);

    for (auto object : data_runtime.list_objects)
    {
        object->item->setPos(object->property.coordinate.first, object->property.coordinate.second); //更新位置
        object->item->setRotation(object->property.rotation.first);                                  //更新角度
        continue;
    }
}

void GamingWidget::exec()
{
    this->show();
    QEventLoop loop;
    loop.exec();
}

void GamingWidget::test()
{

    data_runtime.p1 = new ManipulableObject(*dynamic_cast<ManipulableObject *>(&objects_inner[InnerObjects::Block_zero_blue]));
    data_runtime.p1->property.coordinate = {100, 100};
    data_runtime.p1->add_to_scene(scene_main);    //添加到场景
    data_runtime.list_objects << data_runtime.p1; //添加到对象列表中

    auto p = new ManipulableObject(*dynamic_cast<ManipulableObject *>(&objects_inner[InnerObjects::Block_zero_red]));
    p->property.coordinate={500,200};
    p->add_to_scene(scene_main);    //添加到场景
    data_runtime.list_objects << p; //添加到对象列表中


    timer.setTimerType(Qt::TimerType::PreciseTimer);//精密计时器
    timer.start(Settings::interval); //开启定时器

    Settings::reset_key_map(); //重置/初始化键位
}

void GamingWidget::update()
{
    //状态栏数据更新
    static auto start = std::chrono::steady_clock::now();
    auto end = start;
    start=std::chrono::steady_clock::now();
    auto tt = std::chrono::duration_cast<std::chrono::microseconds>(start-end).count();
    time_consumption_total+=tt;
    if(num_updates%Settings::count_frames==0)
    {
        info_status_bar.time_consumption_average=time_consumption_total/Settings::count_frames;//计算平均时长
        time_consumption_total=0;
    }

    info_status_bar.time_consumption=tt;


    info_status_bar.pos_mouse = view_main->pos_mouse;                                //鼠标坐标
    info_status_bar.num_updates = num_updates;                                       //更新数
    info_status_bar.num_objects = data_runtime.list_objects.size();                  //对象数量
    data_runtime.pos_mouse_scene = view_main->mapToScene(info_status_bar.pos_mouse); //鼠标坐标

    emit signal_send_status_bar_info(&this->info_status_bar); //更新状态栏信息

    key_process(); //按键处理

    if (cooldown_next_data_update == 0)
    {
        emit signal_process_data(); //发送信号, 子线程处理数据

        ObjectsControl::manage_objects();  //管理对象
        ObjectsControl::update_property(); //主线程更新界面属性

        cooldown_next_data_update = Settings::period_data_update;

        if (mouse_delay > 0)
            --mouse_delay;
    }

    --cooldown_next_data_update;

    view_main->update(); //视图更新
//    view_main->viewport()->update();
//    view_main->repaint();
    ++num_updates;


}

void GamingWidget::keyPressEvent(QKeyEvent *event)
{
    if (!event->isAutoRepeat()) //排除自动重复的键盘事件
    {
        unsigned code_key = event->nativeVirtualKey();

        if (Settings::map_keys.find(code_key) != Settings::map_keys.end())
        {
            //设置对应键位按下状态
            data_runtime.status_keys[Settings::map_keys[code_key]] = true;
        }
        key_process();
    }
}

void GamingWidget::keyReleaseEvent(QKeyEvent *event)
{
    if (!event->isAutoRepeat()) //排除自动重复的键盘事件
    {
        unsigned code_key = event->nativeVirtualKey();

        if (Settings::map_keys.find(code_key) != Settings::map_keys.end())
        {
            //设置对应键位释放状态
            data_runtime.status_keys[Settings::map_keys[code_key]] = false;
            //            key_process(); //按键处理
        }
    }
}

void GamingWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton)
    {
        data_runtime.status_keys[Key::ML] = true;
        mouse_delay = 7;
    }
    if (event->buttons() & Qt::RightButton)
    {
        data_runtime.status_keys[Key::MR] = true;
    }
}

void GamingWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (!event->buttons() & Qt::LeftButton)
    {
        data_runtime.status_keys[Key::ML] = false;
    }
    if (!event->buttons() & Qt::RightButton)
    {
        data_runtime.status_keys[Key::MR] = false;
    }
}
