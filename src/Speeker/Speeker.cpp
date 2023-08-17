#include <Speeker/Speeker.h>
#include <QTextToSpeech>
#include <QDebug>
#include <QLoggingCategory>

class Speeker::Impl{
public:
    QTextToSpeech* m_speech;
    bool m_enable = true;

    QString toString(QTextToSpeech::State state){
        switch (state) {
        case QTextToSpeech::Ready:
            return "Ready";
        case QTextToSpeech::Speaking:
            return "Speaking";
        case QTextToSpeech::Paused:
            return "Paused";
        case QTextToSpeech::BackendError:
            return "BackendError";
        default:
            return QString("Unkown(%1)").arg(state);
        }
    }

    Impl(){
        m_speech = NULL;

        QLoggingCategory::setFilterRules(QStringLiteral("qt.speech.tts=true \n qt.speech.tts.*=true"));
        qDebug()  <<QStringLiteral("可用%1个语音引擎:[%2]").
                    arg(QTextToSpeech::availableEngines().size()).
                    arg(QTextToSpeech::availableEngines().join(","));
        if(!QTextToSpeech::availableEngines().isEmpty()){

            m_speech = new QTextToSpeech();
            QObject::connect(m_speech,&QTextToSpeech::stateChanged,[=](QTextToSpeech::State state){
                qDebug() << QStringLiteral("语音状态切换到[%1]").arg(toString(state));
            });

            QStringList locales;
            foreach(auto p,m_speech->availableLocales()){
                QString name(QString("%1 (%2)")
                             .arg(QLocale::languageToString(p.language()))
                             .arg(QLocale::countryToString(p.country())));
                locales.append(name);
            }
            qDebug() << QStringLiteral("可用%1个语音区域:[%2]").arg(locales.size()).arg(locales.join(","));

            /// win7 没有locales是这里访问有空指针bug
            if(!locales.isEmpty()){
                QStringList voices;
                foreach(auto v,m_speech->availableVoices()){
                    voices.append(QString("%1 - %2 - %3").arg(v.name())
                                  .arg(QVoice::genderName(v.gender()))
                                  .arg(QVoice::ageName(v.age())));
                }
                qDebug() << QStringLiteral("可用%1个语音音色:[%2]").arg(voices.size()).arg(voices.join(","));
            }

        }
    }
    ~Impl(){
        if(m_speech != NULL){
            m_speech->stop();
            m_speech->deleteLater();
        }
    }
};

Speeker::Speeker(QObject *parent):QObject(parent){
    m_Impl = new Impl();
}

Speeker::~Speeker(){
    delete  m_Impl;
}

void Speeker::setEnable(bool enable)
{
    if(enable != m_Impl->m_enable){
        qDebug() << QStringLiteral("%1语音").arg(enable?"Open":"Close");
        m_Impl->m_enable = enable;
        enableChanged();
    }
}

bool Speeker::isEnable()
{
    return m_Impl->m_enable;
}

void Speeker::speak(const QString& txt){
    speekRequested(txt);

    if(!m_Impl->m_enable){
        qDebug() <<QStringLiteral("【静音播报】：[%1]").arg(txt);
        return;
    }
    qDebug() << QStringLiteral("【语音播报】：[%1]").arg(txt);
    if(m_Impl->m_speech != NULL){
        m_Impl->m_speech->stop();
        m_Impl->m_speech->say(txt);
    } else {
        qWarning() << QStringLiteral("【无法语音播报】：[%1]").arg(txt);
    }
}

QString Speeker::toSpeekString(double value,int precision /*= 2*/)
{
    if(value >= 0){
        return QString::number(value,'f',precision);
    } else {
        return QString("%1%2").arg(QStringLiteral("负")).arg(QString::number(qAbs(value),'f',precision));
    }
}
