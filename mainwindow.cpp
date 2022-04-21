#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent,QString defaultCom)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow), defaultCommand(defaultCom)
{
    //row; Operation: <AddrType> Operand1, Operand2
    QString EM2Template = "###; @@@: <@@> ###, ###";
    QRegularExpression separators("[;:,<> ]");
    parser = new Parser(EM2Template,separators);

    ui->setupUi(this);
    this->setWindowIcon(QIcon(QString(":images/Computer.png")));
    this->setWindowTitle("EM-2");
    this->curFile = "";
    ui->fileNameLabel->setText("untitled.txt");

    for(int i = 1; i < 512; i++)
    {
        QString row = QString::number(i) + "; ";
        row = row.rightJustified(5,'0');
        defaultProgram+=row + defaultCommand;
        if(i!=511)
        {
            defaultProgram+="\n";
        }
    }

    ui->programTextEdit->setPlainText(defaultProgram);
}

MainWindow::~MainWindow()
{
    delete ui;
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

    if(parser->IsProgramValid())
    {
        out << program;
        curFile = fileName;
        ui->fileNameLabel->setText(fileName);
        ui->programTextEdit->document()->setModified(false);
    }
    return true;
}

void MainWindow::loadFile(const QString &fileName)
{
    QFile file(fileName);
    if(!file.open(QFile::ReadOnly|QFile::Text))
    {
        QMessageBox::warning(this,tr("Error"),tr("Cannot read file: %1").arg(fileName));
        return;
    }

    QTextStream inp(&file);

    QString program = inp.readAll();
    parser->setProgramText(program);
    if(parser->IsProgramValid())
    {
        ui->programTextEdit->setPlainText(parser->getProgram());
        this->curFile = fileName;
        ui->fileNameLabel->setText(fileName);
    }
}

bool MainWindow::MaybeSave()
{
    if(ui->programTextEdit->document()->isModified())
    {
        QMessageBox::StandardButton choice = QMessageBox::warning(this,tr("Document Was Modified"),
                                               tr("Document has been modified.\nDo you want to save the changes?\n"),
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

}


void MainWindow::on_about_Commands_triggered()
{

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
    }

}


void MainWindow::on_quit_Quit_triggered()
{
    QApplication::quit();
}
