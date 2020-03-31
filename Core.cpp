#include "universal.h"


namespace Settings
{
    int interval=10;//两次画面刷新之间的间隔(单位ms)(10ms更新一次)
    Integer period_data_update=1;//周期(10ms更新一次)
    bool flag_highlight=true;//受击高亮
    Integer time_highlight=4;//高亮持续刻数
    bool flag_music=false;//音乐开关
    bool flag_sound=false;//音效开关

    int width_interface=500;
    int height_interface=200;

    int width_gaming=1280;
    int height_gaming=720;

    int width_button=200;//按钮统一宽度
    int height_button=50;//按钮统一高度

    int width_gaming_menu=300;//菜单宽度

    int count_frames=100;

    int size_font=30;//字体大小


    int radius_blur=30;//模糊半径
    int interval_title=50;//标题页背景刷新间隔(单位ms)
    int period_change_title_bg=600;//更换标题背景图片的周期
    int period_change_speed=500;//更换移动速度的周期
    Decimal speed_bg_moving=0.8;//背景图轴向最大移动速度

    Decimal distance_mutex=1;//互斥距离(单位: 像素)

    QMap<unsigned,Definition::Key> map_keys{};

    void reset_key_map()
    {
        //默认键位
        //P0键位
        map_keys[87]=Key::P0_UP;//P0上键_W
        map_keys[83]=Key::P0_DOWN;//P0下键_S
        map_keys[65]=Key::P0_LEFT;//P0左键_A
        map_keys[68]=Key::P0_RIGHT;//P0右键_D
        map_keys[74]=Key::P0_IACT;//P0交互_J
        map_keys[75]=Key::P0_SP;//P0特殊_K

        //P1键位
        map_keys[38]=Key::P1_UP;//P1上键_UP
        map_keys[40]=Key::P1_DOWN;//P1下键_DW
        map_keys[37]=Key::P1_LEFT;//P1左键_LF
        map_keys[39]=Key::P1_RIGHT;//P1右键_RW
        map_keys[110]=Key::P0_IACT;//P0交互_小键盘.
        map_keys[108]=Key::P0_SP;//P0特殊_小键盘回车

        map_keys[27]=Key::ESC;//ESC



    }

    //标题图片路径
    QString path_title_image="./data/images/title.png";

    QString path_dir_title_bg="./data/images/images_background_title/";


}


Core::Objects::Element::Element(const QPixmap &_pixmap, const int &_number_frame,const int &_period_frame)
    :QGraphicsPixmapItem(nullptr)
//    ,number_frame(_number_frame)
//    ,period_frame(_period_frame)
{
    this->set_frames(_pixmap,_number_frame,_period_frame);//设置帧
}

Core::Objects::Element::Element(const Core::Objects::Element &ano)
    :obj_manage(nullptr)
    ,number_frame(ano.number_frame)
    ,period_frame(ano.period_frame)
    ,frames(ano.frames)
{
    this->setPixmap(ano.pixmap());
    this->setOffset(ano.offset());
}

void Core::Objects::Element::next_frame()
{
    if(cooldown_next_frame==0)
    {
        cooldown_next_frame=period_frame;//重置冷却
        ++frame_current;
        this->setPixmap(frames[frame_current%number_frame]);//显示下一帧
    }
    else
        cooldown_next_frame--;//冷却-1
}

void Element::set_frames(const QPixmap &_pixmap, const int &_number_frame,const int &_period_frame)
{
    this->number_frame=_number_frame;
    this->period_frame=_period_frame;

    frames.clear();//清空

    //进行切片
//    frames.resize(number_frame);//设置大小
    this->setShapeMode(ShapeMode::BoundingRectShape);
//    this->boundingRect();
    int width =_pixmap.width(),height=_pixmap.height();
    int width_slice = width/number_frame;
    int x_begin=0;
    for(int i=0;i<number_frame;++i)//循环切片
        frames<<_pixmap.copy(x_begin,0,width_slice,height);//加入容器中
    this->setPixmap(frames[frame_current]);//默认设置第一帧
//    this->setOffset(-width_slice/2, -height/2);

}



