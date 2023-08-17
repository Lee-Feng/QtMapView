/*******************************************************************************
* @file   .hpp
* @brief
*
* ...detail describe
*
* @version   V1.0.0
* @author    LiFeng,
* @date      2016-05-12
*
* @note
*******************************************************************************/

#pragma once
#include <QLayout>
#include <QStackedWidget>
#include <QTabWidget>

namespace utiles {

/**
 * @brief removeAll
 * 移除并删除layout里面的所有控件
 * @param layout
 */
static inline void removeAll(QLayout* layout,bool delete_flg = true){
    QLayoutItem* item = NULL;
    while(item = layout->takeAt(0)){
        if(delete_flg){
            /// 必须删除，不然对应widget就绝对定位了
            delete item->widget();
            delete item;
        }
    }
}

/**
 * @brief removeAll
 * 移除并删除container中的所有元素
 * @param container
 * @param delete_flg
 */
static inline void removeAll(QStackedWidget* container,bool delete_flg = true){
    QWidget* item = NULL;
    while(item = container->widget(0)){
        if(delete_flg){
            delete item;
        } else {
            container->removeWidget(item);
        }
    }
}

/**
 * @brief removeAll
 * 移除并删除container中的所有元素
 * @param container
 * @param delete_flg
 */
static inline void removeAll(QTabWidget* container,bool delete_flg = true){
    QWidget* item = NULL;
    while(item = container->widget(0)){
        if(delete_flg){
            delete item;
        } else {
            container->removeTab(0);
        }
    }
    container->clear();
}


}
