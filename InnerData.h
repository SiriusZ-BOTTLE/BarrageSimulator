#ifndef INNERDATA_H
#define INNERDATA_H

#include "Core.h"

#define EC2I(enumclassitem) static_cast<int>(enumclassitem)

//程序内置数据

namespace InnerData
{
using namespace Core;

    //内置对象枚举
    enum InnerObjects
    {
        Block_zero_blue,//方块零 初始方块, 蓝色
        Bullet_type0_blue,//子弹 0类型 蓝色
        Block_zero_red,//方块零 红色
        End_Objects//哨兵
    };
    using IOS=InnerObjects;


    constexpr int SeriesNumber=4;//4个系列
    constexpr int UpgradeLevel=10;//10个等级

    //内置图片路径
    extern QString paths_pixmap[EC2I(InnerObjects::End_Objects)];
    //内置图片
    extern QPixmap* pixmaps_inner_objects[EC2I(InnerObjects::End_Objects)];
    //元素
    extern Element* elements_inner_objects[EC2I(InnerObjects::End_Objects)];
    //内置对象
    extern Core::Objects::HitObject objects_inner[EC2I(InnerObjects::End_Objects)];
    //各种fire规则
    extern DeriveRule rules_fire_inner[SeriesNumber][UpgradeLevel];

    //初始化内置数据
    //使用内置数据前必须进行初始化
    void init_inner_data();
    //释放内部数据
    void release_inner_data();

    void init_block_zero_blue();

    void init_bullet_type0_blue();

    void init_block_zero_red();


}


#endif // INNERDATA_H