bool Core::Objects::Element::collidesWithItem(const QGraphicsItem *other, Qt::ItemSelectionMode mode) const
{
//    qDebug()<<"<called> Element::collidesWithItem()";
//    return false;
    const Element* p = static_cast<const Element *>(other);

    //所属同一个碰撞频道
    if(p->obj_manage->property.channel_collision==this->obj_manage->property.channel_collision)
        //都没有开启同频道碰撞
        if(!p->obj_manage->property.flag_channel_collision&&!this->obj_manage->property.flag_channel_collision)
            return false;//没有碰撞
    //调用基类的实现并返回结果
//    return this->QGraphicsItem::collidesWithItem(other, mode);
    return this->QGraphicsPixmapItem::collidesWithItem(other, mode);
}

//void Core::Objects::Element::set_offset(const QPointF &offset)
//{
//    this->offset = offset;
//}

//void Core::Objects::Element::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
//{
//    Q_UNUSED(widget)
//    Q_UNUSED(option)
//    painter->drawPixmap(offset,this->frames[frame_current]);
//}

//QPainterPath Core::Objects::Element::shape() const
//{
//    const QBitmap &ref = this->frames[frame_current].mask();

//    QPainterPath p;

//    QGraphicsPixmapItem;

//}

//QRectF Core::Objects::Element::boundingRect() const
//{
//    return rect_bounding;
//}

Integer Core::Objects::FlyingObject::seq=-1;


Core::Objects::FlyingObject::FlyingObject()
{
    this->id=++seq;//全局唯一标识符
}

Core::Objects::FlyingObject::FlyingObject(const QString &name)
    :_name(name)
{
    this->id=++seq;//全局唯一标识符
}

Core::Objects::FlyingObject::~FlyingObject()
{
    if(this->item)
    {
        const auto &p=this->item->scene();
        if(p!=nullptr)//如果存在场景
            p->removeItem(this->item);//先从场景中删除对象及其子对象
        delete this->item;//释放空间
        this->item=nullptr;//重置为nullptr
    }
}

Core::Objects::FlyingObject::FlyingObject(const Core::Objects::FlyingObject &ano)
    :property(ano.property)
    ,_name(ano._name)
    ,type(ano.type)
    ,flag_group(ano.flag_group)
{
    this->id=++seq;//全局唯一标识符
//    qDebug()<<"<copy cunstructor> FlyingObject";
    if(flag_group)//群组
    {
        this->item=new QGraphicsItemGroup();
        //获取目标飞行对象的子对象
        auto list = static_cast<QGraphicsItemGroup*>(ano.item)->childItems();
        //复制构造所有的子元素, 并添加到组中
        for(auto item_tmp:list)
            static_cast<QGraphicsItemGroup*>(item)
                ->addToGroup(new Element(*static_cast<Element*>(item_tmp) ));//强制类型转换
    }
    else//非群组
    {
        //复制构造
        this->item=new Element(*static_cast<Element*>(ano.item));
        static_cast<Element*>(this->item)->obj_manage=this;//设置管理对象
    }
}

void Core::Objects::FlyingObject::set_name(const QString &name)
{
    this->_name=name;
}

const QString &Core::Objects::FlyingObject::name()const
{
    return this->_name;
}

void Core::Objects::FlyingObject::add_to_scene(QGraphicsScene *scene)
{
    //设置初始角度
    item->setRotation(this->property.rotation.first);
    //设置坐标
    this->item->setPos(property.coordinate.first,property.coordinate.second);
    //添加到场景
    scene->addItem(this->item);
}

void Core::Objects::FlyingObject::set_element(Element*ele)
{
    auto half = (ele->pixmap().size()/2);
    QPointF center(half.width(), half.height());
    set_element(ele, center);
}

void FlyingObject::set_element(Element *ele, const QPointF center)
{
    if(item==nullptr)
    {
        flag_group=false;
        this->item=ele;//设置元素指针
        ele->obj_manage=this;//将其管理对象注册为自身
        ele->setOffset(-center);
    }
    else
    {
        if(flag_group)//群组
            return;
        else
        {
            delete item;//释放原元素
            item=ele;//设置新元素
            ele->obj_manage=this;//将其管理对象注册为自身
            ele->setOffset(-center);
        }
    }
}

