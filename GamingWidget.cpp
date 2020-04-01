#include "universal.h"

///全局变量定义
RunTimeData data_runtime{};
GraphicsScene *scene_main{nullptr};
GraphicsView * view_main{nullptr};//视图

void ObjectsControl::process_data()
{
        QReadLocker(&data_runtime.lock);//读锁定
//    QWriteLocker(&data_runtime.lock); //写锁定


    for (int index = 0; index < data_runtime.list_objects.size(); ++index)
    {
        //        bool flag_max_velocity{false};//满速标记
        ///坐标计算
        FlyingObject *p_crt = data_runtime.list_objects[index];//当前对象
        auto &pro = p_crt->property;         //物理属性
        auto &acc_p = pro.acceleration_polar;   //加速度_极坐标
        auto &acc_a = pro.acceleration_axis;    //加速度_轴坐标
        auto &v_p = pro.speed_polar;            //速度_极坐标
        auto &v_a = pro.speed_axis;             //加速度_轴坐标
        auto &pos = pro.coordinate;             //位置
        auto &att_v = pro.attenuation_velocity; //速度衰减
        auto &pos_mouse = data_runtime.pos_mouse_scene;//鼠标位置

        //        emit push_info(QString::asprintf("<acc_p> %.0f %.0f <acc_a> %.0f %.0f <v_p> %.0f %.0f <v_a> %.0f %.0f",acc_p.first,acc_p.second,acc_a.first,acc_a.second,v_p.first,v_p.second,v_a.first,v_a.second));
        //        qDebug()<<QString::asprintf("<acc_p> %f %f <acc_a> %f %f <v_p> %f %f <v_a> %f %f",acc_p.first,acc_p.second,acc_a.first,acc_a.second,v_p.first,v_p.second,v_a.first,v_a.second);

        if (pro.mode_movement == MovementMode::Stop) //自动停止
        {
            acc_a.first = acc_a.second = acc_p.first = acc_p.second = 0; //清除加速度
            v_p.second -= att_v;                                         //获取当前速度大小减去速度衰减
            if (v_p.second < 0)                                          //如果速度大小为负数
                v_p.second = 0;                                          //设为0
            //根据极坐标计算轴坐标速度
            ToolFunctionsKit::polar_to_axis(v_p, v_a);

            //根据速度更新位置
            pos.first += v_a.first;   //水平位移
            pos.second += v_a.second; //垂直位移
        }
        else if (pro.mode_movement == MovementMode::Unlimited) //无限制
        {
            //根据极坐标计算轴坐标加速度
            ToolFunctionsKit::polar_to_axis(acc_p, acc_a);

            //根据极坐标计算轴坐标速度
            ToolFunctionsKit::polar_to_axis(v_p, v_a);

            //计算加速后的轴坐标速度
            v_a.first += acc_a.first;
            v_a.second += acc_a.second;

            //根据轴坐标更新极坐标速度
            ToolFunctionsKit::axis_to_polar(v_a, v_p);

            if (pro.velocity_max > 0 && v_p.second > pro.velocity_max) //最大速度限制
            {
                v_p.second = pro.velocity_max;
                //重新计算轴坐标
                ToolFunctionsKit::polar_to_axis(v_p, v_a);
            }

            //根据速度更新位置
            pos.first += v_a.first;   //水平位移
            pos.second += v_a.second; //垂直位移
        }
        if (pro.flag_boundary_restriction) //边界检查
        {
            if (pos.first < 0)
            {
                pos.first = 1;
                v_a.first = 0;
            }
            if (pos.first > Settings::width_gaming)
            {
                pos.first = Settings::width_gaming-1;
                v_a.first = 0;
            }
            if (pos.second < 0)
            {
                pos.second = 1;
                v_a.second = 0;
            }
            if (pos.second > Settings::height_gaming)
            {
                pos.second = Settings::height_gaming-1;
                v_a.second = 0;
            }
        }
        ToolFunctionsKit::axis_to_polar(v_a, v_p);

        ///角度计算
        switch (pro.mode_rotation)
        {
        case RotationMode::Fixed: //固定无法旋转
            break;
        case RotationMode::FollowSpeed: //跟随速度
        {
            if (pro.speed_polar.second > DBL_EPSILON || pro.speed_polar.second < -DBL_EPSILON)
                pro.rotation.first = (pro.speed_polar.first) * R2D + pro.offset_front;
            break;
        }
        case RotationMode::FollwoAcceleration: //跟随加速度
        {
            if (pro.acceleration_polar.second > DBL_EPSILON || pro.acceleration_polar.second < -DBL_EPSILON)
                pro.rotation.first = pro.acceleration_polar.first * R2D + pro.offset_front;
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
            pro.angular_initial_target.second = qAtan2(pro.target_aming.second - pos.second, pro.target_aming.first - pos.first) * R2D + pro.offset_front;

            //获取当前角度
            pro.rotation.first = p_crt->item->rotation();

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
        if(!p_crt->property.flag_collision)//没有开启碰撞或已经处理过碰撞
            continue;//跳过

        //获取与当前元素发生碰撞的所有元素
        auto list= scene_main->collidingItems(p_crt->item);
//        QList<QGraphicsItem*> list/*= p_object->item->collidingItems()*/;

        bool flag_clear_last_collisio=true;//清空上次碰撞的标记

        //遍历每个与当前对象发生碰撞的对象
        for(auto item:list)
        {
            //获取元素的管理对象
            auto p_another=static_cast<Element *>(item)->obj_manage;

            bool flag_continue=false;//跳过标记


            BinaryVector<Decimal> tmp1,tmp2;//临时变量, 注意这两个变量在不同位置含义不同
            Decimal radian{0.0};//轴线弧度(计算结果加上该弧度得到最终结果)
            BinaryVector<Decimal> speed0_r{pro.speed_polar},speed1_r{p_another->property.speed_polar};//两个对象的速度

            //计算相对位置轴坐标
            tmp1.first=p_another->property.coordinate.first-pos.first;
            tmp1.second=p_another->property.coordinate.second-pos.second;

            //计算轴线弧度
            radian = qAtan2(tmp1.second,tmp1.first);


            //检查是否与之前碰撞的对象再次碰撞(粘性)
            if(p_crt->id_last_collision==p_another->id)
            {
                flag_clear_last_collisio=false;
                flag_continue=true;
            }
            if(p_another->id_last_collision==p_crt->id)
                flag_continue=true;//跳过

            if(!p_another->property.flag_collision)//查看是否开启碰撞标记
                flag_continue=true;//没开标记跳过


            {
                //都是碰撞对象
                ///处理game数据

                //获取game数据引用
                auto &pro_g_crt = p_crt->property_gaming;
                auto &pro_g_ano = p_another->property_gaming;

                if((pro_g_crt.team==pro_g_ano.team&&(pro_g_crt.flag_team_kill||pro_g_ano.flag_team_kill))||pro_g_crt.team!=pro_g_ano.team)
                    //同队伍但开启了友伤 或 不同队伍
                {
                    if(!(p_crt->id_penetrating==p_another->id||p_another->id_penetrating==p_crt->id))
                        //穿过目标只计算一次伤害
                    {
                        //产生伤害
                        pro_g_crt.endurance.first-=pro_g_ano.damage*pro_g_ano.penetrability/pro_g_crt.resist;
                        pro_g_ano.endurance.first-=pro_g_crt.damage*pro_g_crt.penetrability/pro_g_ano.resist;
                        //穿过物体也算作发生碰撞
                        --p_crt->property.number_rest_collision;
                        --p_another->property.number_rest_collision;
                    }
                }

                if(pro_g_crt.penetrability>pro_g_ano.resist||pro_g_ano.penetrability>pro_g_crt.resist)
                {
                    //互相设置穿透id
                    p_crt->id_penetrating=p_another->id;
                    p_another->id_penetrating=p_crt->id;
                    flag_continue=true;
                    flag_clear_last_collisio=false;
                }

            }

            //互斥
            if(!(p_crt->id_penetrating==p_another->id||p_another->id_penetrating==p_crt->id))
            {
                ///未发生穿透, 则产生互斥
                //计算互斥后坐标
                tmp1.first=radian;
                tmp1.second=Settings::distance_mutex;
                ToolFunctionsKit::polar_to_axis(tmp1,tmp2);
                //更新互斥后坐标
                p_crt->property.coordinate.first-=tmp2.first;
                p_crt->property.coordinate.second-=tmp2.second;
                p_another->property.coordinate.first+=tmp2.first;
                p_another->property.coordinate.second+=tmp2.second;
            }

            if(flag_continue)//跳过
                continue;

            //剩余碰撞计数--
            --p_crt->property.number_rest_collision;
            --p_another->property.number_rest_collision;

            //绝对弧度转为相对于轴的相对弧度
            speed0_r.first-=radian;
            speed1_r.first-=radian;

            //两个相对极坐标表示速度转轴坐标表示(横坐标为轴向值, 纵坐标为垂直轴向值)
            ToolFunctionsKit::polar_to_axis(speed0_r,tmp1);
            ToolFunctionsKit::polar_to_axis(speed1_r,tmp2);

//            qDebug()<<"<radian> "<<radian;
//            qDebug()<<"<speed_axis> "<<tmp1<<" "<<tmp2;

            ///轴向发生正碰, 动量守恒, 动能按系数损失(解二元二次方程)
            Decimal m1=pro.mass,m2=p_another->property.mass;// m1, m2
            Decimal x1,x2,d_tmp,c1=0,c2=0, y1,y2;

            //计算常数c1, c2 (减少两次乘法运算优化速度)
            d_tmp=m1*tmp1.first;// m1 * v1
            c1=d_tmp;
            d_tmp*=tmp1.first;// m1 * v1^2
            c2=d_tmp;

            d_tmp=m2*tmp2.first;// m2 * v2
            c1+=d_tmp;
            d_tmp*=tmp2.first;// m2 * v2^2
            c2+=d_tmp;

            //计算最低动能残留
            Decimal eta = (c1*c1)/((m1+m2)*(c2));
            d_tmp=pro.elasticity*p_another->property.elasticity;
            if(eta>d_tmp)
                d_tmp=eta;
            c2*=d_tmp;//考虑动能损失

            //计算两个解
            y1 = 2 * m1 * c1;// y1 临时作 -b
            y2 = 2*m1*(m1+m2);// y2 临时作 2a
            d_tmp = qSqrt(4*m1*(m1*c1*c1-(m1+m2)*(c1*c1-m2*c2)));// (b^2 - 4ac)^0.5
            x1 = (y1 + d_tmp) / y2;//第一个解
            x2 = (y1 - d_tmp) / y2;//第二个解

            //计算两个解对应的另一个速度
            y1 = (c1-m1*x1)/m2;
            y2 = (c1-m1*x2)/m2;

            //以下注释代码含有BUG
/*            if(std::signbit(tmp1.first)==std::signbit(tmp2.first))//速度方向相同, 追击碰撞
            {
                qDebug()<<"追击碰撞";
                if((tmp1.first>tmp2.first&&x1<tmp1.first)||(tmp2.first>tmp1.first&&y1<tmp2.first))
                    //第一组解中, 速度大的那个反而更大了 (等价于小的更小了)
                {
                    x1=x2;//使用第二组解
                    y1=y2;
                }
            }
            else//常规碰撞
            {
                qDebug()<<"常规碰撞";
                //默认使用第一组解
                if(std::signbit(x1)==std::signbit(tmp1.first)&&std::signbit(y1)==std::signbit(tmp2.first))//第一组解同号
                {
                    x1=x2;//使用第二组解
                    y1=y2;
                    qDebug()<<"第二组解";
                }else
                    qDebug()<<"第一组解";
            }*/

            //得到的解更新速度(直接使用第二组解)
            tmp1.first=x2;
            tmp2.first=y2;

            ToolFunctionsKit::axis_to_polar(tmp1,pro.speed_polar);
            ToolFunctionsKit::axis_to_polar(tmp2,p_another->property.speed_polar);

            //更新极坐标速度
            pro.speed_polar.first+=radian;
            p_another->property.speed_polar.first+=radian;

//            qDebug()<<"<speed> "<<pro.speed_polar.first*R2D<<" "<<pro.speed_polar.second;
//            qDebug()<<"<speed> "<<p_another->property.speed_polar.first*R2D<<" "<<p_another->property.speed_polar.second;

            p_crt->id_last_collision=p_another->id;
            p_another->id_last_collision=p_crt->id;

            ///非轴向动量守恒, 动能按系数转换为转动动能

//            break;//一个元素一次只处理一个碰撞
        }

        if(flag_clear_last_collisio)//重置id
        {
            p_crt->id_penetrating=-1;
            p_crt->id_last_collision=-1;
        }


    }


}

void ObjectsControl::manage_objects()
{
    QWriteLocker(&data_runtime.lock); //写锁定

    //逐个遍历
    for (int index = 0; index < data_runtime.list_objects.size(); ++index)
    {

        FlyingObject *p_crt = data_runtime.list_objects[index];
        bool flag_destroy=false;//是否需要销毁的标记
        auto &pro = p_crt->property; //获取属性引用

        ///销毁对象

        //检查是否应该删除出界元素
        if (pro.flag_delete_outside_scene)
        {
            auto &pos = pro.coordinate;
            if (pos.first < 0 || pos.first > Settings::width_gaming || pos.second < 0 || pos.second > Settings::height_gaming)
                flag_destroy=true;
        }

        //检查寿命
        if (pro.lifetime == 0)
            flag_destroy=true;
        else if (pro.lifetime > 0)
            --pro.lifetime; //--寿命

        //检查剩余碰撞次数
        if(pro.number_rest_collision==0)
            flag_destroy=true;

        if(p_crt->type==ObjectType::ManipulableObject)
        {
            //检查耐久
            if((p_crt)->property_gaming.endurance.first<0)
                flag_destroy=true;
        }

        if(flag_destroy)
        {
            //销毁对象
            data_runtime.list_objects.removeAt(index); //删除元素
            if(pro.rule_on_destroy)
                derive_object(pro, pro.rule_on_destroy); //使用销毁规则进行派生(相当于炉石的亡语)
            delete p_crt;                                //释放
            --index;
            continue;
        }


        ///派生对象
        if (!pro.rule) //派生规则不存在
            continue;  //跳过

        if (pro.cooldown_drive > 0)
        {
            --pro.cooldown_drive;
            continue;
        }
        else
            pro.cooldown_drive = pro.rule->period;//重设冷却

        //! 没有打开标记也应该降低冷却值, 之前这是个BUG
        if (!pro.flag_drive) //派生规则为空, 或标记未打开
            continue;

        derive_object(pro, pro.rule);//使用常驻规则进行派生
        ++pro.count_drive; //派生计数+1

    }
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

void ObjectsControl::derive_object(const ObjectControlProperty &pro, DeriveRule *rule)
{
    FlyingObject *obj_new{nullptr};

    //逐个遍历派生单元
    for (const DeriveUnit &unit : rule->units)
    {
        if(ToolFunctionsKit::get_random_decimal_0_1()>unit.probability)
            continue;
        try
        {
            obj_new = new FlyingObject(*(unit.p)); //申请新对象
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
        Decimal offset_rotation = (ToolFunctionsKit::get_random_decimal_0_1() - 0.5) * range_rotation_float; //获取浮动值
        obj_new->property.rotation.first = offset_rotation;

        switch (unit.ref_direction)
        {
        case DR::RelativeToParentRotation: //相对基对象
        {
            obj_new->property.rotation.first += pro.rotation.first - pro.offset_front;
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
            ToolFunctionsKit::axis_to_polar(unit.pos, bv_tmp);
            bv_tmp.first += (obj_new->property.rotation.first + pro.offset_front) * D2R;
            ToolFunctionsKit::polar_to_axis(bv_tmp, obj_new->property.coordinate);

            obj_new->property.coordinate.first += pro.coordinate.first;
            obj_new->property.coordinate.second += pro.coordinate.second;
        }

        ///处理速度

        bv_tmp = unit.speed;
        if (unit.speed.second < 0)                         //如果速度大小为负
            bv_tmp.second = unit.p->property.velocity_max; //使用最大速率
        //速度方向修正(以派生对象当前方向为参照)
        bv_tmp.first = bv_tmp.first + obj_new->property.rotation.first * D2R;
        ToolFunctionsKit::polar_to_axis(bv_tmp, obj_new->property.speed_axis); //转为轴坐标

        if (unit.flag_inherit_speed) //速度继承
        {
            //继承全局速度
            obj_new->property.speed_axis.first += pro.speed_axis.first;
            obj_new->property.speed_axis.second += pro.speed_axis.second;
        }
        //根据轴坐标转换为极坐标
        ToolFunctionsKit::axis_to_polar(obj_new->property.speed_axis, obj_new->property.speed_polar);

        ///处理加速度
        bv_tmp = unit.acceleration;
        if (unit.acceleration.second < 0)                      //如果速度大小为负
            bv_tmp.second = unit.p->property.acceleration_max; //使用最大加速度
        //速度方向修正(以派生对象当前方向为参照)
        bv_tmp.first = bv_tmp.first + obj_new->property.rotation.first * D2R;
        ToolFunctionsKit::polar_to_axis(bv_tmp, obj_new->property.acceleration_axis); //转为轴坐标

        //根据轴坐标转换为极坐标
        ToolFunctionsKit::axis_to_polar(obj_new->property.acceleration_axis, obj_new->property.acceleration_polar);

        data_runtime.list_objects.push_back(obj_new); //添加到管理列表
        obj_new->add_to_scene(scene_main);//添加到场景
    }
}


GameWidget::GameWidget(MainWindow *_main_window)
    : QWidget(), main_window(_main_window)
{
    init_components();
    init_UI();
    init_threads();
    init_signal_slots();

    load_title_images();//加载标题背景图片
}

GameWidget::~GameWidget()
{

    thread_data_process.quit(); //退出线程
//    thread_objects_management.quit();
    thread_data_process.wait(); //等待线程结束
//    thread_objects_management.wait();
    this->reset();//重置
}

void GameWidget::init_components()
{
    layout_main = new QGridLayout();
    //scene_main并不是字段, 是全局变量
    scene_main = new GraphicsScene(0, 0, Settings::width_gaming, Settings::height_gaming);
    scene_main->setItemIndexMethod(QGraphicsScene::NoIndex);//无索引(对于动态元素效果更好)

    view_main = new GraphicsView(scene_main);

    //标题页场景
    scene_title=new GraphicsScene(0,0,Settings::width_gaming, Settings::height_gaming);
    view_title=new GraphicsView(scene_title);

    widget_main=new QStackedWidget();
    widget_title=new QWidget();
    panel_title=new QWidget();
    widget_start=new QWidget();
    panel_start=new QWidget();
    list_widget_start=new QListWidget();


    widget_menu = new QWidget();

    layout_widget_menu = new QGridLayout();
    layout_title=new QGridLayout();
    layout_start=new QGridLayout();
    layout_panel_title=new QGridLayout();
    layout_panel_start=new QGridLayout();


    button_start=new Button("START");
    button_load=new Button("LOAD");
    button_options=new Button("OPTIONS");
    button_exit=new Button("EXIT");
    button_refresh_start=new Button("REFRESH");

    button_resume_pause_menu = new Button("RESUME");
    button_exit_pause_menu = new Button("EXIT");
    button_back_start=new Button("BACK");
    button_play_start=new Button("PLAY!");

    label_info_esc_menu = new QLabel();
    label_title=new QLabel();
    label_bottom_info_title=new QLabel();
    label_start_page_top=new QLabel();
}

void GameWidget::init_UI()
{

    ///总体样式
    this->setStyleSheet(
        //全部
        "QWidget { background-color:rgba(0,0,0,0.01); color: #ffffff; font-size: 20px; font-family: consolas; }"
        //暂停菜单
        "QWidget#widget_menu { background-color: rgba(00,00,00,0.6); }"
        //面板
        "QWidget#panel { background-color: rgba(0,0,0,0.8); }"
        //按钮
        "QPushButton { background-color: rgba(0,0,0,1.0); color: #ffffff; border: 2px solid #00ff00; width: 200px; height:50px; }"
        "QWidget:disabled { background-color: rgba(0,0,0,0.0); color: #999999; border: 2px solid #000000; }"
        //按钮hover
        "QPushButton::hover{ background-color: rgba(200,200,200,1.0); border: 2px solid #000000; }"
        //面板下
        "QWidget#panel > QListWidget { border: 2px solid #00ff00; }"
        "QWidget#panel > QPushButton { width: 400px; }"
        "QWidget#panel > QPushButton#refresh { width:200px; border: 2px solid #000000; }"
        //label背景色
        "QWidget QLabel#title{ background-color: rgba(0,0,0,0.8); color: #ffffff; }"
    );



    ///主窗口
    this->setWindowTitle("BarrageSimulator - Particles");

    this->setMinimumSize(Settings::width_gaming,Settings::height_gaming);

    this->setLayout(layout_main);
    layout_main->setContentsMargins(0,0,0,0);

    layout_main->addWidget(widget_main);


    ///标题页
    widget_title->setLayout(layout_title);//设置布局
    layout_title->setContentsMargins(0,0,0,0);


    //模糊效果
    effect_blur.setBlurRadius(Settings::radius_blur);
    effect_blur.setBlurHints(QGraphicsBlurEffect::BlurHint::PerformanceHint);
    bg.setGraphicsEffect(&effect_blur);

    auto widget_OGL =new QOpenGLWidget();

    view_title->setViewport(widget_OGL);
//    view_title->setGraphicsEffect(&effect_blur);

    view_title->setSceneRect(0,0,view_title->width(),view_title->height());

    view_title->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff); //水平
    view_title->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);   //垂直
    view_title->setParent(this);
    view_title->lower();

    view_title->setCacheMode(QGraphicsView::CacheBackground);
//    view_title->setRenderHints(QPainter::Antialiasing);//抗锯齿

    label_title->setPixmap(QPixmap(Settings::path_title_image));

    layout_title->addWidget(label_title,0,0,1,3,Qt::AlignLeft|Qt::AlignTop);
    layout_title->setRowStretch(1,1);
    layout_title->addWidget(panel_title,2,0,1,4);
    layout_title->setRowStretch(3,1);
    layout_title->addWidget(label_bottom_info_title,4,0,1,4,Qt::AlignLeft);

    panel_title->setLayout(layout_panel_title);
    panel_title->setObjectName("panel");

    layout_panel_title->addWidget(button_start,0,1,1,2,Qt::AlignCenter);
    layout_panel_title->addWidget(button_load,1,1,1,2,Qt::AlignCenter);
    layout_panel_title->addWidget(button_options,2,1,1,2,Qt::AlignCenter);
    layout_panel_title->addWidget(button_exit,3,1,1,2,Qt::AlignCenter);



    widget_main->insertWidget(TitlePage,widget_title);


//    view_title->setStyleSheet("QGraphicsView { border: 5px solid #00ff00; }");

    ///start页
    widget_start->setLayout(layout_start);
    layout_start->setContentsMargins(0,0,0,0);

    label_start_page_top->setText("Scenes");
    label_start_page_top->setContentsMargins(5,5,5,5);
    label_start_page_top->setObjectName("title");

    layout_start->addWidget(label_start_page_top,0,0,Qt::AlignLeft);
    layout_start->setRowStretch(1,1);
    layout_start->addWidget(panel_start,2,0);
    layout_start->setRowStretch(3,1);

    panel_start->setLayout(layout_panel_start);
    panel_start->setObjectName("panel");

    button_refresh_start->setObjectName("refresh");
    button_play_start->setEnabled(false);//默认不可用

    layout_panel_start->addWidget(list_widget_start,0,1,Qt::AlignCenter);//列表组件
    layout_panel_start->addWidget(button_refresh_start,0,2,Qt::AlignLeft|Qt::AlignTop);//刷新按钮
    layout_panel_start->addWidget(button_play_start,1,2,Qt::AlignLeft);//刷新按钮
    layout_panel_start->addWidget(button_back_start,1,1,Qt::AlignCenter);//返回按钮

    layout_panel_start->setColumnStretch(0,1);
    layout_panel_start->setColumnStretch(1,0);
    layout_panel_start->setColumnStretch(2,1);

    list_widget_start->setFixedSize(402,500);


    widget_main->insertWidget(StartPage,widget_start);

    ///game页
    auto p =new QOpenGLWidget();
    view_main->setViewport(p);
    view_main->setMouseTracking(true);

    view_main->setContentsMargins(0,0,0,0);
    view_main->setSceneRect(0,0,view_main->width(),view_main->height());

    //关闭滚动条
    view_main->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff); //水平
    view_main->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);   //垂直

//    this->setLayout(layout_gird_main);
//    layout_gird_main->setContentsMargins(2, 2, 2, 2);
//    layout_gird_main->addWidget(widget_main);

    widget_main->insertWidget(GamePage,view_main);

    //主界面设置


    ///暂停菜单
    widget_menu->setLayout(layout_widget_menu);

    layout_widget_menu->setSpacing(10);
    layout_widget_menu->addWidget(label_info_esc_menu, 0, 0, Qt::AlignTop | Qt::AlignTop);
    layout_widget_menu->setRowStretch(1, 10);
    layout_widget_menu->addWidget(button_resume_pause_menu, 2, 0, Qt::AlignLeft);
    layout_widget_menu->addWidget(button_exit_pause_menu, 3, 0, Qt::AlignLeft);

    label_info_esc_menu->setText("Barrage Simulator");
    label_info_esc_menu->setObjectName("title");

    widget_menu->setParent(this);
    widget_menu->setVisible(false); //不可见
    widget_menu->raise();           //置于顶层
    widget_menu->setObjectName("widget_menu");





    //各种组件的尺寸初始设置
    this->resize(Settings::width_gaming,Settings::height_gaming);

    view_title->resize(Settings::width_gaming,Settings::height_gaming);

    widget_menu->resize(Settings::width_gaming,Settings::height_gaming);

    widget_main->setCurrentIndex(0);
}

void GameWidget::init_signal_slots()
{
    //定时器更新信号
    connect(&timer, &QTimer::timeout, this, &GameWidget::update);
    connect(&timer_title, &QTimer::timeout, this, &GameWidget::update_bg_image_position);

    //发生消息给控制台
    connect(object_thread_data_process, &ObjectsControl::push_info, main_window, &MainWindow::push_info);

    //按钮
    connect(button_resume_pause_menu, &QPushButton::clicked, this, &GameWidget::esc);
    connect(button_start, &QPushButton::clicked, this, &GameWidget::goto_start_page);
    connect(button_back_start, &QPushButton::clicked, this, &GameWidget::goto_title_page);

}

void GameWidget::init_threads()
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
    connect(this, &GameWidget::signal_process_data, object_thread_data_process, &ObjectsControl::process_data);
    //    connect(this, &GamingWidget::signal_manage_objects, object_thread_objects_management, &ObjectsControl::manage_objects);
}

