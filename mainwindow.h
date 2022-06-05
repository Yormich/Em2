#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include  <QtGui>
#include <QtWidgets>
#include "parser.h"
#include "defs.h"
#include "memory.h"
#include "processor.h"

//возможно закинуть код связанный с файлами в класс-менеджер(меньше кода в форме, хотя в чём смысл слотов)


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr,
               QString defaultCommand = "MOV: <DI> 0, 0.0",int Size = 512);
    ~MainWindow();
    QTimer *timer;

private slots:
    void on_file_Open_triggered();

    void on_about_Program_triggered();

    bool on_file_Save_triggered();

    bool on_file_SaveAs_triggered();

    void on_file_Close_triggered();

    void on_quit_Quit_triggered();

    void on_actionNew_triggered();

    void on_runProgram_clicked();

    void on_confirmInputButton_clicked();

    void timerSignalHandler();

private:

    bool MaybeSave();
    void loadFile(const QString& fileName);
    bool saveFile(const QString& fileName);

    Ui::MainWindow *ui;
    QString curFile;
    QString defaultCommand;
    QString defaultProgram;
    Parser *parser;
    Memory *memory;
    Processor *processor;
};
#endif // MAINWINDOW_H