void Core::Objects::FlyingObject::initialize()
{
    //初始化本对象
    this->property.initialize();//初始化属性
}





Core::Objects::ManipulableObject::ManipulableObject()
{
    type=ObjectType::ManipulableObject;//更改类型, 碰撞对象
}

Core::Objects::ManipulableObject::ManipulableObject(const Core::Objects::ManipulableObject &ano)
    :FlyingObject(ano)
    ,property_gaming(ano.property_gaming)
{

}


Core::RunTimeData::RunTimeData()
{
    for(int i=0;i<End_Key;++i)
        status_keys[i]=false;
}

void Core::Definition::ObjectControlProperty::initialize()
{
    get_inertial_rotation_angle();//计算惯性旋转角

    this->rotation.first=this->angular_initial_target.first;//设置初始角度
}

void Core::Definition::ObjectControlProperty::get_inertial_rotation_angle()
{
    Decimal res{0};
    Decimal v_angular{0};
    while(v_angular<this->angular_speed_max.second)
    {
        res+=v_angular;//累计滑行角度
        v_angular+=this->angular_acc_max.second;//加速
    }
    this->rotation.second=res;//记录值
}


///工具函数

void Core::ToolFunctionsKit::polar_to_axis(const Definition::BinaryVector<Decimal> &polar, Definition::BinaryVector<Decimal> &axis)
{
    if(polar.second>DBL_EPSILON||polar.second<-DBL_EPSILON)//当极坐标矢量有大小时才进行计算
    {
        axis.first=qCos(polar.first)*polar.second;//水平方向乘以大小
        axis.second=qSin(polar.first)*polar.second;//垂直方向乘以大小
    }
    else
        //否则水平和垂直全部置为0
        axis.first=axis.second=0;
}

void Core::ToolFunctionsKit::axis_to_polar(const Definition::BinaryVector<Decimal> &axis, Definition::BinaryVector<Decimal> &polar)
{

    polar.first=qAtan2(axis.second,axis.first);//获取弧度
    polar.second=qSqrt(axis.first*axis.first+axis.second*axis.second);//获取大小
    if(polar.first<DBL_EPSILON&&polar.first>-DBL_EPSILON)
        polar.first=0;
    if(polar.second<DBL_EPSILON&&polar.second>-DBL_EPSILON)
        polar.second=0;
}

MovementMode ToolFunctionsKit::string_to_movement_mode(const QString &str)
{
    if(str=="Stop")
        return MovementMode::Stop;
    else if(str=="TowardsTarget")
        return MovementMode::TowardsTarget;
    else if(str=="Unlimited")
        return MovementMode::Unlimited;
    else
        return MovementMode::None;
}

RotationMode ToolFunctionsKit::string_to_rotation_mode(const QString &str)
{
    if(str=="Fixed")
        return RotationMode::Fixed;
    else if(str=="Stop")
        return RotationMode::Stop;
    else if(str=="FollowSpeed")
        return RotationMode::FollowSpeed;
    else if(str=="FollwoAcceleration")
        return RotationMode::FollwoAcceleration;
    else if(str=="TowardsMouse")
        return RotationMode::TowardsMouse;
    else if(str=="TowardsTarget")
        return RotationMode::TowardsTarget;
    else if(str=="Unlimited")
        return RotationMode::Unlimited;
    else
        return RotationMode::None;

}




Core::Definition::Decimal Core::ToolFunctionsKit::get_random_decimal_0_1()
{
    return qrand()/static_cast<Decimal>(RAND_MAX);//返回一个[0,1]区间内的浮点数
}






void Game::ResourcePackage::load(const QString &path)
{
    QDir dir;
    dir.setPath(path);

    //检查目录是否存在
    if(!dir.exists())
        throw QString::asprintf("<ERROR> DIR %s not found",path.toStdString().c_str());

}