void GameWidget::load_title_images()
{
    QDir dir(Settings::path_dir_title_bg);
    QStringList filter{"*.png","*.jpg"};
    QFileInfoList infos=dir.entryInfoList(filter,QDir::Files|QDir::Readable,QDir::Name);

    if(!dir.exists()||infos.size()==0)
    {
        this->label_bottom_info_title->setText("No background images found in DIR "+Settings::path_dir_title_bg);
        return;
    }

    QPixmap pixmap;

    //读取全部图片并放入容器
    for(QFileInfo info:infos)
    {
        if(!pixmap.load(info.filePath()))
            continue;//读取失败则跳过
        images_title.append(pixmap);
    }

    timer_title.setInterval(Settings::interval_title);
//    timer_title.start();

    scene_title->addItem(&bg);//添加到场景
}

void GameWidget::key_process()
{
    if(widget_main->currentIndex()==GamePage)
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

    }

    //退出键
    if (data_runtime.status_keys[Key::ESC])
    {
        data_runtime.status_keys[Key::ESC] = false; //重置按键状态
        esc();                                      //退出键
    }
}

void GameWidget::esc()
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

void GameWidget::clear()
{
    qDebug()<<"<called> clear()";
    //析构全部对象
    for(auto p_object:data_runtime.list_objects)
        delete p_object;
}

