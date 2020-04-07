#include "universal.h"

namespace InnerData
{
//定义

QString paths_pixmap[EC2I(InnerObjects::End_Objects)]=
{
    "./data/images/block_zero_blue.png",
    "./data/images/bullet_type0_blue.png",
    "./data/images/block_zero_red.png"
};


QPixmap* pixmaps_inner_objects[EC2I(IOS::End_Objects)];
Element* elements_inner_objects[EC2I(InnerObjects::End_Objects)];
Objects::FlyingObject objects_inner[EC2I(IOS::End_Objects)];
DeriveRule rules_fire_inner[SeriesNumber][UpgradeLevel];

void init_inner_data()
{

//    qDebug()<<"init_inner_data()";

    ///初始化图片对象
//    pixmaps_inner_objects[EC2I(IOS::Block_zero_blue)]=new QPixmap(paths_pixmap[IOS::Block_zero_blue]);
//    pixmaps_inner_objects[EC2I(IOS::Bullet_type0_blue)]=new QPixmap(paths_pixmap[IOS::Bullet_type0_blue]);
//    pixmaps_inner_objects[EC2I(IOS::Block_zero_red)]=new QPixmap(paths_pixmap[IOS::Block_zero_red]);

    for(int i=0;i<IOS::End_Objects;++i)
        pixmaps_inner_objects[i]=new QPixmap(paths_pixmap[i]);


    ///初始化显示元素
//    elements_inner_objects[EC2I(IOS::Block_zero_blue)]=new Element(*pixmaps_inner_objects[IOS::Block_zero_blue]);
//    elements_inner_objects[EC2I(IOS::Bullet_type0_blue)]=new Element(*pixmaps_inner_objects[IOS::Bullet_type0_blue]);

    for(int i=0;i<IOS::End_Objects;++i)
    {
        elements_inner_objects[i]=new Element(*pixmaps_inner_objects[i]);
    }

    ///初始化派生对象
    init_bullet_type0_blue();

    ///初始化操纵对象
    init_block_zero_blue();
    init_block_zero_red();

    ///初始化fire规则
    rules_fire_inner[0][0]=
    {
        15,//十个数据更新周期可以生成执行一次生成操作
        {
//            {
//                objects_inner+IOS::Bullet_type0_blue,
//                DR::RelativeToParentDirection,
//                false,//不继承父对象速度
//                true,//相对位置
//                {0,-50},//初始位置
//                {0,-1},//初始速度
//                {0,-1},//初始加速度
//                0,//初始角度
//                0.02//角度浮动
//            },
//            {
//                objects_inner+IOS::Bullet_type0_blue,
//                DR::RelativeToParentRotation,
//                false,//不继承父对象速度
//                true,//相对位置
//                {0,-50},//初始位置
//                {0,-1},//初始速度
//                {0,-1},//初始加速度
//                0,//初始角度
//                0.02//角度浮动
//            }
        }
    };


//用于测试是否聚合
//    qDebug()<<std::is_aggregate<ObjectBasicProperty>::value;
//    QVector<int> v{1,2,3};

}

void init_block_zero_blue()
{
    //初始化 方块零 block_zero
    auto &tmp =objects_inner[IOS::Block_zero_blue];
//    tmp=new ManipulableObject();
    tmp.set_name("block_zero_blue");
    tmp.set_element(elements_inner_objects[IOS::Block_zero_blue]);//默认居中

    tmp.property.channel_collision=PlayerManipulation;//player操纵
    tmp.property.flag_channel_collision=true;//同频道碰撞
    tmp.property.number_rest_collision=-1;//碰撞次数(负数代表无限)
    tmp.property.flag_delete_outside_scene=false;//出场景析构
    tmp.property.mass=100;//质量
    tmp.property.lifetime=-1;
    tmp.property.flag_boundary_restriction=true;//边界限制开启
    tmp.property.velocity_max=15;//最大速率限制
    tmp.property.attenuation_velocity=0.4;//速度衰减
    tmp.property.acceleration_max=0.4;//最大加速度
    tmp.property.mode_movement=MovementMode::Stop;//自动停止(根据按键设置)
    tmp.property.mode_rotation=RotationMode::TowardsMouse;//跟随鼠标
    tmp.property.offset_front=90;//正面偏移量
    tmp.property.angular_speed_max.second=10;//最大角速度
    tmp.property.angular_acc_max.second=0.2;//最大角加速度
//    tmp.property.rule=&(rules_fire_inner[0][0]);//生成规则

    tmp.property_gaming.team=InnerTeam::PlayerTeam;//队伍
    tmp.property_gaming.damage=5;//碰撞伤害
    tmp.property_gaming.resist=10;//抵抗
    tmp.property_gaming.endurance={100,100};//耐久
    tmp.property_gaming.penetrability=0;//穿透
    tmp.property_gaming.flag_team_kill=false;//友伤

    tmp.initialize();//初始化(计算一些必要数据)
//    tmp_sub->property_gaming.
}

void init_bullet_type0_blue()
{
    //初始化 蓝色类型0子弹 Bullet_type0_blue
    auto &tmp =objects_inner[IOS::Bullet_type0_blue];
    tmp=objects_inner[IOS::Bullet_type0_blue];
//    tmp=new ManipulableObject();
    tmp.set_name("Bullet_type0_blue");
    tmp.set_element(elements_inner_objects[IOS::Bullet_type0_blue]);

    tmp.property.channel_collision=PlayerManipulation;//player操纵
    tmp.property.flag_channel_collision=false;//同频道碰撞
    tmp.property.number_rest_collision=2;//碰撞次数
    tmp.property.mass=5;//质量
    tmp.property.lifetime=200;
    tmp.property.flag_boundary_restriction=false;//边界限制关闭
    tmp.property.flag_delete_outside_scene=false;
    tmp.property.velocity_max=15;//最大速率限制
    tmp.property.attenuation_velocity=0.0;//速度衰减
    tmp.property.acceleration_max=0;//最大加速度
    tmp.property.mode_movement=MovementMode::Unlimited;//无限制(根据按键设置)
    tmp.property.mode_rotation=RotationMode::FollowSpeed;//跟随速度方向
    tmp.property.offset_front=90;//正面角度偏移量

//    tmp.property_gaming.index_bullet_series=0;//0系列
//    tmp.property_gaming.index_bullet_level=0;//等级0

    tmp.property_gaming.team=InnerTeam::PlayerTeam;//队伍
    tmp.property_gaming.damage=10;//碰撞伤害
    tmp.property_gaming.resist=100;//抵抗
    tmp.property_gaming.endurance={100,100};//耐久
    tmp.property_gaming.penetrability=5;//穿透
    tmp.property_gaming.flag_team_kill=false;//友伤

    tmp.initialize();//初始化(计算一些必要数据)
}

void init_block_zero_red()
{
    //初始化 方块零 block_zero_red
    auto &tmp =objects_inner[IOS::Block_zero_red];
//    tmp=new ManipulableObject();
    tmp.set_name("Block_zero_red");
    tmp.set_element(elements_inner_objects[IOS::Block_zero_red]);

    tmp.property.channel_collision=ProgramManipulation;//程序操纵
    tmp.property.flag_channel_collision=true;//同频道碰撞
    tmp.property.number_rest_collision=-1;//碰撞次数(负数代表无限)
    tmp.property.mass=100;//质量
    tmp.property.lifetime=-1;
    tmp.property.flag_boundary_restriction=true;//边界限制开启
    tmp.property.velocity_max=15;//最大速率限制
    tmp.property.attenuation_velocity=0.4;//速度衰减
    tmp.property.acceleration_max=0.4;//最大加速度
    tmp.property.mode_movement=MovementMode::Stop;//自动停止(根据按键设置)
    tmp.property.mode_rotation=RotationMode::Stop;//跟随鼠标
    tmp.property.offset_front=90;//正面偏移量
    tmp.property.angular_speed_max.second=10;//最大角速度
    tmp.property.angular_acc_max.second=0.2;//最大角加速度
//    tmp.property.rule=&(rules_fire_inner[0][0]);//生成规则

    tmp.property_gaming.team=InnerTeam::ComputerTeam;//队伍
    tmp.property_gaming.damage=5;//碰撞伤害
    tmp.property_gaming.resist=10;//抵抗
    tmp.property_gaming.endurance={100,100};//耐久
    tmp.property_gaming.penetrability=0;//穿透
    tmp.property_gaming.flag_team_kill=false;//友伤

    tmp.initialize();//初始化(计算一些必要数据)
}



void release_inner_data()
{
    for(int i=0;i<End_Objects;++i)
    {
        if(pixmaps_inner_objects[i])
        {
            delete pixmaps_inner_objects[i];
            pixmaps_inner_objects[i]=nullptr;
        }
    }
}


}










