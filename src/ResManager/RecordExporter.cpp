#include <ResManager/RecordExporter.h>
#include <ResManager/ResManager.h>
#include <QtConcurrent>
#include <Utiles/SimpleDatabaseConnectManager.hpp>
#include <QDebug>
#include <BasicExcel/BasicExcel.hpp>
#include <QDir>

RecordExporter::RecordExporter():QObject(NULL){
}
RecordExporter::~RecordExporter(){

}

RecordExporter* RecordExporter::getExporter(){
    static RecordExporter g;
    return &g;
}

void RecordExporter::exportDatas(const QVariantList &files, QString dir){
    if(m_exporting){
        exportError(QStringLiteral("当前正在导出，不能启动新的导出"));
        return;
    }
    if(files.isEmpty()){
        exportError(QStringLiteral("没有选择要导出的文件"));
        return;
    }

    if(dir.isEmpty()){
        dir = ResManager::getManager()->getWriteableDir() + "/exported";
        qWarning() << QStringLiteral("当前没有指定导出路径，默认使用[%1]").arg(dir);
    }
    QDir p(dir);
    dir = p.absolutePath();
    m_exporting = true;
    QtConcurrent::run([=](){
        qDebug() << QStringLiteral("导出到(%1)...").arg(dir);
        bool suc = doExport(files,dir);
        qDebug() << QStringLiteral("导出到(%1)[%2]").arg(dir).arg(suc?"OK":"FAILD");
        m_exporting = false;
    });
}

void RecordExporter::stopExporting(){
    m_exporting = false;
}

static inline QString getSuffix(QString file){
    QFileInfo f(file);
    if(f.suffix().isEmpty()){
        return "";
    }
    return QString(".%1").arg(f.suffix());
}