void GameWidget::load_scene_list()
{
    QDir dir(Settings::path_scenes);
    if(!dir.exists())
    {
        list_widget_start->clear();//清空
        return;
    }
    QStringList filter{"*.json"};
    QStringList path_files=dir.entryList(filter,QDir::Readable,QDir::Name);
}


void GameWidget::initialize()
{
    timer.setTimerType(Qt::TimerType::PreciseTimer);//精密计时器
    timer.start(Settings::interval); //开启定时器

}

void GameWidget::reset()
{
    this->hide();//隐藏
    timer.stop();//结束定时器
    timer_title.stop();
    this->clear();
}

void GameWidget::start()
{
    this->show();
    this->timer_title.start();//标题更新计时器开启
    Settings::reset_key_map();//重置键位
}

void GameWidget::exec()
{
    this->show();
    QEventLoop loop;
    loop.exec();
}

void GameWidget::test()
{
    data_runtime.p1 = new FlyingObject(objects_inner[InnerObjects::Block_zero_blue]);
    data_runtime.p1->property.coordinate = {100, 100};
    data_runtime.p1->add_to_scene(scene_main);    //添加到场景
    data_runtime.list_objects << data_runtime.p1; //添加到对象列表中

    auto p = new FlyingObject(objects_inner[InnerObjects::Block_zero_red]);
    p->property.coordinate={500,200};
    p->add_to_scene(scene_main);    //添加到场景
    data_runtime.list_objects << p; //添加到对象列表中

    initialize();//初始化

    this->setVisible(true);

    Settings::reset_key_map(); //重置/初始化键位
}

