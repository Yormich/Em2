#ifndef PROCESSOR_H
#define PROCESSOR_H
#include "cmath"
#include "defs.h"
#include "memory.h"
#include <QString>
#include "parser.h"
#include <climits>
#include "ui_mainwindow.h"
#include <QObject>
#include <QMap>
#include <cmath>
using namespace std;

class Processor : public QObject
{
    Q_OBJECT

private:

QRegularExpression inputSplit;
QStringList src;
QStringList destination;

QMap<QString,int> intRegisters;
QMap<QString,double> floatRegisters;

QTimer *timer;

AddressingTypes addrType;

Operations commandOper;

Parser *parser;

Memory *memory;

Ui::MainWindow *mainWindow;

QString possibleInput;

bool isWereWaitingForInput;

int IRA, IRB;

double FRA, FRB;

int ESM,ECX;

int Omega;// 0 - равно, 1 - больше, -1 - меньше

bool ZFlag,ErrorFlag;

//8 регистров определить????ВЫФВ

int currentIteration;

QString operand1,operand2;

void UpdateRegisters();

void fillSrcAndDest();

void PrepareForInputFuncs();

void mov();

void pow();

void addFloat();

void subFloat();

void multFloat();

void divFloat();

void inputFloat();

void outputFloat();

void floatToInt();

void jump();

void addInt();

void subInt();

void multInt();

void divInt();

void mod();

void inputInt();

void outputInt();

void intToFloat();

void compare();

void jumpIfZero();

void jumpIfNZero();

void jumpIfGreater();

void jumpIfLower();

bool runCommand();

void restart();

public:
    Processor(Parser *pars,Memory *mem,Ui::MainWindow *window,QTimer *timer);
    void RunProgram();

    int getIteration() const;

    void setIteration(int iter);

    void continueProgramRunning();

    ~Processor();
public slots:
    void setInput(QString input);
};

#endif // PROCESSOR_H
