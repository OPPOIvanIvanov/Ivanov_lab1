#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QMessageBox"
#include <QStandardItemModel>
#include "cmath"
#include "QTableView"
#include <QtDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // для однозначного декодирования алфавит должен быть простой длины, поэтому были добавлены некоторые символы
    ui->abcd->setText("абвгдеёжзийклмнопрстуфхцчшщъыьэюя., ?");
    ui->spinBox->setValue(0);
}

MainWindow::~MainWindow()
{
    delete ui;
}

int MainWindow::det_2(int matrx[2][2])
{
     return (matrx[0][0] * matrx[1][1] - matrx[0][1] * matrx[1][0]);
}

int MainWindow::det_3(int matrx[3][3])
{
    int d = 0;
       d += matrx[0][0] * matrx[1][1] * matrx[2][2];
       d += matrx[0][1] * matrx[1][2] * matrx[2][0];
       d += matrx[1][0] * matrx[2][1] * matrx[0][2];

       d -= matrx[0][2] * matrx[1][1] * matrx[2][0];
       d -= matrx[0][1] * matrx[1][0] * matrx[2][2];
       d -= matrx[0][0] * matrx[1][2] * matrx[2][1];
       return d;
}

int MainWindow::Eva_extended(int a, int b, int & x, int & y)
{
    a = abs(a);
    if (a == 0) {
            x = 0; y = 1;
            return b;
        }
        int x1, y1;
        int d = Eva_extended(b%a, a, x1, y1);
        x = y1 - (b / a) * x1;
        y = x1;
        return abs(d);
}
int MainWindow::Evklid(int a, int m)
{
    int x, y, bb;
    int g = Eva_extended(a, m, x, y);
    if (g != 1)
    {
        bb = 0;
        return bb;
    }
    else
    {
        x = (x % m + m) % m;
        return x;
    }
}

bool MainWindow::check_det(int key_matrx[3][3])
{
    int determinant = det_3(key_matrx);
    if (determinant == 0)
    {
        QMessageBox::warning(this, "Внимание","Неверный ключ. Детерминант матрицы ключа не имеет обратного элемента по модулю");
        return false;
    }
    int bb = Evklid(determinant, ui->abcd->text().size());
    if (bb == 0)
    {
        QMessageBox::warning(this, "Внимание","Неверный ключ. Детерминант матрицы ключа не имеет обратного элемента по модулю");
        return false;
    }
    return true;
}

void MainWindow::decoder()
{
    int alp_size = ui->abcd->text().size();
    int key_matrix[3][3];
    int k = 0;
    // заполнение матрицы ключа
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            key_matrix[i][j] = find(key.at(k));
            ui->key_table_2->setItem(i, j, new QTableWidgetItem(QString::number(key_matrix[i][j])));
            k++;
        }
    }
    k = 0;
    // заполнение матрицы сообщения
    if (check_det(key_matrix))
    {
    int unshif_matrix[message.size()/3][3];
    for (int i = 0; i < message.size()/3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            unshif_matrix[i][j] = find(message.at(k));
            k++;
        }
    }
    int determinant = det_3(key_matrix); // ищет определитель третьего порядка
    int x = Evklid(determinant, alp_size); // с помощью расширенного алгоритма Евклида ищем число обратное по модулю
    if (determinant < 0)
            x = x*(-1);
    //тут получается обратная матрица
    int reverse_matrx[3][3];
    for (int i = 0; i < 3; i++){
        for(int j = 0; j < 3; j++){
            int tmp[2][2], stroka=0, stolbec=0;
            for(int ii = 0; ii < 3; ii++){
                if(ii != i){
                    for(int jj = 0; jj < 3;jj++){
                        if(jj != j){
                            tmp[stroka][stolbec] = key_matrix[ii][jj];
                            stolbec ++;
                        }
                    }
                    stroka ++;
                    stolbec = 0;
                }
            }
            reverse_matrx[j][i] = int(pow((-1),(i+j)))*(det_2(tmp));
            reverse_matrx[j][i] = (reverse_matrx[j][i]*x)%(alp_size);
            while(reverse_matrx[j][i] < 0)
                reverse_matrx[j][i] += alp_size;
        }
    }
    // процесс расшифровки
    // обратная матрица умножается на матрицу сообщения
    // после чего берется остаток от деления на длину алфавита
    QString shifr_text = "";
    for(int k = 0; k < (message.size()/3); k++){
        for(int i = 0; i < 3; i++){
            int count = 0;
            for(int j = 0; j < 3; j++)
                count += reverse_matrx[i][j] * unshif_matrix[k][j];
            shifr_text += ui->abcd->text().at(count % alp_size);
        }
    }
    shifr = shifr_text;
    }

}

