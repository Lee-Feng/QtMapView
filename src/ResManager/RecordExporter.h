#include <QObject>
#include <QVariant>
#include <QVariantList>

class RecordExporter : public QObject{
    Q_OBJECT
public:
    static RecordExporter* getExporter();

    /**
     * @brief exportDatas
     * 异步导出数据，导出过程中将通知：exportProcessing
     * 导出失败将通知：exportError
     * @param files
     * @param dir
     */
    void exportDatas(const QVariantList &files, QString dir);

    /**
     * @brief stopExporting
     * 停止当前正在导出的操作
     */
    void stopExporting();

private:
    bool doExport(const QVariantList &files, QString dir);
signals:
    /**
     * @brief exportProcessing
     * 导出过程中通知,0就是刚开始，100是已经导出完成
     * @param process [0,100]
     */
    void exportProcessing(int process,QString msg);
    /**
     * @brief exportError
     * 导出失败
     * @param error
     */
    void exportError(QString error);

private:
    RecordExporter();
    ~RecordExporter();
    bool m_exporting = false;
};
