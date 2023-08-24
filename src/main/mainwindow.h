#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProcess>
#include <memory>

namespace Ui {
class MainWindow;
}

class QtMapView;
class QtMapItem;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_actionmoveToP1_triggered();

    void on_actionmoveToP2_triggered();

    void on_actionremoveOneNode_triggered();

    void on_actionaddOneNode_triggered();

    void on_actionradmonAdd_triggered();

    void on_actionremoveAll_triggered();

private:
    Ui::MainWindow *ui;

    QPointF p1;
    QPointF p2;
    QtMapView* m_map;
};

#endif // MAINWINDOW_H