FlyingObject *Game::ResourcePackage::parse_json__object(const QString &path)
{
    QFile file;
    file.setFileName(path);

    //检查文件是否存在
    if(!file.exists())
        throw QString::asprintf("<ERROR> FILE %s not found",path.toStdString().c_str());

    //打开文件
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    //读取所有内容
    QString content = file.readAll();
    //关闭文件
    file.close();

    QJsonParseError error_json;//解析错误
    QJsonDocument doc_json;//json文档

    doc_json.fromJson(content.toUtf8(),&error_json);

    if(error_json.error!=QJsonParseError::NoError)
        throw QString::asprintf("<ERROR> %s",error_json.errorString().toStdString().c_str());



    QJsonObject obj_json_root=doc_json.object();//获取根对象

    QStringList keys = obj_json_root.keys();//获取所有键




    ///---------------------------------------------------------------------------开始解析
    ManipulableObject obj_new;

    QString name;///name(*)
    if(obj_json_root.contains("name"))
    {
        QJsonValue value = obj_json_root.value("name");
        if(!value.isString())
            throw QString::asprintf("<ERROR> type of KEY \"name\" is not correct in FILE %s",path.toStdString().c_str());
        name=value.toString();
        obj_new.set_name(name);
    }


    //容器中添加一个对象
    objects[name]=obj_new;
    ManipulableObject & ref_obj_new = objects[name];


    QPointF center{-1.0,-1.0};///center
    if(obj_json_root.contains("center"))
    {
        QJsonValue center_v=obj_json_root.value("center");
        if(!center_v.isArray())
            throw QString::asprintf("<ERROR> type of KEY \"center\" is not correct in FILE %s",path.toStdString().c_str());

        QJsonArray array= center_v.toArray();

        if(array.size()!=2)
            throw QString::asprintf("<ERROR> VALUE of KEY \"center\" is not correct in FILE %s",path.toStdString().c_str());

        QJsonValue first=array[0],second=array[1];

        if(!(first.isDouble()&&second.isDouble()))
            throw QString::asprintf("<ERROR> type of VALUE in KEY \"center\" is not correct in FILE %s",path.toStdString().c_str());
        center.rx()=first.toDouble();
        center.ry()=second.toDouble();
    }
    else
        //非可选键未找到, 抛出异常
        throw QString::asprintf("<ERROR> KEY \"center\" not found in FILE %s",error_json.errorString().toStdString().c_str());

    int number_frame=1;///number_frame*
    if(obj_json_root.contains("number_frame"))
    {
        QJsonValue number_frame_v = obj_json_root.value("number_frame");

        if(!number_frame_v.isDouble())
            throw QString::asprintf("<ERROR> type of KEY \"number_frame\" is not correct in FILE %s",path.toStdString().c_str());
        number_frame=number_frame_v.toInt();
    }

    int interval=-1;///interval*
    if(obj_json_root.contains("interval"))
    {
        QJsonValue interval_v = obj_json_root.value("interval");

        if(!interval_v.isDouble())
            throw QString::asprintf("<ERROR> type of KEY \"interval\" is not correct in FILE %s",path.toStdString().c_str());
        interval=interval_v.toInt();
    }


    ///file_path
    if(obj_json_root.contains("file_path"))
    {
        QJsonValue file_path = obj_json_root.value("file_path");
        if(!file_path.isString())
            throw QString::asprintf("<ERROR> type of KEY \"file_path\" type is not correct in FILE %s",path.toStdString().c_str());

        pixmaps.emplace_back();//容器中添加一个对象(调用默认构造函数原地构造)
        pixmaps.back().load(this->path_pkg+file_path.toString());//加载图片文件

        elements.emplace_back(pixmaps.back(),number_frame,interval);//原地构造

        ref_obj_new.set_element(&elements.back(),center);//设置元素
    }
    else
        //非可选键未找到, 抛出异常
        throw QString::asprintf("<ERROR> KEY \"file_path\" not found in FILE %s",error_json.errorString().toStdString().c_str());

    ///property
    if(obj_json_root.contains("property"))
    {
        //获取property对象
        QJsonObject property = obj_json_root.value("property").toObject();

        auto & pro = ref_obj_new.property;


        ///---------------------------------------------------------------------------数值型

        Integer lifetime=pro.lifetime;///lifetime*
        if(property.contains("lifetime"))
        {
            QJsonValue lifetime_v = property.value("lifetime");
            if(!lifetime_v.isDouble())
                throw QString::asprintf("<ERROR> type of KEY \"property.lifetime\" is not correct in FILE %s",path.toStdString().c_str());
            lifetime=lifetime_v.toInt();
            pro.lifetime=lifetime;
        }

        Integer channel_collision=pro.channel_collision;///channel_collision*
        if(property.contains("channel_collision"))
        {
            QJsonValue channel_collision_v = property.value("channel_collision");
            if(!channel_collision_v.isDouble())
                throw QString::asprintf("<ERROR> type of KEY \"property.channel_collision\" is not correct in FILE %s",path.toStdString().c_str());
            channel_collision=channel_collision_v.toInt();
            pro.channel_collision=channel_collision;
        }

        Integer number_rest_collision=pro.number_rest_collision;///number_rest_collision*
        if(property.contains("number_rest_collision"))
        {
            QJsonValue number_rest_collision_v = property.value("number_rest_collision");
            if(!number_rest_collision_v.isDouble())
                throw QString::asprintf("<ERROR> type of KEY \"property.number_rest_collision\" is not correct in FILE %s",path.toStdString().c_str());
            number_rest_collision=number_rest_collision_v.toInt();
            pro.number_rest_collision=number_rest_collision;
        }

        Integer rule/*=pro.rule*/;///rule*
        if(property.contains("rule"))
        {
            QJsonValue rule_v = property.value("rule");
            if(!rule_v.isDouble())
                throw QString::asprintf("<ERROR> type of KEY \"property.rule\" is not correct in FILE %s",path.toStdString().c_str());
            rule=rule_v.toInt();
//            pro.rule=rule;
        }

        Integer rule_on_collision/*=pro.rule_on_collision*/;///rule_on_collision*
        if(property.contains("rule_on_collision"))
        {
            QJsonValue rule_on_collision_v = property.value("rule_on_collision");
            if(!rule_on_collision_v.isDouble())
                throw QString::asprintf("<ERROR> type of KEY \"property.rule_on_collision\" is not correct in FILE %s",path.toStdString().c_str());
            rule_on_collision=rule_on_collision_v.toInt();
//            pro.rule_on_collision=rule_on_collision;
        }

        Integer rule_on_destroy/*=pro.rule_on_destroy*/;///rule_on_destroy*
        if(property.contains("rule_on_destroy"))
        {
            QJsonValue rule_on_destroy_v = property.value("rule_on_destroy");
            if(!rule_on_destroy_v.isDouble())
                throw QString::asprintf("<ERROR> type of KEY \"property.rule_on_destroy\" is not correct in FILE %s",path.toStdString().c_str());
            rule_on_destroy=rule_on_destroy_v.toInt();
//            pro.rule_on_destroy=rule_on_destroy;
        }

        Decimal mass=pro.mass;///mass*
        if(property.contains("mass"))
        {
            QJsonValue mass_v = property.value("mass");
            if(!mass_v.isDouble())
                throw QString::asprintf("<ERROR> type of KEY \"property.mass\" is not correct in FILE %s",path.toStdString().c_str());
            mass=mass_v.toDouble();
            pro.mass=mass;
        }


        Decimal inertia_rotational=pro.inertia_rotational;///inertia_rotational*
        if(property.contains("inertia_rotational"))
        {
            QJsonValue inertia_rotational_v = property.value("inertia_rotational");
            if(!inertia_rotational_v.isDouble())
                throw QString::asprintf("<ERROR> type of KEY \"property.inertia_rotational\" is not correct in FILE %s",path.toStdString().c_str());
            inertia_rotational=inertia_rotational_v.toDouble();
            pro.inertia_rotational=inertia_rotational;
        }

        Decimal elasticity=pro.elasticity;///elasticity*
        if(property.contains("elasticity"))
        {
            QJsonValue elasticity_v = property.value("elasticity");
            if(!elasticity_v.isDouble())
                throw QString::asprintf("<ERROR> type of KEY \"property.elasticity\" is not correct in FILE %s",path.toStdString().c_str());
            elasticity=elasticity_v.toDouble();
            pro.elasticity=elasticity;
        }

        Decimal velocity_max=pro.velocity_max;///velocity_max*
        if(property.contains("velocity_max"))
        {
            QJsonValue velocity_max_v = property.value("velocity_max");
            if(!velocity_max_v.isDouble())
                throw QString::asprintf("<ERROR> type of KEY \"property.velocity_max\" is not correct in FILE %s",path.toStdString().c_str());
            velocity_max=velocity_max_v.toDouble();
            pro.velocity_max=velocity_max;
        }

        Decimal acceleration_max=pro.acceleration_max;///acceleration_max*
        if(property.contains("acceleration_max"))
        {
            QJsonValue acceleration_max_v = property.value("acceleration_max");
            if(!acceleration_max_v.isDouble())
                throw QString::asprintf("<ERROR> type of KEY \"property.acceleration_max\" is not correct in FILE %s",path.toStdString().c_str());
            acceleration_max=acceleration_max_v.toDouble();
            pro.acceleration_max=acceleration_max;
        }

        Decimal attenuation_velocity=pro.attenuation_velocity;///attenuation_velocity*
        if(property.contains("attenuation_velocity"))
        {
            QJsonValue attenuation_velocity_v = property.value("attenuation_velocity");
            if(!attenuation_velocity_v.isDouble())
                throw QString::asprintf("<ERROR> type of KEY \"property.attenuation_velocity\" is not correct in FILE %s",path.toStdString().c_str());
            attenuation_velocity=attenuation_velocity_v.toDouble();
            pro.attenuation_velocity=attenuation_velocity;
        }

        Decimal coefficient_friction=pro.coefficient_friction;///coefficient_friction*
        if(property.contains("coefficient_friction"))
        {
            QJsonValue coefficient_friction_v = property.value("coefficient_friction");
            if(!coefficient_friction_v.isDouble())
                throw QString::asprintf("<ERROR> type of KEY \"property.coefficient_friction\" is not correct in FILE %s",path.toStdString().c_str());
            coefficient_friction=coefficient_friction_v.toDouble();
            pro.coefficient_friction=coefficient_friction;
        }

        ///---------------------------------------------------------------------------数组型


        BinaryVector<Decimal> angular_initial_target=pro.angular_initial_target;///angular_initial_target*
        if(obj_json_root.contains("angular_initial_target"))
        {
            QJsonValue angular_initial_target_v=obj_json_root.value("angular_initial_target");
            if(!angular_initial_target_v.isArray())
                throw QString::asprintf("<ERROR> type of KEY \"property.angular_initial_target\" is not correct in FILE %s",path.toStdString().c_str());

            QJsonArray array= angular_initial_target_v.toArray();

            if(array.size()!=2)
                throw QString::asprintf("<ERROR> VALUE of KEY \"property.angular_initial_target\" is not correct in FILE %s",path.toStdString().c_str());

            QJsonValue first=array[0],second=array[1];

            if(!(first.isDouble()&&second.isDouble()))
                throw QString::asprintf("<ERROR> type of VALUE in KEY \"property.angular_initial_target\" is not correct in FILE %s",path.toStdString().c_str());
            angular_initial_target.first=first.toDouble();
            angular_initial_target.second=second.toDouble();

            pro.angular_initial_target=angular_initial_target;
        }

        BinaryVector<Decimal> angular_speed_max=pro.angular_speed_max;///angular_speed_max*
        if(obj_json_root.contains("angular_speed_max"))
        {
            QJsonValue angular_speed_max_v=obj_json_root.value("angular_speed_max");
            if(!angular_speed_max_v.isArray())
                throw QString::asprintf("<ERROR> type of KEY \"property.angular_speed_max\" is not correct in FILE %s",path.toStdString().c_str());

            QJsonArray array= angular_speed_max_v.toArray();

            if(array.size()!=2)
                throw QString::asprintf("<ERROR> VALUE of KEY \"property.angular_speed_max\" is not correct in FILE %s",path.toStdString().c_str());

            QJsonValue first=array[0],second=array[1];

            if(!(first.isDouble()&&second.isDouble()))
                throw QString::asprintf("<ERROR> type of VALUE in KEY \"property.angular_speed_max\" is not correct in FILE %s",path.toStdString().c_str());
            angular_speed_max.first=first.toDouble();
            angular_speed_max.second=second.toDouble();

            pro.angular_speed_max=angular_speed_max;
        }

        BinaryVector<Decimal> angular_acc_max=pro.angular_acc_max;///angular_acc_max*
        if(obj_json_root.contains("angular_acc_max"))
        {
            QJsonValue angular_acc_max_v=obj_json_root.value("angular_acc_max");
            if(!angular_acc_max_v.isArray())
                throw QString::asprintf("<ERROR> type of KEY \"property.angular_acc_max\" is not correct in FILE %s",path.toStdString().c_str());

            QJsonArray array= angular_acc_max_v.toArray();

            if(array.size()!=2)
                throw QString::asprintf("<ERROR> VALUE of KEY \"property.angular_acc_max\" is not correct in FILE %s",path.toStdString().c_str());

            QJsonValue first=array[0],second=array[1];

            if(!(first.isDouble()&&second.isDouble()))
                throw QString::asprintf("<ERROR> type of VALUE in KEY \"property.angular_acc_max\" is not correct in FILE %s",path.toStdString().c_str());
            angular_acc_max.first=first.toDouble();
            angular_acc_max.second=second.toDouble();

            pro.angular_acc_max=angular_acc_max;
        }


        ///---------------------------------------------------------------------------字符串型


        QString mode_movement="";///mode_movement*
        if(property.contains("mode_movement"))
        {
            QJsonValue mode_movement_v = property.value("mode_movement");
            if(!mode_movement_v.isString())
                throw QString::asprintf("<ERROR> type of KEY \"property.mode_movement\" is not correct in FILE %s",path.toStdString().c_str());
            mode_movement=mode_movement_v.toString();
            pro.mode_movement=ToolFunctionsKit::string_to_movement_mode(mode_movement);
        }

        QString mode_rotation="";///mode_rotation*
        if(property.contains("mode_rotation"))
        {
            QJsonValue mode_rotation_v = property.value("mode_rotation");
            if(!mode_rotation_v.isString())
                throw QString::asprintf("<ERROR> type of KEY \"property.mode_rotation\" is not correct in FILE %s",path.toStdString().c_str());
            mode_rotation=mode_rotation_v.toString();
            pro.mode_rotation=ToolFunctionsKit::string_to_rotation_mode(mode_rotation);
        }


        ///---------------------------------------------------------------------------布尔型

        bool flag_delete_outside_scene=pro.flag_delete_outside_scene;///flag_delete_outside_scene*
        if(property.contains("flag_delete_outside_scene"))
        {
            QJsonValue flag_delete_outside_scene_v = property.value("flag_delete_outside_scene");
            if(!flag_delete_outside_scene_v.isBool())
                throw QString::asprintf("<ERROR> type of KEY \"property.flag_delete_outside_scene\" is not correct in FILE %s",path.toStdString().c_str());
            flag_delete_outside_scene=flag_delete_outside_scene_v.toBool();
            pro.flag_delete_outside_scene=flag_delete_outside_scene;
        }

        bool flag_boundary_restriction=pro.flag_boundary_restriction;///flag_boundary_restriction*
        if(property.contains("flag_boundary_restriction"))
        {
            QJsonValue flag_boundary_restriction_v = property.value("flag_boundary_restriction");
            if(!flag_boundary_restriction_v.isBool())
                throw QString::asprintf("<ERROR> type of KEY \"property.flag_boundary_restriction\" is not correct in FILE %s",path.toStdString().c_str());
            flag_boundary_restriction=flag_boundary_restriction_v.toBool();
            pro.flag_boundary_restriction=flag_boundary_restriction;
        }

        bool flag_collision=pro.flag_collision;///flag_collision*
        if(property.contains("flag_collision"))
        {
            QJsonValue flag_collision_v = property.value("flag_collision");
            if(!flag_collision_v.isBool())
                throw QString::asprintf("<ERROR> type of KEY \"property.flag_collision\" is not correct in FILE %s",path.toStdString().c_str());
            flag_collision=flag_collision_v.toBool();
            pro.flag_collision=flag_collision;
        }

        bool flag_channel_collision=pro.flag_channel_collision;///flag_channel_collision*
        if(property.contains("flag_channel_collision"))
        {
            QJsonValue flag_channel_collision_v = property.value("flag_channel_collision");
            if(!flag_channel_collision_v.isBool())
                throw QString::asprintf("<ERROR> type of KEY \"property.flag_channel_collision\" is not correct in FILE %s",path.toStdString().c_str());
            flag_channel_collision=flag_channel_collision_v.toBool();
            pro.flag_channel_collision=flag_channel_collision;
        }



    }

    ///property_game
    if(obj_json_root.contains("property_game"))
    {
        //获取property对象
        QJsonObject property_game = obj_json_root.value("property_game").toObject();

        auto & pro_g = ref_obj_new.property_gaming;


        ///---------------------------------------------------------------------------数值型

        Integer team=pro_g.team;///team*
        if(property_game.contains("team"))
        {
            QJsonValue value = property_game.value("team");
            if(!value.isDouble())
                throw QString::asprintf("<ERROR> type of KEY \"property_game.team\" is not correct in FILE %s",path.toStdString().c_str());
            team=value.toInt();
            pro_g.team=team;
        }

        Decimal damage=pro_g.damage;///damage*
        if(property_game.contains("damage"))
        {
            QJsonValue lifetime_v = property_game.value("damage");
            if(!lifetime_v.isDouble())
                throw QString::asprintf("<ERROR> type of KEY \"property_game.damage\" is not correct in FILE %s",path.toStdString().c_str());
            damage=lifetime_v.toInt();
            pro_g.damage=damage;
        }

        Decimal resist=pro_g.resist;///resist*
        if(property_game.contains("resist"))
        {
            QJsonValue resist_v = property_game.value("resist");
            if(!resist_v.isDouble())
                throw QString::asprintf("<ERROR> type of KEY \"property_game.resist\" is not correct in FILE %s",path.toStdString().c_str());
            resist=resist_v.toInt();
            pro_g.resist=resist;
        }

        Decimal penetrability=pro_g.penetrability;///penetrability*
        if(property_game.contains("penetrability"))
        {
            QJsonValue penetrability_v = property_game.value("penetrability");
            if(!penetrability_v.isDouble())
                throw QString::asprintf("<ERROR> type of KEY \"property_game.penetrability\" is not correct in FILE %s",path.toStdString().c_str());
            penetrability=penetrability_v.toInt();
            pro_g.penetrability=penetrability;
        }


        ///---------------------------------------------------------------------------数组型


        BinaryVector<Decimal> endurance=pro_g.endurance;///endurance*
        if(obj_json_root.contains("endurance"))
        {
            QJsonValue endurance_v=obj_json_root.value("endurance");
            if(!endurance_v.isArray())
                throw QString::asprintf("<ERROR> type of KEY \"property_game.endurance\" is not correct in FILE %s",path.toStdString().c_str());

            QJsonArray array= endurance_v.toArray();

            if(array.size()!=2)
                throw QString::asprintf("<ERROR> VALUE of KEY \"property_game.endurance\" is not correct in FILE %s",path.toStdString().c_str());

            QJsonValue first=array[0],second=array[1];

            if(!(first.isDouble()&&second.isDouble()))
                throw QString::asprintf("<ERROR> type of VALUE in KEY \"property_game.endurance\" is not correct in FILE %s",path.toStdString().c_str());
            endurance.first=first.toDouble();
            endurance.second=second.toDouble();

            pro_g.endurance=endurance;
        }


        ///---------------------------------------------------------------------------布尔型

        bool flag_team_kill=pro_g.flag_team_kill;///flag_team_kill*
        if(property_game.contains("flag_team_kill"))
        {
            QJsonValue flag_team_kill_v = property_game.value("flag_team_kill");
            if(!flag_team_kill_v.isBool())
                throw QString::asprintf("<ERROR> type of KEY \"property_game.flag_team_kill\" is not correct in FILE %s",path.toStdString().c_str());
            flag_team_kill=flag_team_kill_v.toBool();
            pro_g.flag_team_kill=flag_team_kill;
        }





    }




}