void GameWidget::update()
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
//        emit signal_process_data(); //发送信号, 子线程处理数据

        ObjectsControl::process_data();
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

void GameWidget::update_bg_image_position()
{
    static Integer count=0;//刷新次数
    static int index_crt=0;
    static int size=this->images_title.size();//获取图片数量
    static QSize size_pixmap_crt;

    if(size<=0)
        return;//没有图片直接退出

    //每秒更新数
//    const static Integer count_per_second = 1000/Settings::interval_title;

    auto pos = bg.pos();
    pos.rx()+=speed_bg_moving.first;
    pos.ry()+=speed_bg_moving.second;
    view_title->update();

    if(size_pixmap_crt.rheight()>view_title->height()&&size_pixmap_crt.rwidth()>view_title->width())
    {
        Decimal tmp{0.0};
        tmp=-(size_pixmap_crt.rwidth()-view_title->width())/2;
        if(pos.rx()<tmp)
        {
            pos.rx()=tmp;
            speed_bg_moving.first=-speed_bg_moving.first;
        }
        tmp=-tmp;
        if(pos.rx()>tmp)
        {
            pos.rx()=tmp;
            speed_bg_moving.first=-speed_bg_moving.first;
        }
        tmp=-(size_pixmap_crt.rheight()-view_title->height())/2;
        if(pos.ry()<tmp)
        {
            pos.ry()=tmp;
            speed_bg_moving.second=-speed_bg_moving.second;
        }
        tmp=-tmp;
        if(pos.ry()>tmp)
        {
            pos.ry()=tmp;
            speed_bg_moving.second=-speed_bg_moving.second;
        }

        bg.setPos(pos);//更新位置
    }


    if(count%Settings::period_change_speed==0)//更新移动速度
    {
        //水平速度
        BinaryVector<Decimal> speed_polar{0,Settings::speed_bg_moving};
        speed_polar.first=2*PI*ToolFunctionsKit::get_random_decimal_0_1();//方
        ToolFunctionsKit::polar_to_axis(speed_polar,speed_bg_moving);//极转轴
    }

    if(count%Settings::period_change_title_bg==0)//更换图片
    {
        ++index_crt;
        index_crt%=size;
        this->bg.setPixmap(images_title[index_crt]);
        size_pixmap_crt=images_title[index_crt].size();
        bg.setOffset(-size_pixmap_crt.rwidth()/2, -size_pixmap_crt.rheight()/2);//设置中心偏移

        bg.setPos(0,0);//置于中心
    }


    ++count;
}

void GameWidget::goto_start_page()
{
    this->widget_main->setCurrentIndex(StartPage);
}

void GameWidget::goto_title_page()
{
    this->widget_main->setCurrentIndex(TitlePage);
}



void GameWidget::keyPressEvent(QKeyEvent *event)
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

void GameWidget::keyReleaseEvent(QKeyEvent *event)
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

void GameWidget::mousePressEvent(QMouseEvent *event)
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

void GameWidget::mouseReleaseEvent(QMouseEvent *event)
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

void GameWidget::resizeEvent(QResizeEvent *event)
{
    this->view_title->resize(event->size());
    this->widget_menu->resize(event->size());

    view_main->setSceneRect(0,0,view_main->width()-1,view_main->height()-1);
    view_title->setSceneRect(-view_title->width()/2,-view_title->height()/2,view_title->width(),view_title->height());
}

//void GameWidget::moveEvent(QMoveEvent *event)
//{

//}