/// exported/2020-10-27_09-17-30-619/正常图片/线路名字_方向_2020-10-22/站区名字-隧道名字-杆号-<文件名字>.<后缀>
bool RecordExporter::doExport(const QVariantList &files, QString dir){
    int process = 0;
    exportProcessing(process,QStringLiteral("数据准备中"));
    //QThread::msleep(1000);
    process ++;

    ResManager* res = ResManager::getManager();
    QStringList fileNames;
    foreach(const auto& p,files){
        QString fileBaseName = res->getFileBaseName(p.toString());
        fileNames.append(QString("'%1'").arg(fileBaseName));
    }

    QString now = QDateTime::currentDateTime().toString("yyyy-MM-dd_hh-mm-ss-zzz");
    dir = dir + "/" + now;

    utiles::DatabaseInfo connect_info;
    connect_info.type = res->getHistoryDatabaseDriver().toString();
    connect_info.database = res->getHistoryDatabaseName();
    connect_info.connectname = "catenary_monitor_history";
    QString table = res->getHistoryDatabaseTable();

    utiles::SimpleDatabaseConnectManager  mgr(connect_info);
    utiles::SqlQuery queryCover(&mgr);
    QSqlQuery& query = queryCover.getQSqlQuery();
    ///
    QString sql = QString("SELECT count(id) as exported_count FROM %1 where file_name in (%2)").arg(table).arg(fileNames.join(","));
    qDebug() << sql;
    if(!query.exec(sql)){
        QString err = QStringLiteral("导出失败,提取记录出错无法获取数据总数。\n%1").arg(query.lastError().text());
        qWarning() << err;
        exportError(err);
        return false;
    }
    query.next();
    int count = query.value(0).toInt();

    sql = QString("SELECT * FROM %1 where file_name in (%2)").arg(table).arg(fileNames.join(","));
    qDebug() << sql;
    if(!query.exec(sql)){
        QString err = QStringLiteral("导出失败,提取记录出错。\n%1").arg(query.lastError().text());
        qWarning() << err;
        exportError(err);
        return false;
    }

    int col = 0;
    int row = 0;
    using namespace YExcel;
    BasicExcel excle;
    excle.New(1);
    BasicExcelWorksheet* sheet = excle.GetWorksheet(size_t(0));
    if(sheet == NULL){
        QString err = QStringLiteral("导出失败,创建Worksheet失败。");
        qWarning() << err;
        exportError(err);
        return false;
    }
    /// 创建表头
    sheet->Cell(row,col)->SetWString(QStringLiteral("记录编号").toStdWString().c_str());col++;
    sheet->Cell(row,col)->SetWString(QStringLiteral("文件名").toStdWString().c_str());col++;
    sheet->Cell(row,col)->SetWString(QStringLiteral("标记文件名").toStdWString().c_str());col++;
    sheet->Cell(row,col)->SetWString(QStringLiteral("标记结果").toStdWString().c_str());col++;
    sheet->Cell(row,col)->SetWString(QStringLiteral("线路名称").toStdWString().c_str());col++;
    sheet->Cell(row,col)->SetWString(QStringLiteral("线路方向").toStdWString().c_str());col++;
    sheet->Cell(row,col)->SetWString(QStringLiteral("站区").toStdWString().c_str());col++;
    sheet->Cell(row,col)->SetWString(QStringLiteral("隧道").toStdWString().c_str());col++;
    sheet->Cell(row,col)->SetWString(QStringLiteral("杆号").toStdWString().c_str());col++;
    sheet->Cell(row,col)->SetWString(QStringLiteral("GPS可用性").toStdWString().c_str());col++;
    sheet->Cell(row,col)->SetWString(QStringLiteral("GPS纬度").toStdWString().c_str());col++;
    sheet->Cell(row,col)->SetWString(QStringLiteral("GPS经度").toStdWString().c_str());col++;
    sheet->Cell(row,col)->SetWString(QStringLiteral("GPS高度").toStdWString().c_str());col++;
    sheet->Cell(row,col)->SetWString(QStringLiteral("北斗可用性").toStdWString().c_str());col++;
    sheet->Cell(row,col)->SetWString(QStringLiteral("北斗纬度").toStdWString().c_str());col++;
    sheet->Cell(row,col)->SetWString(QStringLiteral("北斗经度").toStdWString().c_str());col++;
    sheet->Cell(row,col)->SetWString(QStringLiteral("北斗高度").toStdWString().c_str());col++;
    sheet->Cell(row,col)->SetWString(QStringLiteral("记录时间").toStdWString().c_str());col++;


    QMap<QString,QString> resultDir;
    resultDir[QStringLiteral("正常")] = QStringLiteral("正常图片");
    resultDir[QStringLiteral("异常")] = QStringLiteral("异常图片");
    bool suc = true;
    while(query.next()){
        row ++;
        col = 0;
        QString fileName = query.value("file_name").toString();
        QString lineName = query.value("line_name").toString();
        QString lineDir = query.value("line_dir").toString();
        QString station = query.value("station").toString();
        QString tunnel = query.value("tunnel").toString();
        QString backbone_name = query.value("backbone_name").toString();
        if(lineName.isEmpty()){
            lineName = QStringLiteral("未知线路");
        }
        if(lineDir.isEmpty()){
            lineDir = QStringLiteral("未知方向");
        }
        QString result = query.value("result").toString().trimmed();
        QDateTime date = query.value("update_time").toDateTime();
        sheet->Cell(row,col)->SetInteger(query.value("id").toInt());col++;
        sheet->Cell(row,col)->SetWString(fileName.toStdWString().c_str());col++;
        sheet->Cell(row,col)->SetWString(query.value("marked_file_name").toString().toStdWString().c_str());col++;
        sheet->Cell(row,col)->SetWString(result.toStdWString().c_str());col++;
        sheet->Cell(row,col)->SetWString(lineName.toStdWString().c_str());col++;
        sheet->Cell(row,col)->SetWString(lineDir.toStdWString().c_str());col++;
        sheet->Cell(row,col)->SetWString(station.toStdWString().c_str());col++;
        sheet->Cell(row,col)->SetWString(tunnel.toStdWString().c_str());col++;
        sheet->Cell(row,col)->SetWString(backbone_name.toStdWString().c_str());col++;
        sheet->Cell(row,col)->SetInteger(query.value("gps_coord_valid").toInt());col++;
        sheet->Cell(row,col)->SetDouble(query.value("gps_latitude").toDouble());col++;
        sheet->Cell(row,col)->SetDouble(query.value("gps_longitude").toDouble());col++;
        sheet->Cell(row,col)->SetDouble(query.value("gps_altitude").toDouble());col++;
        sheet->Cell(row,col)->SetInteger(query.value("bds_coord_valid").toInt());col++;
        sheet->Cell(row,col)->SetDouble(query.value("bds_latitude").toDouble());col++;
        sheet->Cell(row,col)->SetDouble(query.value("bds_longitude").toDouble());col++;
        sheet->Cell(row,col)->SetDouble(query.value("bds_altitude").toDouble());col++;
        sheet->Cell(row,col)->SetWString(date.toString("yyyy-MM-dd hh:mm:ss,zzz").toStdWString().c_str());col++;
        if(!m_exporting){
            suc = false;
            QString err = QStringLiteral("导出失败,操作被中断。");
            qWarning() << err;
            exportError(err);
            break;
        }

        /// 进度在[2~98之间]
        double percent = 2 + (((double)row) / ((double)count)) * 97.0;
        if(percent>=98){
            percent=98;
        }

        QString dateString = date.toString("yyyy-MM-dd");
        QString destNamePrefix = QString("%1-%2-%3").arg(station).arg(tunnel).arg(backbone_name);
        QString dirName = QString("%1_%2_%3").arg(lineName).arg(lineDir).arg(dateString);
        /// 正常图片，异常图片，XXX图片
        QString groupDir = resultDir.value(result,QStringLiteral("%1图片").arg(result));
        QString dstPath = QStringLiteral("%1/%2/%3").arg(dir).arg(groupDir).arg(dirName);
        QDir d;
        d.mkpath(dstPath);
        /// 同baseName的全部一起拷贝
        foreach(auto p,res->getSameBaseNameFiles(fileName)){
            QString fileSrcAbsPath = res->getFileAbsPath(p);
            QString destPath = QString("%1/%2-%3%4").arg(dstPath).arg(destNamePrefix).arg(fileName).arg(getSuffix(fileSrcAbsPath));
            if(!QFile::copy(fileSrcAbsPath,destPath)){
                qWarning() << QStringLiteral("拷贝文件失败\n(%1\n --> \n%2)").arg(fileSrcAbsPath).arg(destPath);
            }
            exportProcessing(percent,destPath);
            //QThread::msleep(1000);
        }

    }
    if(!suc){
        return false;
    }


    QDir mk;
    mk.mkpath(dir);
    QString excleFile = QStringLiteral("%1/导出记录表-%2.xls").arg(dir).arg(now);
    exportProcessing(99,QStringLiteral("生成记录:%1").arg(excleFile));
    //QThread::msleep(1000);
    qDebug() << QStringLiteral("导出文件:") << excleFile;
    if(!excle.SaveAs(qPrintable(excleFile))){
        QString err = QStringLiteral("导出失败,保存excel记录失败。");
        qWarning() << err;
        exportError(err);
        return false;
    }
    //QThread::msleep(1000);
    exportProcessing(100,QStringLiteral("导出完成"));
    return true;
}