void MainWindow::on_shifr_clicked()
{
    shifr = "";
    key = "";
    message = "";
    bool check_text = true;
    ui->spinBox->setValue(0);
    QString txt;
    txt = ui->key_1->text();
    //проверяеются введенный текст и ключ на соответсвтие с алфавитом
    for (int i = 0; i < ui->text_shifr->text().size(); i++)
    {
        if (find(ui->text_shifr->text().at(i)) == 37)
            check_text = false;
    }
    for (int i = 0; i <txt.size(); i++)
    {
        if (find(txt.at(i)) == 37)
            check_text = false;
    }
    // добавялем пробелы, если не хватает длины или если не кратен трём
    if (txt.size() <= 9 && check_text)
    {
        while (txt.size() != 9)
           txt += " ";
        key = txt;
        txt = ui->text_shifr->text();
        while (txt.size() % 3 != 0)
        {
            txt += " ";
            ui->spinBox->setValue(ui->spinBox->value() + 1);
        }
        message = txt;
        encrypt();
        ui->text_unshifr->setText(shifr);
    }
    else
        if (!check_text)
            QMessageBox::warning(this, "Внимание","Введите текст и ключ, используя символы из алфавита!");
    else
         QMessageBox::warning(this, "Внимание","Длина ключа должна быть не больше 9");
}

void MainWindow::encrypt()
{
    int key_matrix[3][3];
    int k = 0;
    // заполняется матрица ключа
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            key_matrix[i][j] = find(key.at(k));
            ui->key_table_1->setItem(i, j, new QTableWidgetItem(QString::number(key_matrix[i][j])));
            k++;
        }
    }
    k = 0;
    // создаётся матрица сообщения
    if (check_det(key_matrix))
    {
    int message_matrix[message.size()/3][3];
    for (int i = 0; i < message.size()/3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            message_matrix[i][j] = find(message.at(k));
            k++;
        }
    }
    // само шифрование, перемножение матрицы сообщения на матрицу ключа,
    // после чего берется остаток от деления на размер алфавита
    QString txt;
    int alp_size = ui->abcd->text().size();
    for (int i = 0; i < message.size()/3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
           int count = 0;
           for (int k = 0; k < 3; k++)
           {
               count += message_matrix[i][k]*key_matrix[j][k];
           }
           txt += ui->abcd->text().at(count % alp_size);
        }
    }
    shifr = txt;
    }
}
// ищет определенный символ в алфавите и возвращает его индекс
// вернёт 37, если символ не будет найден
int MainWindow::find(QString word)
{
    QString txt = ui->abcd->text();
    int i = 0;
    while (i < txt.size() && word != txt.at(i))
        i++;
    return i;
}


void MainWindow::on_un_shif_clicked()
{
    shifr = "";
    key = "";
    message = "";
    bool check_text = true;
    ui->spinBox->setValue(0);
    QString txt = ui->key_2->text();
    //проверяеются введенный текст и ключ на соответсвтие с алфавитом
    for (int i = 0; i < ui->text_unshifr->text().size(); i++)
    {
        if (find(ui->text_unshifr->text().at(i)) == 37)
            check_text = false;
    }
    for (int i = 0; i <txt.size(); i++)
    {
        if (find(txt.at(i)) == 37)
            check_text = false;
    }
    if (txt.size() <= 9 && check_text)
    {
        while (txt.size() != 9)
           txt += " ";
        key = txt;
        txt = ui->text_unshifr->text();
        while (txt.size() % 3 != 0)
        {
            txt += " ";
            ui->spinBox->setValue(ui->spinBox->value() + 1);
        }
        message = txt;
        decoder();
        ui->text_shifr->setText(shifr);
    }
    else
        if (!check_text)
            QMessageBox::warning(this, "Внимание","Введите текст и ключ, используя символы из алфавита!");
    else
         QMessageBox::warning(this, "Внимание","Длина ключа должна быть не больше 9");
}

