#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_shifr_clicked();
    void encrypt();
    int find(QString word);
    void decoder();
    void on_un_shif_clicked();
    int det_3(int matrx[3][3]);
    int det_2(int matrx[2][2]);
    int Evklid(int a, int b);
    int Eva_extended(int a, int b, int & x, int & y);
    bool check_det(int key_matrx[3][3]);
private:
    Ui::MainWindow *ui;
    QString key;
    QString message;
    QString shifr;
};
#endif // MAINWINDOW_H
