#include "universal.h"


namespace Settings
{
    int interval=10;//两次画面刷新之间的间隔(ms做单位)(10ms更新一次)
    Integer period_data_update=1;//周期(10ms更新一次)
    bool flag_highlight=true;//受击高亮
    Integer time_highlight=4;//高亮持续刻数
    bool flag_music=false;//音乐开关
    bool flag_sound=false;//音效开关

    int width_interface=500;
    int height_interface=200;

    int width_gaming=1200;
    int height_gaming=800;

    int width_button=200;//按钮统一宽度
    int height_button=50;//按钮统一高度

    int width_gaming_menu=300;//菜单宽度

    int count_frames=100;

    int size_font=30;//字体大小

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

    //容器中添加一个对象
    objects.resize(objects.size()+1);
    FlyingObject & ref_obj_new = objects.back();

    QJsonObject obj_json_root=doc_json.object();//获取根对象

    QStringList keys = obj_json_root.keys();//获取所有键

    if(obj_json_root.contains("name"))//名称键(*)
    {
        QJsonValue name = obj_json_root.value("name");
        if(!name.isString())
            throw QString::asprintf("<ERROR> KEY \"name\" type is not correct in FILE %s",path.toStdString().c_str());
        ref_obj_new.set_name(name.toString());
    }


    QPointF center{-1.0,-1.0};
    if(obj_json_root.contains("center"))//图片路径
    {
        QJsonValue center=obj_json_root.value("center");
        if(!center.isArray())
            throw QString::asprintf("<ERROR> KEY \"center\" type is not correct in FILE %s",path.toStdString().c_str());

        QJsonArray array= center.toArray();

        if(array.size()!=2)
            throw QString::asprintf("<ERROR> VALUE of KEY \"center\" is not correct in FILE %s",path.toStdString().c_str());
    }
    else
        //非可选键未找到, 抛出异常
        throw QString::asprintf("<ERROR> KEY \"center\" not found in FILE %s",error_json.errorString().toStdString().c_str());



    if(obj_json_root.contains("file_path"))//图片路径
    {
        QJsonValue file_path = obj_json_root.value("file_path");
        if(!file_path.isString())
            throw QString::asprintf("<ERROR> KEY \"file_path\" type is not correct in FILE %s",path.toStdString().c_str());

        pixmaps.resize(pixmaps.size()+1);//容器中添加一个对象
        pixmaps.back().load(file_path.toString());//加载图片

        if(center.x()<0)
            elements.emplace_back(pixmaps.back(),1,2);//原地构造

    }
    else
        //非可选键未找到, 抛出异常
        throw QString::asprintf("<ERROR> KEY \"file_path\" not found in FILE %s",error_json.errorString().toStdString().c_str());


}
























