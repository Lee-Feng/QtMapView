/*******************************************************************************
* @file   .hpp
* @brief
*
* ...detail describe
*
* @version   V1.0.0
* @author    LiFeng,
* @date      2020-06-10
*
* @note
*******************************************************************************/

#pragma once

#include <QString>
#include <QImage>
#include <QDebug>
#include <QFileInfo>
#include <QDir>

namespace utiles {

/**
 * @brief The SmartSpilt class
 * 智能分割。用于分割文字和字符串。
 */
class ThumbImage{
public:
    /**
     * @brief ThumbImage
     * @param path  原始图片路径
     */
    ThumbImage(QString path){
        m_path = path;
        setThumbSize(128,128);
    }

    /**
     * @brief ThumbImage
     * @param path   原始图片路径
     * @param width  缩率图容量宽度，内部会以能放进这个框的最大缩放方式缩小
     * @param height 缩率图容量高度
     */
    ThumbImage(QString path,int width,int height){
        m_path = path;
        setThumbSize(width,height);
    }

    /**
     * @brief setThumbSize
     * @param width  缩率图容量宽度，内部会以能放进这个框的最大缩放方式缩小
     * @param height 缩率图容量高度
     */
    void setThumbSize(int width,int height){
        m_width = width;
        m_height = height;
    }

    /**
     * @brief loadThumb
     * 加载缩率图，如果没有就创建
     * @return
     */
    QImage loadThumb(){
        QString thumfile = getThumbFilepath();
        QFileInfo thumInfo(thumfile);
        if(thumInfo.exists()){
            return QImage(thumfile);
        }
        QImage ret;
        createThumb(ret);
        return ret;
    }

    /**
     * @brief loadImage
     * 加载原始图片
     * @return
     */
    QImage loadImage(){
        return QImage(m_path);
    }

    /**
     * @brief getThumbFilepath
     * 获取缩率图保存路径
     * @return
     */
    QString getThumbFilepath(){
        QFileInfo info(m_path);
        /// 缩率图文件放在图片文件目录的.thumb目录下
        /// -bigImage1.jpg
        /// -bigImage2.jpg
        /// -bigImage3.jpg
        /// -.thumb
        ///     |-bigImage1.jpg
        ///     |-bigImage2.jpg
        QString thumfile = QString("%1/.thumb/%2").arg(info.path()).arg(info.fileName());
        return thumfile;
    }

    /**
     * @brief createThumb
     * 创建tumb文件
     * @return
     */
    bool createThumb(QImage& thumb){
        QImage source = loadImage();
        return createThumbFromImage(source,thumb);
    }

    /**
     * @brief createThumbFromImage
     * 创建缩略图
     * @param src
     * @param thumb
     * @return
     */
    bool createThumbFromImage(const QImage& src,QImage& thumb){
        QString tumbfile = getThumbFilepath();
        thumb = src.scaled(m_width,m_height, Qt::KeepAspectRatio, Qt::FastTransformation);
        QFileInfo info(tumbfile);
        if(!info.exists()){
            QDir d;
            d.mkpath(info.path());
            qDebug() << "New Thumb:"<<tumbfile;
        }
        return thumb.save(tumbfile);
    }

    /**
     * @brief createThumb
     * 创建tumb文件
     * @return
     */
    bool createThumb(){
        QImage thumb;
        return createThumb(thumb);
    }

protected:
    int m_width = 0;
    int m_height = 0;
    QString m_path;
};

}
