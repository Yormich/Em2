#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent,QString defaultCom,int Size)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow), defaultCommand(defaultCom)
{

    //row; Operation: <AddrType> Operand1, Operand2
    QString EM2Template = "###; @@@: <@@> #, #";
    QRegularExpression separators("[;:,<> ]");

    parser = new Parser(EM2Template,separators,"+-.e");
    memory = new Memory(Size,defaultCommand);

    this->defaultProgram = memory->getProgram();
    ui->setupUi(this);
    this->setWindowIcon(QIcon(QString(":images/Computer.png")));
    this->setWindowTitle("EM-2");
    this->curFile = "";
    ui->fileNameLabel->setText("untitled.txt");

    timer = new QTimer( this );

    processor = new Processor(parser,memory,ui,timer);

    ui->inputTextEdit->setReadOnly(true);
    ui->programTextEdit->setPlainText(defaultProgram);

    connect(timer,&QTimer::timeout,this,&MainWindow::timerSignalHandler);
}

MainWindow::~MainWindow()
{
    delete timer;
    delete parser;
    delete processor;
    delete memory;
    delete ui;
}

void MainWindow::timerSignalHandler()
{
    QString input = ui->inputTextEdit->toPlainText();

    processor->setInput(input);

    ui->confirmInputButton->setEnabled(false);
    ui->inputTextEdit->setReadOnly(true);
    ui->inputTextEdit->clear();

    processor->continueProgramRunning();
    ui->programTextEdit->setPlainText(memory->getProgram());
}

bool MainWindow::saveFile(const QString &fileName)
{
    QFile file(fileName);
    if(!file.open(QFile::WriteOnly | QFile::Text))
    {
        QMessageBox::warning(this,"Error",tr("Error write file: %1").arg(fileName));
    }

    QTextStream out(&file);
    QString program = ui->programTextEdit->toPlainText();
    parser->setProgramText(program);

        out << program;
        memory->setMemory(program);
        curFile = fileName;
        ui->fileNameLabel->setText(fileName);
        ui->programTextEdit->document()->setModified(false);

    return true;
}

void MainWindow::loadFile(const QString &fileName)
{
    QFile file(fileName);
    if(!file.open(QFile::ReadOnly|QFile::Text))
    {
        QMessageBox::warning(this,tr("Ошибка чтения"),tr("Не получилось прочитать файл: %1").arg(fileName));
        return;
    }

    QTextStream inp(&file);

    QString program = inp.readAll();
    parser->setProgramText(program);
    //if(parser->IsProgramValid())
    //{
        memory->setMemory(program);
        ui->programTextEdit->setPlainText(parser->getProgram());
        this->curFile = fileName;
        ui->fileNameLabel->setText(fileName);
    //}
}

bool MainWindow::MaybeSave()
{
    if(ui->programTextEdit->document()->isModified())
    {
        QMessageBox::StandardButton choice = QMessageBox::warning(this,tr("Программа изменена"),
                                               tr("Программа была изменена.\nХотите ли вы сохранить изменения?\n"),
                                               QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        switch(choice)
        {
        case QMessageBox::Save:
            return on_file_Save_triggered();
            break;
        case QMessageBox::Cancel:
            return false;
            break;
        default:
            break;
        }

        return true;
    }
    return true;
}

void MainWindow::on_file_Open_triggered()
{
    if(MaybeSave())
    {
        QString fileName = QFileDialog::getOpenFileName();
        if(!fileName.isEmpty())
        {
            loadFile(fileName);
        }
    }
}


void MainWindow::on_about_Program_triggered()
{
    QMessageBox::information(this,tr("О программе"),tr("Эта программа - сконструированная мной(Рябовом Дмитрием) \nУМ-2 - учебная машина двух адресная. Память УМ-2 состоит из 512 ячеек, имеющая адреса от 1 до 511(в интерфейсе).\nМашина поддерживат следующие типы аддресации: Прямая, Относительная, Базовая Регистровая, Непрямая Регистровая.\nБазовый формат команд: Текущая Ячейка Памяти; Код Операции: <Тип аддресации> Операнд1, Операнд2.\n"));
}

bool MainWindow::on_file_Save_triggered()
{
    if(curFile.isEmpty())
    {
        return on_file_SaveAs_triggered();
    }
    else
    {
        return saveFile(curFile);
    }
}


bool MainWindow::on_file_SaveAs_triggered()
{
    QString fileName = QFileDialog::getSaveFileName();
    if(fileName.isEmpty())
    {
        return false;
    }
    return saveFile(fileName);
}

void MainWindow::on_actionNew_triggered()
{
    if(MaybeSave())
    {
        curFile = "";
        ui->fileNameLabel->setText("untitled.txt");
        ui->programTextEdit->setPlainText(defaultProgram);
        parser->setProgramText(defaultProgram);
        memory->setMemory(defaultProgram);
    }
}

void MainWindow::on_file_Close_triggered()
{
    if(MaybeSave())
    {
        curFile = "";
        ui->fileNameLabel->setText("untitled.txt");
        ui->programTextEdit->setPlainText(defaultProgram);
        parser->setProgramText(defaultProgram);
        memory->setMemory(defaultProgram);
    }

}


void MainWindow::on_quit_Quit_triggered()
{
    if(MaybeSave())
    {
        QApplication::quit();
    }
}

void MainWindow::on_runProgram_clicked()
{
    if(MaybeSave() && parser->IsProgramValid())
    {
        ui->outputTextEdit->clear();
        processor->RunProgram();
        ui->programTextEdit->setPlainText(memory->getProgram());
    }
}

void MainWindow::on_confirmInputButton_clicked()
{
    if(!ui->inputTextEdit->isReadOnly())
    {
        QString input = ui->inputTextEdit->toPlainText();
        processor->setInput(input);
        timer->stop();
        ui->confirmInputButton->setEnabled(false);
        ui->inputTextEdit->setReadOnly(true);
        ui->inputTextEdit->clear();

        processor->continueProgramRunning();
        ui->programTextEdit->setPlainText(memory->getProgram());
    }
}

