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
    //场景文件路径
    QString path_scenes="./data/levels/";
    //资源文件夹路径
    QString path_resource_pkg="./data/resources/";
    //
    QString path_images="/images/";


}


Core::Objects::Element::Element(const QPixmap &_pixmap, const int &_number_frame,const int &_period_frame)
    :QGraphicsPixmapItem(nullptr)
//    ,number_frame(_number_frame)
//    ,period_frame(_period_frame)
{
//    qDebug()<<"<called> constructor of Element";
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

void FlyingObject::remove_from_scene()
{
    this->item->scene()->removeItem(this->item);
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




Core::RunTimeData::RunTimeData()
{
    for(int i=0;i<End_Key;++i)
        status_keys[i]=false;
}

void RunTimeData::clear()
{
    //析构全部对象
    for(auto p_object:this->list_objects)
        delete p_object;
    this->pkg.clear();//资源包清空
    this->scene.clear();//场景清空
    this->score=0;//重置分数
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

DirectionReference ToolFunctionsKit::string_to_direction_reference(const QString &str)
{
    if(str=="RelativeToParentDirection")
        return DirectionReference::RelativeToParentDirection;
    else if(str=="RelativeToParentSpeed")
        return DirectionReference::RelativeToParentSpeed;
    else if(str=="RelativeToParentAcc")
        return DirectionReference::RelativeToParentAcc;
    else if(str=="Absolute")
        return DirectionReference::Absolute;
    else
        return DirectionReference::None;
}


Core::Definition::Decimal Core::ToolFunctionsKit::get_random_decimal_0_1()
{
    return qrand()/static_cast<Decimal>(RAND_MAX);//返回一个[0,1]区间内的浮点数
}





void Game::ResourcePackage::set_package_path(const QString &path)
{
    this->path_pkg=path+'/';
}

void Game::ResourcePackage::load()
{
    QDir dir;
    dir.setPath(this->path_pkg);

    //检查目录是否存在
    if(!dir.exists())
        throw QString::asprintf("<ERROR> DIR %s not found",path_pkg.toStdString().c_str());

    QStringList filters{"*.json"};
    //获取json文件列表
    QStringList list_files=dir.entryList(filters,QDir::Readable|QDir::Files,QDir::Name);


    this->load_images();//加载全部图片
    for(QString path:list_files)
    {
        if(path[0]=='o')//对象文件(子母o开头)
            this->parse_json__object(path);
        else if(path[0]=='r')
            this->parse_json__derive_rule(path);
    }

}

void ResourcePackage::clear()
{
    new (this)ResourcePackage();//调用默认构造函数重新构造
}

void ResourcePackage::load_images()
{
    QDir dir;
    QString path_dir=this->path_pkg+Settings::path_images;
    dir.setPath(path_dir);

    //检查目录是否存在
    if(!dir.exists())
        throw QString::asprintf("<ERROR> DIR %s not found",dir.path().toStdString().c_str());

    QStringList filters{"*.png","*.jpg","*.jpeg"};
    //获取图片文件列表
    QStringList list_files=dir.entryList(filters,QDir::Readable|QDir::Files,QDir::Name);

    QPixmap pixmap;
    for(QString name:list_files)
    {
        if(!pixmap.load(path_dir+name))
            throw QString::asprintf("<ERROR> fail to load image FILE %s",name.toStdString().c_str());
        this->pixmaps[name]=pixmap;//添加到容器中
    }

}

void Game::ResourcePackage::parse_json__object(const QString &path)
{
    QFile file;
    file.setFileName(this->path_pkg+path);

    //检查文件是否存在
    if(!file.exists())
        throw QString::asprintf("<ERROR> FILE %s not found",file.fileName().toStdString().c_str());

    //打开文件(只读|文本)
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    //读取所有内容
    QString content = file.readAll();
    //关闭文件
    file.close();

    QJsonParseError error_json;//解析错误
    QJsonDocument doc_json;//json文档

    doc_json=QJsonDocument::fromJson(content.toUtf8(),&error_json);

    if(error_json.error!=QJsonParseError::NoError)
        throw QString::asprintf("<ERROR> %s",error_json.errorString().toStdString().c_str());

    QJsonObject root=doc_json.object();//获取根对象

//    qDebug()<<root.keys();


    ///---------------------------------------------------------------------------开始解析
    FlyingObject obj_new;

    QString name;///name*
    if(root.contains("name"))
    {
        QJsonValue value = root.value("name");
        if(!value.isString())
            throw QString::asprintf("<ERROR> type of KEY \"name\" is not correct in FILE %s",path.toStdString().c_str());
        name=value.toString();
        obj_new.set_name(name);
    }

//    FlyingObject & obj_new = objects[name];


    QPointF center{-1.0,-1.0};///center
    if(root.contains(QStringLiteral("center")))
    {
        QJsonValue center_v=root.value("center");
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
        throw QString::asprintf("<ERROR> KEY \"center\" not found in FILE %s",path.toStdString().c_str());

    int number_frame=1;///number_frame*
    if(root.contains("number_frame"))
    {
        QJsonValue number_frame_v = root.value("number_frame");

        if(!number_frame_v.isDouble())
            throw QString::asprintf("<ERROR> type of KEY \"number_frame\" is not correct in FILE %s",path.toStdString().c_str());
        number_frame=number_frame_v.toInt();
    }

    int interval=-1;///interval*
    if(root.contains("interval"))
    {
        QJsonValue interval_v = root.value("interval");

        if(!interval_v.isDouble())
            throw QString::asprintf("<ERROR> type of KEY \"interval\" is not correct in FILE %s",path.toStdString().c_str());
        interval=interval_v.toInt();
    }


    ///file_path
    if(root.contains("file_name"))
    {
        QJsonValue value = root.value("file_name");
        if(!value.isString())
            throw QString::asprintf("<ERROR> type of KEY \"file_name\" type is not correct in FILE %s",path.toStdString().c_str());

        QString name=value.toString();

        Element * p_new_ele=new Element(pixmaps[name],number_frame,interval);
        elements.push_back(p_new_ele);

        obj_new.set_element(elements.back(),center);//设置元素

    }
    else
        //非可选键未找到, 抛出异常
        throw QString::asprintf("<ERROR> KEY \"file_name\" not found in FILE %s",error_json.errorString().toStdString().c_str());

    ///property
    if(root.contains("property"))
    {
        //获取property对象
        QJsonObject obj_property = root.value("property").toObject();

        auto & pro = obj_new.property;


        ///---------------------------------------------------------------------------数值型

        ///lifetime*
        if(obj_property.contains("lifetime"))
        {
            QJsonValue value = obj_property.value("lifetime");
            if(!value.isDouble())
                throw QString::asprintf("<ERROR> type of KEY \"property.lifetime\" is not correct in FILE %s",path.toStdString().c_str());
            pro.lifetime=value.toInt();
        }

        ///channel_collision*
        if(obj_property.contains("channel_collision"))
        {
            QJsonValue value = obj_property.value("channel_collision");
            if(!value.isDouble())
                throw QString::asprintf("<ERROR> type of KEY \"property.channel_collision\" is not correct in FILE %s",path.toStdString().c_str());
            pro.channel_collision=value.toInt();
        }

        Integer number_rest_collision=pro.number_rest_collision;///number_rest_collision*
        if(obj_property.contains("number_rest_collision"))
        {
            QJsonValue number_rest_collision_v = obj_property.value("number_rest_collision");
            if(!number_rest_collision_v.isDouble())
                throw QString::asprintf("<ERROR> type of KEY \"property.number_rest_collision\" is not correct in FILE %s",path.toStdString().c_str());
            number_rest_collision=number_rest_collision_v.toInt();
            pro.number_rest_collision=number_rest_collision;
        }

        Integer rule/*=pro.rule*/;///rule*
        if(obj_property.contains("rule"))
        {
            QJsonValue value = obj_property.value("rule");
            if(!value.isDouble())
                throw QString::asprintf("<ERROR> type of KEY \"property.rule\" is not correct in FILE %s",path.toStdString().c_str());
            rule=value.toInt();
//            pro.rule=rule;
        }

        Integer rule_on_collision/*=pro.rule_on_collision*/;///rule_on_collision*
        if(obj_property.contains("rule_on_collision"))
        {
            QJsonValue value = obj_property.value("rule_on_collision");
            if(!value.isDouble())
                throw QString::asprintf("<ERROR> type of KEY \"property.rule_on_collision\" is not correct in FILE %s",path.toStdString().c_str());
            rule_on_collision=value.toInt();
//            pro.rule_on_collision=rule_on_collision;
        }

        Integer rule_on_destroy/*=pro.rule_on_destroy*/;///rule_on_destroy*
        if(obj_property.contains("rule_on_destroy"))
        {
            QJsonValue value = obj_property.value("rule_on_destroy");
            if(!value.isDouble())
                throw QString::asprintf("<ERROR> type of KEY \"property.rule_on_destroy\" is not correct in FILE %s",path.toStdString().c_str());
            rule_on_destroy=value.toInt();
//            pro.rule_on_destroy=rule_on_destroy;
        }

        ///mass*
        if(obj_property.contains("mass"))
        {
            QJsonValue value = obj_property.value("mass");
            if(!value.isDouble())
                throw QString::asprintf("<ERROR> type of KEY \"property.mass\" is not correct in FILE %s",path.toStdString().c_str());
            pro.mass=value.toDouble();
        }

        ///force_mutex*
        if(obj_property.contains("force_mutex"))
        {
            QJsonValue value = obj_property.value("force_mutex");
            if(!value.isDouble())
                throw QString::asprintf("<ERROR> type of KEY \"property.force_mutex\" is not correct in FILE %s",path.toStdString().c_str());
            pro.force_mutex=value.toDouble();
        }


        ///inertia_rotational*
        if(obj_property.contains("inertia_rotational"))
        {
            QJsonValue value = obj_property.value("inertia_rotational");
            if(!value.isDouble())
                throw QString::asprintf("<ERROR> type of KEY \"property.inertia_rotational\" is not correct in FILE %s",path.toStdString().c_str());
            pro.inertia_rotational=value.toDouble();
        }

        Decimal elasticity=pro.elasticity;///elasticity*
        if(obj_property.contains("elasticity"))
        {
            QJsonValue value = obj_property.value("elasticity");
            if(!value.isDouble())
                throw QString::asprintf("<ERROR> type of KEY \"property.elasticity\" is not correct in FILE %s",path.toStdString().c_str());
            elasticity=value.toDouble();
            pro.elasticity=elasticity;
        }

        Decimal velocity_max=pro.velocity_max;///velocity_max*
        if(obj_property.contains("velocity_max"))
        {
            QJsonValue value = obj_property.value("velocity_max");
            if(!value.isDouble())
                throw QString::asprintf("<ERROR> type of KEY \"property.velocity_max\" is not correct in FILE %s",path.toStdString().c_str());
            velocity_max=value.toDouble();
            pro.velocity_max=velocity_max;
        }

        Decimal acceleration_max=pro.acceleration_max;///acceleration_max*
        if(obj_property.contains("acceleration_max"))
        {
            QJsonValue value = obj_property.value("acceleration_max");
            if(!value.isDouble())
                throw QString::asprintf("<ERROR> type of KEY \"property.acceleration_max\" is not correct in FILE %s",path.toStdString().c_str());
            acceleration_max=value.toDouble();
            pro.acceleration_max=acceleration_max;
        }

        Decimal attenuation_velocity=pro.attenuation_velocity;///attenuation_velocity*
        if(obj_property.contains("attenuation_velocity"))
        {
            QJsonValue value = obj_property.value("attenuation_velocity");
            if(!value.isDouble())
                throw QString::asprintf("<ERROR> type of KEY \"property.attenuation_velocity\" is not correct in FILE %s",path.toStdString().c_str());
            attenuation_velocity=value.toDouble();
            pro.attenuation_velocity=attenuation_velocity;
        }

        Decimal offset_front=pro.offset_front;///offset_front*
        if(obj_property.contains("offset_front"))
        {
            QJsonValue value = obj_property.value("offset_front");
            if(!value.isDouble())
                throw QString::asprintf("<ERROR> type of KEY \"property.offset_front\" is not correct in FILE %s",path.toStdString().c_str());
            offset_front=value.toDouble();
            pro.offset_front=offset_front;
        }

        Decimal coefficient_friction=pro.coefficient_friction;///coefficient_friction*
        if(obj_property.contains("coefficient_friction"))
        {
            QJsonValue value = obj_property.value("coefficient_friction");
            if(!value.isDouble())
                throw QString::asprintf("<ERROR> type of KEY \"property.coefficient_friction\" is not correct in FILE %s",path.toStdString().c_str());
            coefficient_friction=value.toDouble();
            pro.coefficient_friction=coefficient_friction;
        }

        ///---------------------------------------------------------------------------数组型


        BinaryVector<Decimal> angular_initial_target=pro.angular_initial_target;///angular_initial_target*
        if(obj_property.contains("angular_initial_target"))
        {
            QJsonValue angular_initial_target_v=obj_property.value("angular_initial_target");
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
        if(obj_property.contains("angular_speed_max"))
        {
            QJsonValue angular_speed_max_v=obj_property.value("angular_speed_max");
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

        ///angular_acc_max*
        if(obj_property.contains("angular_acc_max"))
        {
            QJsonValue value=obj_property.value("angular_acc_max");
            if(!value.isArray())
                throw QString::asprintf("<ERROR> type of KEY \"property.angular_acc_max\" is not correct in FILE %s",path.toStdString().c_str());

            QJsonArray array= value.toArray();

            if(array.size()!=2)
                throw QString::asprintf("<ERROR> VALUE of KEY \"property.angular_acc_max\" is not correct in FILE %s",path.toStdString().c_str());

            QJsonValue first=array[0],second=array[1];

            if(!(first.isDouble()&&second.isDouble()))
                throw QString::asprintf("<ERROR> type of VALUE in KEY \"property.angular_acc_max\" is not correct in FILE %s",path.toStdString().c_str());
            pro.angular_acc_max.first=first.toDouble();
            pro.angular_acc_max.second=second.toDouble();
        }


        ///---------------------------------------------------------------------------字符串型


        QString mode_movement="";///mode_movement*
        if(obj_property.contains("mode_movement"))
        {
            QJsonValue mode_movement_v = obj_property.value("mode_movement");
            if(!mode_movement_v.isString())
                throw QString::asprintf("<ERROR> type of KEY \"property.mode_movement\" is not correct in FILE %s",path.toStdString().c_str());
            mode_movement=mode_movement_v.toString();
            pro.mode_movement=ToolFunctionsKit::string_to_movement_mode(mode_movement);
        }

        QString mode_rotation="";///mode_rotation*
        if(obj_property.contains("mode_rotation"))
        {
            QJsonValue mode_rotation_v = obj_property.value("mode_rotation");
            if(!mode_rotation_v.isString())
                throw QString::asprintf("<ERROR> type of KEY \"property.mode_rotation\" is not correct in FILE %s",path.toStdString().c_str());
            mode_rotation=mode_rotation_v.toString();
            pro.mode_rotation=ToolFunctionsKit::string_to_rotation_mode(mode_rotation);
        }


        ///---------------------------------------------------------------------------布尔型

        bool flag_delete_outside_scene=pro.flag_delete_outside_scene;///flag_delete_outside_scene*
        if(obj_property.contains("flag_delete_outside_scene"))
        {
            QJsonValue flag_delete_outside_scene_v = obj_property.value("flag_delete_outside_scene");
            if(!flag_delete_outside_scene_v.isBool())
                throw QString::asprintf("<ERROR> type of KEY \"property.flag_delete_outside_scene\" is not correct in FILE %s",path.toStdString().c_str());
            flag_delete_outside_scene=flag_delete_outside_scene_v.toBool();
            pro.flag_delete_outside_scene=flag_delete_outside_scene;
        }

        bool flag_boundary_restriction=pro.flag_boundary_restriction;///flag_boundary_restriction*
        if(obj_property.contains("flag_boundary_restriction"))
        {
            QJsonValue flag_boundary_restriction_v = obj_property.value("flag_boundary_restriction");
            if(!flag_boundary_restriction_v.isBool())
                throw QString::asprintf("<ERROR> type of KEY \"property.flag_boundary_restriction\" is not correct in FILE %s",path.toStdString().c_str());
            flag_boundary_restriction=flag_boundary_restriction_v.toBool();
            pro.flag_boundary_restriction=flag_boundary_restriction;
        }

        bool flag_collision=pro.flag_collision;///flag_collision*
        if(obj_property.contains("flag_collision"))
        {
            QJsonValue flag_collision_v = obj_property.value("flag_collision");
            if(!flag_collision_v.isBool())
                throw QString::asprintf("<ERROR> type of KEY \"property.flag_collision\" is not correct in FILE %s",path.toStdString().c_str());
            flag_collision=flag_collision_v.toBool();
            pro.flag_collision=flag_collision;
        }

        ///flag_channel_collision*
        if(obj_property.contains("flag_channel_collision"))
        {
            QJsonValue value = obj_property.value("flag_channel_collision");
            if(!value.isBool())
                throw QString::asprintf("<ERROR> type of KEY \"property.flag_channel_collision\" is not correct in FILE %s",path.toStdString().c_str());
            pro.flag_channel_collision=value.toBool();
        }

        ///flag_mutex*
        if(obj_property.contains("flag_mutex"))
        {
            QJsonValue value = obj_property.value("flag_mutex");
            if(!value.isBool())
                throw QString::asprintf("<ERROR> type of KEY \"property.flag_mutex\" is not correct in FILE %s",path.toStdString().c_str());
            pro.flag_mutex=value.toBool();
        }

    }

    ///property_game
    if(root.contains("property_game"))
    {
        //获取property对象
        QJsonObject obj_property_game = root.value("property_game").toObject();

        auto & pro_g = obj_new.property_gaming;


        ///---------------------------------------------------------------------------数值型

        ///team*
        if(obj_property_game.contains("team"))
        {
            QJsonValue value = obj_property_game.value("team");
            if(!value.isDouble())
                throw QString::asprintf("<ERROR> type of KEY \"property_game.team\" is not correct in FILE %s",path.toStdString().c_str());
            pro_g.team=value.toInt();
        }

        ///score*
        if(obj_property_game.contains("score"))
        {
            QJsonValue value = obj_property_game.value("team");
            if(!value.isDouble())
                throw QString::asprintf("<ERROR> type of KEY \"property_game.score\" is not correct in FILE %s",path.toStdString().c_str());
            pro_g.score=value.toInt();
        }


        ///damage*
        if(obj_property_game.contains("damage"))
        {
            QJsonValue value = obj_property_game.value("damage");
            if(!value.isDouble())
                throw QString::asprintf("<ERROR> type of KEY \"property_game.damage\" is not correct in FILE %s",path.toStdString().c_str());
            pro_g.damage=value.toInt();
        }

        ///resist*
        if(obj_property_game.contains("resist"))
        {
            QJsonValue value = obj_property_game.value("resist");
            if(!value.isDouble())
                throw QString::asprintf("<ERROR> type of KEY \"property_game.resist\" is not correct in FILE %s",path.toStdString().c_str());
            pro_g.resist=value.toInt();
        }

        ///penetrability*
        if(obj_property_game.contains("penetrability"))
        {
            QJsonValue value = obj_property_game.value("penetrability");
            if(!value.isDouble())
                throw QString::asprintf("<ERROR> type of KEY \"property_game.penetrability\" is not correct in FILE %s",path.toStdString().c_str());
            pro_g.penetrability=value.toInt();
        }


        ///---------------------------------------------------------------------------数组型


        ///endurance*
        if(root.contains("endurance"))
        {
            QJsonValue endurance_v=root.value("endurance");
            if(!endurance_v.isArray())
                throw QString::asprintf("<ERROR> type of KEY \"property_game.endurance\" is not correct in FILE %s",path.toStdString().c_str());

            QJsonArray array= endurance_v.toArray();

            if(array.size()!=2)
                throw QString::asprintf("<ERROR> VALUE of KEY \"property_game.endurance\" is not correct in FILE %s",path.toStdString().c_str());

            QJsonValue first=array[0],second=array[1];

            if(!(first.isDouble()&&second.isDouble()))
                throw QString::asprintf("<ERROR> type of VALUE in KEY \"property_game.endurance\" is not correct in FILE %s",path.toStdString().c_str());
            pro_g.endurance.first=first.toDouble();
            pro_g.endurance.second=second.toDouble();
        }


        ///---------------------------------------------------------------------------布尔型

        ///flag_team_kill*
        if(obj_property_game.contains("flag_team_kill"))
        {
            QJsonValue flag_team_kill_v = obj_property_game.value("flag_team_kill");
            if(!flag_team_kill_v.isBool())
                throw QString::asprintf("<ERROR> type of KEY \"property_game.flag_team_kill\" is not correct in FILE %s",path.toStdString().c_str());
            pro_g.flag_team_kill=flag_team_kill_v.toBool();
        }

    }

    //容器中添加一个对象
    objects[name]=obj_new;

}

void ResourcePackage::parse_json__derive_rule(const QString &path)
{
    QFile file;
    file.setFileName(this->path_pkg+path);

    //检查文件是否存在
    if(!file.exists())
        throw QString::asprintf("<ERROR> FILE %s not found",path.toStdString().c_str());

    //打开文件(只读|文本)
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    //读取所有内容
    QString content = file.readAll();
    //关闭文件
    file.close();

    QJsonParseError error_json;//解析错误
    QJsonDocument doc_json;//json文档

    doc_json=QJsonDocument::fromJson(content.toUtf8(),&error_json);

    if(error_json.error!=QJsonParseError::NoError)
        throw QString::asprintf("<ERROR> %s",error_json.errorString().toStdString().c_str());

    QJsonObject root=doc_json.object();//获取根对象


    ///---------------------------------------------------------------------------开始解析

    DeriveRule rule_new;


    Integer id = 0;///id
    if(root.contains("id"))
    {
        QJsonValue value=root.value("id");
        if(!value.isDouble())
            throw QString::asprintf("<ERROR> type of KEY \"id\" is not correct in FILE %s",path.toStdString().c_str());
        id=value.toInt();
    }
    else
        throw QString::asprintf("<ERROR> KEY \"center\" not found in FILE %s",error_json.errorString().toStdString().c_str());

    Integer period = 0;///period
    if(root.contains("period"))
    {
        QJsonValue value=root.value("period");
        if(!value.isDouble())
            throw QString::asprintf("<ERROR> type of KEY \"period\" is not correct in FILE %s",path.toStdString().c_str());
        period=value.toInt();
        rule_new.period=period;
    }
    else
        throw QString::asprintf("<ERROR> KEY \"period\" not found in FILE %s",error_json.errorString().toStdString().c_str());


    ///units
    if(root.contains("units"))
    {
        //获取property对象
        QJsonArray array = root.value("units").toArray();

        for(int i=0,size=array.size();i<size;++i)
        {
            DeriveUnit unit;

            if(!array[i].isObject())
                throw QString::asprintf("<ERROR> structure of KEY \"units\" is not correct in FILE %s",error_json.errorString().toStdString().c_str());

            QJsonObject obj=array[i].toObject();

            QString name;///object
            if(obj.contains("object"))
            {
                QJsonValue value=obj.value("object");
                if(!value.isString())
                    throw QString::asprintf("<ERROR> type of KEY \"units.object\" is not correct in FILE %s",path.toStdString().c_str());
            }
            else
                throw QString::asprintf("<ERROR> KEY \"units.object\" not found in FILE %s",error_json.errorString().toStdString().c_str());

            QString ref_direction;///ref_direction*
            if(obj.contains("ref_direction"))
            {
                QJsonValue value=obj.value("ref_direction");
                if(!value.isString())
                    throw QString::asprintf("<ERROR> type of KEY \"units.ref_direction\" is not correct in FILE %s",path.toStdString().c_str());
                ref_direction=value.toString();
                unit.ref_direction=ToolFunctionsKit::string_to_direction_reference(ref_direction);
                if(unit.ref_direction==DR::None)
                    throw QString::asprintf("<ERROR> value of KEY \"units.ref_direction\" is unknown in FILE %s",path.toStdString().c_str());
            }


            ///flag_inherit_speed*
            if(obj.contains("flag_inherit_speed"))
            {
                QJsonValue value=obj.value("flag_inherit_speed");
                if(!value.isBool())
                    throw QString::asprintf("<ERROR> type of KEY \"units.flag_inherit_speed\" is not correct in FILE %s",path.toStdString().c_str());
                unit.flag_inherit_speed=value.toBool();
            }

            ///flag_relative_position*
            if(obj.contains("flag_relative_position"))
            {
                QJsonValue value=obj.value("flag_relative_position");
                if(!value.isBool())
                    throw QString::asprintf("<ERROR> type of KEY \"units.flag_relative_position\" is not correct in FILE %s",path.toStdString().c_str());
                unit.flag_inherit_speed=value.toBool();
            }


            ///position*
            if(root.contains("position"))
            {
                QJsonValue value=root.value("position");
                if(!value.isArray())
                    throw QString::asprintf("<ERROR> type of KEY \"units.position\" is not correct in FILE %s",path.toStdString().c_str());

                QJsonArray array= value.toArray();

                if(array.size()!=2)
                    throw QString::asprintf("<ERROR> VALUE of KEY \"units.position\" is not correct in FILE %s",path.toStdString().c_str());

                QJsonValue first=array[0],second=array[1];

                if(!(first.isDouble()&&second.isDouble()))
                    throw QString::asprintf("<ERROR> type of VALUE in KEY \"units.position\" is not correct in FILE %s",path.toStdString().c_str());

                unit.position.first=first.toDouble();
                unit.position.second=second.toDouble();
            }

            ///speed*
            if(root.contains("speed"))
            {
                QJsonValue value=root.value("speed");
                if(!value.isArray())
                    throw QString::asprintf("<ERROR> type of KEY \"units.speed\" is not correct in FILE %s",path.toStdString().c_str());

                QJsonArray array= value.toArray();

                if(array.size()!=2)
                    throw QString::asprintf("<ERROR> VALUE of KEY \"units.speed\" is not correct in FILE %s",path.toStdString().c_str());

                QJsonValue first=array[0],second=array[1];

                if(!(first.isDouble()&&second.isDouble()))
                    throw QString::asprintf("<ERROR> type of VALUE in KEY \"units.speed\" is not correct in FILE %s",path.toStdString().c_str());

                unit.speed.first=first.toDouble();
                unit.speed.second=second.toDouble();
            }

            ///acceleration*
            if(root.contains("acceleration"))
            {
                QJsonValue value=root.value("acceleration");
                if(!value.isArray())
                    throw QString::asprintf("<ERROR> type of KEY \"units.acceleration\" is not correct in FILE %s",path.toStdString().c_str());

                QJsonArray array= value.toArray();

                if(array.size()!=2)
                    throw QString::asprintf("<ERROR> VALUE of KEY \"units.acceleration\" is not correct in FILE %s",path.toStdString().c_str());

                QJsonValue first=array[0],second=array[1];

                if(!(first.isDouble()&&second.isDouble()))
                    throw QString::asprintf("<ERROR> type of VALUE in KEY \"units.acceleration\" is not correct in FILE %s",path.toStdString().c_str());

                unit.acceleration.first=first.toDouble();
                unit.acceleration.second=second.toDouble();
            }


            ///rotation*
            if(root.contains("rotation"))
            {
                QJsonValue value=root.value("rotation");
                if(!value.isDouble())
                    throw QString::asprintf("<ERROR> type of KEY \"units.rotation\" is not correct in FILE %s",path.toStdString().c_str());

                unit.rotation=value.toDouble();
            }

            ///rotation_float*
            if(root.contains("rotation_float"))
            {
                QJsonValue value=root.value("rotation_float");
                if(!value.isDouble())
                    throw QString::asprintf("<ERROR> type of KEY \"units.rotation_float\" is not correct in FILE %s",path.toStdString().c_str());

                unit.rotation_float=value.toDouble();
            }

            ///probability*
            if(root.contains("probability"))
            {
                QJsonValue value=root.value("probability");
                if(!value.isDouble())
                    throw QString::asprintf("<ERROR> type of KEY \"units.probability\" is not correct in FILE %s",path.toStdString().c_str());

                unit.probability=value.toDouble();
            }

            //添加到末尾
            rule_new.units<<(unit);
        }


    }

    this->rules_derive[id]=rule_new;//添加到容器中


}



void Game::Scene::set_file_path(const QString &path)
{
    this->path_file=path;
}

void Scene::load()
{
    QFile file;
    file.setFileName(this->path_file);

    //检查文件是否存在
    if(!file.exists())
        throw QString::asprintf("<ERROR> FILE %s not found",file.fileName().toStdString().c_str());

    //打开文件(只读|文本)
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    //读取所有内容
    QString content = file.readAll();
    //关闭文件
    file.close();

    QJsonParseError error_json;//解析错误
    QJsonDocument doc_json;//json文档

    doc_json=QJsonDocument::fromJson(content.toUtf8(),&error_json);

    if(error_json.error!=QJsonParseError::NoError)
        throw QString::asprintf("<ERROR> %s",error_json.errorString().toStdString().c_str());

    QJsonObject root=doc_json.object();//获取根对象

    ///---------------------------------------------------------------------------开始解析

    ///name*
    if(root.contains("name"))
    {
        QJsonValue value=root.value("name");
        if(!value.isString())
            throw QString::asprintf("<ERROR> type of KEY \"name\" is not correct in FILE %s",path_file.toStdString().c_str());
        this->name=value.toString();
    }

    ///description*
    if(root.contains("description"))
    {
        QJsonValue value=root.value("description");
        if(!value.isString())
            throw QString::asprintf("<ERROR> type of KEY \"description\" is not correct in FILE %s",path_file.toStdString().c_str());
        this->description=value.toString();
    }

    ///name_resource
    if(root.contains("name_resource"))
    {
        QJsonValue value=root.value("name_resource");
        if(!value.isString())
            throw QString::asprintf("<ERROR> type of KEY \"name_resource\" is not correct in FILE %s",path_file.toStdString().c_str());
        this->name_resource=value.toString();
    }

    ///image_background
    if(root.contains("image_internal"))
    {
        QJsonValue value=root.value("image_internal");
        if(!value.isString())
            throw QString::asprintf("<ERROR> type of KEY \"image_internal\" is not correct in FILE %s",path_file.toStdString().c_str());
        this->image_internal=value.toString();
    }

    ///image_background
    if(root.contains("image_external"))
    {
        QJsonValue value=root.value("image_external");
        if(!value.isString())
            throw QString::asprintf("<ERROR> type of KEY \"image_external\" is not correct in FILE %s",path_file.toStdString().c_str());
        this->image_external=value.toString();
    }

    ///size
    if(root.contains(QStringLiteral("size")))
    {
        QJsonValue center_v=root.value("size");
        if(!center_v.isArray())
            throw QString::asprintf("<ERROR> type of KEY \"size\" is not correct in FILE %s",path_file.toStdString().c_str());

        QJsonArray array= center_v.toArray();

        if(array.size()!=2)
            throw QString::asprintf("<ERROR> VALUE of KEY \"size\" is not correct in FILE %s",path_file.toStdString().c_str());

        QJsonValue first=array[0],second=array[1];

        if(!(first.isDouble()&&second.isDouble()))
            throw QString::asprintf("<ERROR> type of VALUE in KEY \"size\" is not correct in FILE %s",path_file.toStdString().c_str());
        this->size.rx()=first.toInt();
        this->size.ry()=second.toInt();
    }
    else
        //非可选键未找到, 抛出异常
        throw QString::asprintf("<ERROR> KEY \"size\" not found in FILE %s",path_file.toStdString().c_str());

    ///pos_view*
    if(root.contains(QStringLiteral("pos_view")))
    {
        QJsonValue center_v=root.value("pos_view");
        if(!center_v.isArray())
            throw QString::asprintf("<ERROR> type of KEY \"pos_view\" is not correct in FILE %s",path_file.toStdString().c_str());

        QJsonArray array= center_v.toArray();

        if(array.size()!=2)
            throw QString::asprintf("<ERROR> VALUE of KEY \"pos_view\" is not correct in FILE %s",path_file.toStdString().c_str());

        QJsonValue first=array[0],second=array[1];

        if(!(first.isDouble()&&second.isDouble()))
            throw QString::asprintf("<ERROR> type of VALUE in KEY \"pos_view\" is not correct in FILE %s",path_file.toStdString().c_str());
        this->pos_view.rx()=first.toInt();
        this->pos_view.ry()=second.toInt();
    }

    ///pos_initial*
    if(root.contains(QStringLiteral("pos_initial")))
    {
        QJsonValue center_v=root.value("pos_initial");
        if(!center_v.isArray())
            throw QString::asprintf("<ERROR> type of KEY \"pos_initial\" is not correct in FILE %s",path_file.toStdString().c_str());

        QJsonArray array= center_v.toArray();

        if(array.size()!=2)
            throw QString::asprintf("<ERROR> VALUE of KEY \"pos_initial\" is not correct in FILE %s",path_file.toStdString().c_str());

        QJsonValue first=array[0],second=array[1];

        if(!(first.isDouble()&&second.isDouble()))
            throw QString::asprintf("<ERROR> type of VALUE in KEY \"pos_initial\" is not correct in FILE %s",path_file.toStdString().c_str());
        this->pos_initial.rx()=first.toDouble();
        this->pos_initial.ry()=second.toDouble();
    }


    ///rules_generate
    if(root.contains("rules_generate"))
    {
        //获取property对象
        QJsonArray array = root.value("rules_generate").toArray();

        for(int i=0;i<array.size();++i)
        {
            QJsonObject obj=array[i].toObject();

            std::pair<SceneGenerateRule,ObjectActionProperty> pair;

            if(obj.contains("rule"))
            {
                QJsonObject obj_rule=obj.value("rule").toObject();

                ///flag_in_viewport*
                if(obj_rule.contains("flag_in_viewport"))
                {
                    QJsonValue value = obj_rule.value("flag_in_viewport");
                    if(!value.isBool())
                        throw QString::asprintf("<ERROR> type of VALUE in KEY \"rules_generate.rule.flag_in_viewport\" is not correct in FILE %s",path_file.toStdString().c_str());
                    pair.first.flag_in_viewport=value.toBool();
                }

                ///position*
                if(obj_rule.contains(QStringLiteral("position")))
                {
                    QJsonValue center_v=obj_rule.value("position");
                    if(!center_v.isArray())
                        throw QString::asprintf("<ERROR> type of KEY \"rules_generate.action.position\" is not correct in FILE %s",path_file.toStdString().c_str());

                    QJsonArray array= center_v.toArray();

                    if(array.size()!=2)
                        throw QString::asprintf("<ERROR> VALUE of KEY \"rules_generate.action.position\" is not correct in FILE %s",path_file.toStdString().c_str());

                    QJsonValue first=array[0],second=array[1];

                    if(!(first.isDouble()&&second.isDouble()))
                        throw QString::asprintf("<ERROR> type of VALUE in KEY \"rules_generate.action.position\" is not correct in FILE %s",path_file.toStdString().c_str());
                    pair.first.position.rx()=first.toDouble();
                    pair.first.position.ry()=second.toDouble();
                }

                ///requirement_time*
                if(obj_rule.contains("requirement_time"))
                {
                    QJsonValue value = obj_rule.value("requirement_time");
                    if(!value.isDouble())
                        throw QString::asprintf("<ERROR> type of VALUE in KEY \"rules_generate.rule.requirement_time\" is not correct in FILE %s",path_file.toStdString().c_str());
                    pair.first.requirement_time=value.toInt();
                }

                ///requirement_score*
                if(obj_rule.contains("requirement_score"))
                {
                    QJsonValue value = obj_rule.value("requirement_score");
                    if(!value.isDouble())
                        throw QString::asprintf("<ERROR> type of VALUE in KEY \"rules_generate.rule.requirement_score\" is not correct in FILE %s",path_file.toStdString().c_str());
                    pair.first.requirement_score=value.toInt();
                }

                ///number
                if(obj_rule.contains("number"))
                {
                    QJsonValue value = obj_rule.value("number");
                    if(!value.isDouble())
                        throw QString::asprintf("<ERROR> type of VALUE in KEY \"rules_generate.rule.number\" is not correct in FILE %s",path_file.toStdString().c_str());
                    pair.first.number=value.toInt();
                }

                ///name_object
                if(obj_rule.contains("name_object"))
                {
                    QJsonValue value = obj_rule.value("name_object");
                    if(!value.isString())
                        throw QString::asprintf("<ERROR> type of VALUE in KEY \"rules_generate.rule.name_object\" is not correct in FILE %s",path_file.toStdString().c_str());
                    pair.first.name_object=value.toString();
                }
                else
                    //非可选键未找到, 抛出异常
                    throw QString::asprintf("<ERROR> KEY \"rules_generate.rule.name_object\" not found in FILE %s",path_file.toStdString().c_str());

                ///number
                if(obj_rule.contains("rotation"))
                {
                    QJsonValue value = obj_rule.value("rotation");
                    if(!value.isDouble())
                        throw QString::asprintf("<ERROR> type of VALUE in KEY \"rules_generate.rule.rotation\" is not correct in FILE %s",path_file.toStdString().c_str());
                    pair.first.rotation=value.toDouble();
                }
            }

            if(obj.contains("action"))
            {
                QJsonObject obj_action=obj.value("action").toObject();


                ///flag_player_manip*
                if(obj_action.contains("flag_player_manip"))
                {
                    QJsonValue value = obj_action.value("flag_player_manip");
                    if(!value.isBool())
                        throw QString::asprintf("<ERROR> type of VALUE in KEY \"rules_generate.action.flag_player_manip\" is not correct in FILE %s",path_file.toStdString().c_str());
                    pair.second.flag_player_manip=value.toBool();
                }

                ///flag_auto_target*
                if(obj_action.contains("flag_auto_target"))
                {
                    QJsonValue value = obj_action.value("flag_auto_target");
                    if(!value.isBool())
                        throw QString::asprintf("<ERROR> type of VALUE in KEY \"rules_generate.action.flag_auto_target\" is not correct in FILE %s",path_file.toStdString().c_str());
                    pair.second.flag_auto_target=value.toBool();
                }

                ///flag_anticipation*
                if(obj_action.contains("flag_anticipation"))
                {
                    QJsonValue value = obj_action.value("flag_anticipation");
                    if(!value.isBool())
                        throw QString::asprintf("<ERROR> type of VALUE in KEY \"rules_generate.action.flag_anticipation\" is not correct in FILE %s",path_file.toStdString().c_str());
                    pair.second.flag_anticipation=value.toBool();
                }

                ///frequency_dodge*
                if(obj_action.contains("frequency_dodge"))
                {
                    QJsonValue value = obj_action.value("frequency_dodge");
                    if(!value.isDouble())
                        throw QString::asprintf("<ERROR> type of VALUE in KEY \"rules_generate.action.frequency_dodge\" is not correct in FILE %s",path_file.toStdString().c_str());
                    pair.second.frequency_dodge=value.toDouble();
                }

                ///period_dodge*
                if(obj_action.contains("period_dodge"))
                {
                    QJsonValue value = obj_action.value("period_dodge");
                    if(!value.isDouble())
                        throw QString::asprintf("<ERROR> type of VALUE in KEY \"rules_generate.action.period_dodge\" is not correct in FILE %s",path_file.toStdString().c_str());
                    pair.second.cooldown_dodge.second=value.toInt();
                }


                ///rotate_idle*
                if(obj_action.contains(QStringLiteral("rotate_idle")))
                {
                    QJsonValue center_v=obj_action.value("rotate_idle");
                    if(!center_v.isArray())
                        throw QString::asprintf("<ERROR> type of KEY \"rules_generate.action.rotate_idle\" is not correct in FILE %s",path_file.toStdString().c_str());

                    QJsonArray array= center_v.toArray();

                    if(array.size()!=2)
                        throw QString::asprintf("<ERROR> VALUE of KEY \"rules_generate.action.rotate_idle\" is not correct in FILE %s",path_file.toStdString().c_str());

                    QJsonValue first=array[0],second=array[1];

                    if(!(first.isDouble()&&second.isDouble()))
                        throw QString::asprintf("<ERROR> type of VALUE in KEY \"rules_generate.action.rotate_idle\" is not correct in FILE %s",path_file.toStdString().c_str());
                    pair.second.rotate_idle.first=first.toDouble();
                    pair.second.rotate_idle.second=second.toInt();
                }

                ///move_idle*
                if(obj_action.contains(QStringLiteral("move_idle")))
                {
                    QJsonValue center_v=obj_action.value("move_idle");
                    if(!center_v.isArray())
                        throw QString::asprintf("<ERROR> type of KEY \"rules_generate.action.move_idle\" is not correct in FILE %s",path_file.toStdString().c_str());

                    QJsonArray array= center_v.toArray();

                    if(array.size()!=2)
                        throw QString::asprintf("<ERROR> VALUE of KEY \"rules_generate.action.move_idle\" is not correct in FILE %s",path_file.toStdString().c_str());

                    QJsonValue first=array[0],second=array[1];

                    if(!(first.isDouble()&&second.isDouble()))
                        throw QString::asprintf("<ERROR> type of VALUE in KEY \"rules_generate.action.move_idle\" is not correct in FILE %s",path_file.toStdString().c_str());
                    pair.second.move_idle.first=first.toDouble();
                    pair.second.move_idle.second=second.toInt();
                }


                ///period_idle*
                if(obj_action.contains("period_idle"))
                {
                    QJsonValue value = obj_action.value("period_idle");
                    if(!value.isDouble())
                        throw QString::asprintf("<ERROR> type of VALUE in KEY \"rules_generate.action.period_idle\" is not correct in FILE %s",path_file.toStdString().c_str());
                    pair.second.cooldown_idle.second=value.toInt();
                }

                ///perception*
                if(obj_action.contains("perception"))
                {
                    QJsonValue value = obj_action.value("perception");
                    if(!value.isDouble())
                        throw QString::asprintf("<ERROR> type of VALUE in KEY \"rules_generate.action.perception\" is not correct in FILE %s",path_file.toStdString().c_str());
                    pair.second.perception=value.toDouble();
                }

                ///frequency_attack*
                if(obj_action.contains("frequency_attack"))
                {
                    QJsonValue value = obj_action.value("frequency_attack");
                    if(!value.isDouble())
                        throw QString::asprintf("<ERROR> type of VALUE in KEY \"rules_generate.action.frequency_attack\" is not correct in FILE %s",path_file.toStdString().c_str());
                    pair.second.frequency_attack=value.toDouble();
                }

                ///period_attack*
                if(obj_action.contains("period_attack"))
                {
                    QJsonValue value = obj_action.value("period_attack");
                    if(!value.isDouble())
                        throw QString::asprintf("<ERROR> type of VALUE in KEY \"rules_generate.action.period_attack\" is not correct in FILE %s",path_file.toStdString().c_str());
                    pair.second.cooldown_attack.second=value.toInt();
                }


                ///number_rest_attack*
                if(obj_action.contains("number_rest_attack"))
                {
                    QJsonValue value = obj_action.value("number_rest_attack");
                    if(!value.isDouble())
                        throw QString::asprintf("<ERROR> type of VALUE in KEY \"rules_generate.action.number_rest_attack\" is not correct in FILE %s",path_file.toStdString().c_str());
                    pair.second.number_rest_attack=value.toInt();
                }

                ///duration_attack*
                if(obj_action.contains("duration_attack"))
                {
                    QJsonValue value = obj_action.value("duration_attack");
                    if(!value.isDouble())
                        throw QString::asprintf("<ERROR> type of VALUE in KEY \"rules_generate.action.duration_attack\" is not correct in FILE %s",path_file.toStdString().c_str());
                    pair.second.duration_attack=value.toInt();
                }

                ///float_duration_attack*
                if(obj_action.contains("float_duration_attack"))
                {
                    QJsonValue value = obj_action.value("float_duration_attack");
                    if(!value.isDouble())
                        throw QString::asprintf("<ERROR> type of VALUE in KEY \"rules_generate.action.float_duration_attack\" is not correct in FILE %s",path_file.toStdString().c_str());
                    pair.second.float_duration_attack=value.toDouble();
                }

                ///damage_rate*
                if(obj_action.contains("damage_rate"))
                {
                    QJsonValue value = obj_action.value("damage_rate");
                    if(!value.isDouble())
                        throw QString::asprintf("<ERROR> type of VALUE in KEY \"rules_generate.action.damage_rate\" is not correct in FILE %s",path_file.toStdString().c_str());
                    pair.second.damage_rate=value.toDouble();
                }

                ///resist_rate*
                if(obj_action.contains("resist_rate"))
                {
                    QJsonValue value = obj_action.value("resist_rate");
                    if(!value.isDouble())
                        throw QString::asprintf("<ERROR> type of VALUE in KEY \"rules_generate.action.resist_rate\" is not correct in FILE %s",path_file.toStdString().c_str());
                    pair.second.resist_rate=value.toDouble();
                }

                ///penetrability_rate*
                if(obj_action.contains("penetrability_rate"))
                {
                    QJsonValue value = obj_action.value("penetrability_rate");
                    if(!value.isDouble())
                        throw QString::asprintf("<ERROR> type of VALUE in KEY \"rules_generate.action.penetrability_rate\" is not correct in FILE %s",path_file.toStdString().c_str());
                    pair.second.resist_rate=value.toDouble();
                }

            }

            this->rules_generate.emplace_back(pair);//添加到容器中
        }
    }
    else
        //非可选键未找到, 抛出异常
        throw QString::asprintf("<ERROR> KEY \"rules_generate\" not found in FILE %s",path_file.toStdString().c_str());
}

void Scene::clear()
{
    new (this)Scene();//调用默认构造函数重新构造
}







