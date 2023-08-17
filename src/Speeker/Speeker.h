#pragma once

#include <QObject>
#include <QImage>

/**
 * @brief The Speeker class
 * 对于安卓需要添加权限：

<queries>
      <intent>
         <action android:name="android.intent.action.TTS_SERVICE" />
      </intent>
</queries>

 */
class Speeker : public QObject
{
    Q_OBJECT
    Q_PROPERTY(double enable READ isEnable WRITE setEnable NOTIFY enableChanged)

public:
    Speeker(QObject* parent = NULL);
    ~Speeker();

public slots:
    /**
     * @brief setEnable
     * 是否使能，false就不会发声
     * @param enable
     */
    void setEnable(bool enable);

    /**
     * @brief isEnable
     * @return
     */
    bool isEnable();

    /**
     * @brief speak
     * @param txt
     */
    void speak(const QString& txt);

    /**
     * @brief toSpeekString
     * @param value
     * @return
     */
    QString toSpeekString(double value,int precision = 2);

signals:
    void enableChanged();
    void speekRequested(QString txt);
private:
    Speeker(Speeker&);
    Speeker& operator = (Speeker& src);
protected:
    class Impl;
    Impl* m_Impl;
};
