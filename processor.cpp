#include "processor.h"

Processor::Processor(Parser* pars,Memory* mem,Ui::MainWindow *window,QTimer *Timer) : timer(Timer), parser(pars), memory(mem),
    mainWindow(window)
{
    IRA=0,IRB = 0;
    FRA=0.0,FRB = 0.0;
    ESM=0.0,ECX = 0.0;
    ZFlag = false,ErrorFlag=false;
    currentIteration = 0;
    isWereWaitingForInput = false;
    intRegisters = {{"IRA",IRA},{"IRB",IRB},{"ESM",ESM},{"ECX",ECX}};
    floatRegisters = {{"FRA",FRA},{"FRB",FRB}};
    inputSplit = QRegularExpression("[\n, ]");
}

int Processor::getIteration() const
{
    return currentIteration;
}

void Processor::setIteration(int iter)
{
    currentIteration = iter;
}

Processor::~Processor()
{
    delete this;
}

void Processor::setInput(QString input)
{
    possibleInput = input;
}

void Processor::UpdateRegisters()
{
    QMapIterator<QString,int> intIter(intRegisters);
    QMapIterator<QString,double> floatIter(floatRegisters);
    Registers regName;
    while(intIter.hasNext())
    {
        regName = registers[intIter.key()];
        switch(regName)
        {
        case Registers::IRA:
            IRA = intIter.value();
            break;
        case Registers::IRB:
            IRB = intIter.value();
            break;
        case Registers::ECX:
            ECX = intIter.value();
            break;
        case Registers::ESM:
            ESM = intIter.value();
            break;
        default:
            ErrorFlag = true;
            break;
        }
    }
    while(floatIter.hasNext())
    {
        regName = registers[floatIter.key()];
        switch(regName)
        {
        case Registers::FRA:
            FRA = floatIter.value();
            break;
        case Registers::FRB:
            FRB = floatIter.value();
            break;
        default:
            ErrorFlag = true;
            break;
        }
    }

}
void Processor::restart()
{
    IRA=0,IRB = 0;
    FRA=0.0,FRB = 0.0;
    ESM=0.0,ECX = 0.0;
    ZFlag = false,ErrorFlag=false;
    currentIteration = 0;
    isWereWaitingForInput = false;
}

void Processor::RunProgram()
{
    while(!ErrorFlag && runCommand());

    if(!isWereWaitingForInput)
    {
        restart();
    }
}

bool Processor::runCommand()
{
    int CommandPos = 1,addrTypePos = 2,operand1Pos = 3,operand2Pos = 4;
    QString command = memory->get(currentIteration);
    QStringList list = parser->returnSplittedCommand(command);

    if(list.size()==5)
    {
        commandOper = operations[list[CommandPos]];
        addrType = addrTypes[list[addrTypePos]];
        operand1 = list[operand1Pos],operand2 = list[operand2Pos];
        switch(commandOper)
        {
        case Operations::MOV:
            mov();
            break;
        case Operations::ADR:
            addFloat();
            break;
        case Operations::SBR:
            subFloat();
            break;
        case Operations::MLR:
            multFloat();
            break;
        case Operations::DVR:
            divFloat();
            break;
        case Operations::INR:
            PrepareForInputFuncs();
            isWereWaitingForInput = true;
            return false;
            break;
        case Operations::OTR:
            outputFloat();
            break;
        case Operations::JMP:
            jump();
            break;
        case Operations::RTI:
            floatToInt();
            break;
        case Operations::ADI:
            addInt();
            break;
        case Operations::SBI:
            subInt();
            break;
        case Operations::MLI:
            multInt();
            break;
        case Operations::DVI:
            divInt();
            break;
        case Operations::MOD:
            mod();
            break;
        case Operations::INI:
            PrepareForInputFuncs();
            isWereWaitingForInput = true;
            return false;
            break;
        case Operations::OTI:
            outputInt();
            break;
        case Operations::ITR:
            intToFloat();
            break;
        case Operations::CMP:
            compare();
            break;
        case Operations::POW:
            pow();
            break;
        case Operations::JZE:
            jumpIfZero();
            break;
        case Operations::JNZ:
            jumpIfNZero();
            break;
        case Operations::JGR:
            jumpIfGreater();
            break;
        case Operations::JLW:
            jumpIfLower();
            break;
        case Operations::END:
            return false;
            break;
        default:
            QMessageBox::warning(new QMessageBox,QMessageBox::tr("Непредвиденная ошибка"),
                                 QMessageBox::tr("Неизвестний тип операции: %1").arg(list[CommandPos]));
            break;
        }
        currentIteration++;
      //  UpdateRegisters();
    }
    return true;
}

void Processor::PrepareForInputFuncs()
{
    mainWindow->inputTextEdit->setReadOnly(false);
    mainWindow->confirmInputButton->setEnabled(true);
    int numberOfValues = operand2.toInt();
    timer->start(numberOfValues * 20000);
}

void Processor::continueProgramRunning()
{
    isWereWaitingForInput = false;
    switch(commandOper)
    {
    case Operations::INR:
        inputFloat();
        break;
    case Operations::INI:
        inputInt();
        break;
    default:
        break;
    }

    currentIteration++;

    RunProgram();
}

void Processor::mov()
{
    switch(addrType)
    {
    case AddressingTypes::Direct:{
        if(operand1.toInt()!=0)
        {
            QStringList src = parser->returnSplittedCommand(memory->get(operand2.toInt() - 1));
            QStringList destination = parser->returnSplittedCommand(memory->get(operand1.toInt() - 1));
            destination[4] = src[4];
            memory->set(operand1.toInt()-1,parser->formCommand(destination));
        }
        break;
    }
    case AddressingTypes::Realtive:{
        if(operand1.toInt()!=0)
        {
            QStringList src = parser->returnSplittedCommand(memory->get(operand2.toInt() - 1));
            QStringList destination = parser->returnSplittedCommand(memory->get(operand1.toInt() + ECX - 1));
            destination[4] = src[4];
            memory->set(operand1.toInt() + ECX - 1,parser->formCommand(destination));
        }
        break;
    }
    case AddressingTypes::BaseRegister:{
        if(parser->isContainingLetters(operand1) && !parser->isContainingLetters(operand2))
        {
            QStringList src = parser->returnSplittedCommand(memory->get(operand2.toInt() - 1));
            if(intRegisters.count(operand1)==1)
            {
                intRegisters[operand1] = src[4].toInt();
            }
            else if(floatRegisters.count(operand1) == 1)
            {
                floatRegisters[operand1] = src[4].toDouble();
            }

        }
        else if(parser->isContainingLetters(operand2) && !parser->isContainingLetters(operand1))
        {
            QStringList destination = parser->returnSplittedCommand(memory->get(operand1.toInt() -1));
            if(intRegisters.count(operand2)==1)
            {
                destination[4] = QString::number(intRegisters[operand2]);
            }
            else if(floatRegisters.count(operand2) == 1)
            {
                destination[4] = QString::number(floatRegisters[operand2]);
            }
            memory->set(operand1.toInt() - 1,parser->formCommand(destination));
        }
        else if(parser->isContainingLetters(operand1) && parser->isContainingLetters(operand2))
        {
            intRegisters.count(operand1) == 1 ? intRegisters[operand1] = intRegisters[operand2] : floatRegisters[operand1] = floatRegisters[operand2];
        }

        break;
    }
    case AddressingTypes::IndirectRegister:{
        if(parser->isContainingLetters(operand1) && !parser->isContainingLetters(operand2))
        {
            QStringList src = parser->returnSplittedCommand(memory->get(operand2.toInt() - 1));
            if(intRegisters.count(operand1)==1)
            {
                QStringList destination = parser->returnSplittedCommand(memory->get(intRegisters[operand1] - 1));
                destination[4] = QString::number(src[4].toInt());
                memory->set(intRegisters[operand1],parser->formCommand(destination));
            }
            else if(floatRegisters.count(operand1) == 1)
            {
                QStringList destination = parser->returnSplittedCommand(memory->get(floatRegisters[operand1] - 1));
                destination[4] = src[4];
                memory->set(floatRegisters[operand1],parser->formCommand(destination));
            }
        }
        else if(parser->isContainingLetters(operand2) && !parser->isContainingLetters(operand1))
        {
            QStringList destination = parser->returnSplittedCommand(memory->get(operand1.toInt() -1));
            if(intRegisters.count(operand2)==1)
            {
                QStringList src = parser->returnSplittedCommand(memory->get(intRegisters[operand2] - 1));
                destination[4] = src[4];
            }
            else if(floatRegisters.count(operand2) == 1)
            {
                QStringList src = parser->returnSplittedCommand(memory->get(floatRegisters[operand2] - 1));
                destination[4] = src[4];
            }
            memory->set(operand1.toInt() - 1,parser->formCommand(destination));
        }
        else if(parser->isContainingLetters(operand1) && parser->isContainingLetters(operand2))
        {
            if(intRegisters.count(operand1) == 1 && intRegisters.count(operand2) == 1)
            {
                QStringList src = parser->returnSplittedCommand(memory->get(intRegisters[operand2] - 1));
                QStringList destination = parser->returnSplittedCommand(memory->get(intRegisters[operand1] - 1));
                destination[4] = src[4];
                memory->set(intRegisters[operand1] - 1,parser->formCommand(destination));
            }
            else if(floatRegisters.count(operand1) == 1 && floatRegisters.count(operand2) == 1)
            {
                QStringList src = parser->returnSplittedCommand(memory->get(floatRegisters[operand2] - 1));
                QStringList destination = parser->returnSplittedCommand(memory->get(floatRegisters[operand1] - 1));
                destination[4] = src[4];
                memory->set(floatRegisters[operand1] - 1,parser->formCommand(destination));
            }
        }
        break;
    }
    }
}

void Processor::addFloat()
{
    switch(addrType)
    {
    case AddressingTypes::Direct:{
        if(operand1.toInt()!=0)
        {
            QStringList src = parser->returnSplittedCommand(memory->get(operand2.toInt() - 1));
            QStringList destination = parser->returnSplittedCommand(memory->get(operand1.toInt() - 1));
            destination[4] = QString::number(destination[4].toDouble() + src[4].toDouble());
            memory->set(operand1.toInt()-1,parser->formCommand(destination));
        }
        break;
    }
    case AddressingTypes::Realtive:{
        if(operand1.toInt()!=0)
        {
            QStringList src = parser->returnSplittedCommand(memory->get(operand2.toInt() - 1));
            QStringList destination = parser->returnSplittedCommand(memory->get(operand1.toInt() + ECX - 1));
            destination[4] = QString::number(destination[4].toDouble() + src[4].toDouble());
            memory->set(operand1.toInt() + ECX - 1,parser->formCommand(destination));
        }
        break;
    }
    case AddressingTypes::BaseRegister:{
        if(parser->isContainingLetters(operand1) && !parser->isContainingLetters(operand2))
        {
            QStringList src = parser->returnSplittedCommand(memory->get(operand2.toInt() - 1));
            if(floatRegisters.count(operand1)==1)
            {
                floatRegisters[operand1] = floatRegisters[operand1] + src[4].toDouble();
            }

        }
        else if(parser->isContainingLetters(operand2) && !parser->isContainingLetters(operand1))
        {
            QStringList destination = parser->returnSplittedCommand(memory->get(operand1.toInt() -1));
            if(floatRegisters.count(operand2)==1)
            {
                destination[4] = QString::number(destination[4].toDouble() + floatRegisters[operand2]);
            }
            memory->set(operand1.toInt() - 1,parser->formCommand(destination));
        }
        else if(parser->isContainingLetters(operand1) && parser->isContainingLetters(operand2))
        {
            floatRegisters.count(operand1) == 1 ? floatRegisters[operand1] = floatRegisters[operand1] + floatRegisters[operand2] : ErrorFlag = true;
        }

        break;
    }
    case AddressingTypes::IndirectRegister:{
        if(parser->isContainingLetters(operand1) && !parser->isContainingLetters(operand2))
        {
            QStringList src = parser->returnSplittedCommand(memory->get(operand2.toInt() - 1));
            if(floatRegisters.count(operand1)==1)
            {
                QStringList destination = parser->returnSplittedCommand(memory->get(floatRegisters[operand1] - 1));
                destination[4] = QString::number(destination[4].toDouble() + src[4].toDouble());
                memory->set(floatRegisters[operand1] - 1,parser->formCommand(destination));
            }
        }
        else if(parser->isContainingLetters(operand2) && !parser->isContainingLetters(operand1))
        {
            QStringList destination = parser->returnSplittedCommand(memory->get(operand1.toInt() -1));
            if(floatRegisters.count(operand2)==1)
            {
                QStringList src = parser->returnSplittedCommand(memory->get(floatRegisters[operand2] - 1));
                destination[4] = QString::number(destination[4].toDouble() + src[4].toDouble());
                memory->set(operand1.toInt() - 1, parser->formCommand(destination));
            }
            memory->set(operand1.toInt() - 1,parser->formCommand(destination));
        }
        else if(parser->isContainingLetters(operand1) && parser->isContainingLetters(operand2))
        {
            if(floatRegisters.count(operand1) == 1 && floatRegisters.count(operand2) == 1)
            {
                QStringList src = parser->returnSplittedCommand(memory->get(floatRegisters[operand2] - 1));
                QStringList destination = parser->returnSplittedCommand(memory->get(floatRegisters[operand1] - 1));
                destination[4] = QString::number(destination[4].toDouble() + src[4].toDouble());
                memory->set(floatRegisters[operand1] - 1,parser->formCommand(destination));
            }

        }
        break;
    }
    }
}

void Processor::subFloat()
{
    switch(addrType)
    {
    case AddressingTypes::Direct:{
        if(operand1.toInt()!=0)
        {
            QStringList src = parser->returnSplittedCommand(memory->get(operand2.toInt() - 1));
            QStringList destination = parser->returnSplittedCommand(memory->get(operand1.toInt() - 1));
            destination[4] = QString::number(destination[4].toDouble() - src[4].toDouble());
            memory->set(operand1.toInt()-1,parser->formCommand(destination));
        }
        break;
    }
    case AddressingTypes::Realtive:{
        if(operand1.toInt()!=0)
        {
            QStringList src = parser->returnSplittedCommand(memory->get(operand2.toInt() - 1));
            QStringList destination = parser->returnSplittedCommand(memory->get(operand1.toInt() + ECX - 1));
            destination[4] = QString::number(destination[4].toDouble() - src[4].toDouble());
            memory->set(operand1.toInt() + ECX - 1,parser->formCommand(destination));
        }
        break;
    }
    case AddressingTypes::BaseRegister:{
        if(parser->isContainingLetters(operand1) && !parser->isContainingLetters(operand2))
        {
            QStringList src = parser->returnSplittedCommand(memory->get(operand2.toInt() - 1));
            if(floatRegisters.count(operand1)==1)
            {
                floatRegisters[operand1] = floatRegisters[operand1] - src[4].toDouble();
            }

        }
        else if(parser->isContainingLetters(operand2) && !parser->isContainingLetters(operand1))
        {
            QStringList destination = parser->returnSplittedCommand(memory->get(operand1.toInt() -1));
            if(floatRegisters.count(operand2)==1)
            {
                destination[4] = QString::number(destination[4].toDouble() - floatRegisters[operand2]);
            }
            memory->set(operand1.toInt() - 1,parser->formCommand(destination));
        }
        else if(parser->isContainingLetters(operand1) && parser->isContainingLetters(operand2))
        {
            floatRegisters.count(operand1) == 1 ? floatRegisters[operand1] = floatRegisters[operand1] - floatRegisters[operand2] : ErrorFlag = true;
        }

        break;
    }
    case AddressingTypes::IndirectRegister:{
        if(parser->isContainingLetters(operand1) && !parser->isContainingLetters(operand2))
        {
            QStringList src = parser->returnSplittedCommand(memory->get(operand2.toInt() - 1));
            if(floatRegisters.count(operand1)==1)
            {
                QStringList destination = parser->returnSplittedCommand(memory->get(floatRegisters[operand1] - 1));
                destination[4] = QString::number(destination[4].toDouble() - src[4].toDouble());
                memory->set(floatRegisters[operand1] - 1,parser->formCommand(destination));
            }
        }
        else if(parser->isContainingLetters(operand2) && !parser->isContainingLetters(operand1))
        {
            QStringList destination = parser->returnSplittedCommand(memory->get(operand1.toInt() -1));
            if(floatRegisters.count(operand2)==1)
            {
                QStringList src = parser->returnSplittedCommand(memory->get(floatRegisters[operand2] - 1));
                destination[4] = QString::number(destination[4].toDouble() - src[4].toDouble());
                memory->set(operand1.toInt() - 1, parser->formCommand(destination));
            }
            memory->set(operand1.toInt() - 1,parser->formCommand(destination));
        }
        else if(parser->isContainingLetters(operand1) && parser->isContainingLetters(operand2))
        {
            if(floatRegisters.count(operand1) == 1 && floatRegisters.count(operand2) == 1)
            {
                QStringList src = parser->returnSplittedCommand(memory->get(floatRegisters[operand2] - 1));
                QStringList destination = parser->returnSplittedCommand(memory->get(floatRegisters[operand1] - 1));
                destination[4] = QString::number(destination[4].toDouble() - src[4].toDouble());
                memory->set(floatRegisters[operand1] - 1,parser->formCommand(destination));
            }

        }
        break;
    }
    }
}

void Processor::multFloat()
{
    switch(addrType)
    {
    case AddressingTypes::Direct:{
        if(operand1.toInt()!=0)
        {
            QStringList src = parser->returnSplittedCommand(memory->get(operand2.toInt() - 1));
            QStringList destination = parser->returnSplittedCommand(memory->get(operand1.toInt() - 1));
            destination[4] = QString::number(destination[4].toDouble() * src[4].toDouble());
            memory->set(operand1.toInt()-1,parser->formCommand(destination));
        }
        break;
    }
    case AddressingTypes::Realtive:{
        if(operand1.toInt()!=0)
        {
            QStringList src = parser->returnSplittedCommand(memory->get(operand2.toInt() - 1));
            QStringList destination = parser->returnSplittedCommand(memory->get(operand1.toInt() + ECX - 1));
            destination[4] = QString::number(destination[4].toDouble() * src[4].toDouble());
            memory->set(operand1.toInt() + ECX - 1,parser->formCommand(destination));
        }
        break;
    }
    case AddressingTypes::BaseRegister:{
        if(parser->isContainingLetters(operand1) && !parser->isContainingLetters(operand2))
        {
            QStringList src = parser->returnSplittedCommand(memory->get(operand2.toInt() - 1));
            if(floatRegisters.count(operand1)==1)
            {
                floatRegisters[operand1] = floatRegisters[operand1] * src[4].toDouble();
            }

        }
        else if(parser->isContainingLetters(operand2) && !parser->isContainingLetters(operand1))
        {
            QStringList destination = parser->returnSplittedCommand(memory->get(operand1.toInt() -1));
            if(floatRegisters.count(operand2)==1)
            {
                destination[4] = QString::number(destination[4].toDouble() * floatRegisters[operand2]);
            }
            memory->set(operand1.toInt() - 1,parser->formCommand(destination));
        }
        else if(parser->isContainingLetters(operand1) && parser->isContainingLetters(operand2))
        {
            floatRegisters.count(operand1) == 1 ? floatRegisters[operand1] = floatRegisters[operand1] * floatRegisters[operand2] : ErrorFlag = true;
        }

        break;
    }
    case AddressingTypes::IndirectRegister:{
        if(parser->isContainingLetters(operand1) && !parser->isContainingLetters(operand2))
        {
            QStringList src = parser->returnSplittedCommand(memory->get(operand2.toInt() - 1));
            if(floatRegisters.count(operand1)==1)
            {
                QStringList destination = parser->returnSplittedCommand(memory->get(floatRegisters[operand1] - 1));
                destination[4] = QString::number(destination[4].toDouble() * src[4].toDouble());
                memory->set(floatRegisters[operand1] - 1,parser->formCommand(destination));
            }
        }
        else if(parser->isContainingLetters(operand2) && !parser->isContainingLetters(operand1))
        {
            QStringList destination = parser->returnSplittedCommand(memory->get(operand1.toInt() -1));
            if(floatRegisters.count(operand2)==1)
            {
                QStringList src = parser->returnSplittedCommand(memory->get(floatRegisters[operand2] - 1));
                destination[4] = QString::number(destination[4].toDouble() * src[4].toDouble());
                memory->set(operand1.toInt() - 1, parser->formCommand(destination));
            }
            memory->set(operand1.toInt() - 1,parser->formCommand(destination));
        }
        else if(parser->isContainingLetters(operand1) && parser->isContainingLetters(operand2))
        {
            if(floatRegisters.count(operand1) == 1 && floatRegisters.count(operand2) == 1)
            {
                QStringList src = parser->returnSplittedCommand(memory->get(floatRegisters[operand2] - 1));
                QStringList destination = parser->returnSplittedCommand(memory->get(floatRegisters[operand1] - 1));
                destination[4] = QString::number(destination[4].toDouble() * src[4].toDouble());
                memory->set(floatRegisters[operand1] - 1,parser->formCommand(destination));
            }

        }
        break;
    }
    }
}

void Processor::divFloat()
{
    switch(addrType)
    {
    case AddressingTypes::Direct:{
        if(operand1.toInt()!=0)
        {
            QStringList src = parser->returnSplittedCommand(memory->get(operand2.toInt() - 1));
            QStringList destination = parser->returnSplittedCommand(memory->get(operand1.toInt() - 1));
            destination[4] = QString::number(destination[4].toDouble() / src[4].toDouble());
            memory->set(operand1.toInt()-1,parser->formCommand(destination));
        }
        break;
    }
    case AddressingTypes::Realtive:{
        if(operand1.toInt()!=0)
        {
            QStringList src = parser->returnSplittedCommand(memory->get(operand2.toInt() - 1));
            QStringList destination = parser->returnSplittedCommand(memory->get(operand1.toInt() + ECX - 1));
            destination[4] = QString::number(destination[4].toDouble() / src[4].toDouble());
            memory->set(operand1.toInt() + ECX - 1,parser->formCommand(destination));
        }
        break;
    }
    case AddressingTypes::BaseRegister:{
        if(parser->isContainingLetters(operand1) && !parser->isContainingLetters(operand2))
        {
            QStringList src = parser->returnSplittedCommand(memory->get(operand2.toInt() - 1));
            if(floatRegisters.count(operand1)==1)
            {
                floatRegisters[operand1] = floatRegisters[operand1] / src[4].toDouble();
            }

        }
        else if(parser->isContainingLetters(operand2) && !parser->isContainingLetters(operand1))
        {
            QStringList destination = parser->returnSplittedCommand(memory->get(operand1.toInt() -1));
            if(floatRegisters.count(operand2)==1)
            {
                destination[4] = QString::number(destination[4].toDouble() / floatRegisters[operand2]);
            }
            memory->set(operand1.toInt() - 1,parser->formCommand(destination));
        }
        else if(parser->isContainingLetters(operand1) && parser->isContainingLetters(operand2))
        {
            floatRegisters.count(operand1) == 1 ? floatRegisters[operand1] = floatRegisters[operand1] / floatRegisters[operand2] : ErrorFlag = true;
        }

        break;
    }
    case AddressingTypes::IndirectRegister:{
        if(parser->isContainingLetters(operand1) && !parser->isContainingLetters(operand2))
        {
            QStringList src = parser->returnSplittedCommand(memory->get(operand2.toInt() - 1));
            if(floatRegisters.count(operand1)==1)
            {
                QStringList destination = parser->returnSplittedCommand(memory->get(floatRegisters[operand1] - 1));
                destination[4] = QString::number(destination[4].toDouble() / src[4].toDouble());
                memory->set(floatRegisters[operand1] - 1,parser->formCommand(destination));
            }
        }
        else if(parser->isContainingLetters(operand2) && !parser->isContainingLetters(operand1))
        {
            QStringList destination = parser->returnSplittedCommand(memory->get(operand1.toInt() -1));
            if(floatRegisters.count(operand2)==1)
            {
                QStringList src = parser->returnSplittedCommand(memory->get(floatRegisters[operand2] - 1));
                destination[4] = QString::number(destination[4].toDouble() / src[4].toDouble());
                memory->set(operand1.toInt() - 1, parser->formCommand(destination));
            }
            memory->set(operand1.toInt() - 1,parser->formCommand(destination));
        }
        else if(parser->isContainingLetters(operand1) && parser->isContainingLetters(operand2))
        {
            if(floatRegisters.count(operand1) == 1 && floatRegisters.count(operand2) == 1)
            {
                QStringList src = parser->returnSplittedCommand(memory->get(floatRegisters[operand2] - 1));
                QStringList destination = parser->returnSplittedCommand(memory->get(floatRegisters[operand1] - 1));
                destination[4] = QString::number(destination[4].toDouble() / src[4].toDouble());
                memory->set(floatRegisters[operand1] - 1,parser->formCommand(destination));
            }

        }
        break;
    }
    }
}

void Processor::inputFloat()
{
    QStringList values = possibleInput.split(inputSplit,Qt::SkipEmptyParts);
    switch(addrType)
    {
    case AddressingTypes::Direct:{
      int numberOfValues = operand2.toInt();
      int startPos = operand1.toInt();
      if(numberOfValues == values.size())
      {
          for(int i = 0; i < numberOfValues; i++)
          {
              double value = values[i].toDouble();
              QStringList row = parser->returnSplittedCommand(memory->get(startPos + i - 1));
              row[4] = QString::number(value);
              memory->set(startPos + i - 1, parser->formCommand(row));
          }
      }
      else
      {
          QMessageBox::warning(new QMessageBox,QMessageBox::tr("Непредвиденная ошибка"),
                               QMessageBox::tr("Неправильное количество полученных значений: %1 вместо %2").arg(QString::number(values.size())).arg(QString::number(numberOfValues)));
      }
    }
      break;
    case AddressingTypes::BaseRegister:{
        if(values.size()== 1 && floatRegisters.count(operand1) == 1 && operand2.toInt() == 1)
        {
            floatRegisters[operand1] = values[0].toDouble();
        }
        else
        {
            QMessageBox::warning(new QMessageBox,QMessageBox::tr("Непредвиденная ошибка"),
                                 QMessageBox::tr("Нельзя ввести более одного значения в регистр: %1").arg(operand2));
        }
        break;
    }
    default:
        break;
    }
}

void Processor::outputFloat()
{
    switch(addrType)
    {
    case AddressingTypes::Direct:{
        int numberOfValues = operand2.toInt();
        int startPos = operand1.toInt();
        QString result = mainWindow->outputTextEdit->toPlainText();
        for(int i = 0; i < numberOfValues; i++)
        {
            QStringList info = parser->returnSplittedCommand(memory->get(startPos + i -1));
            result += info[4] + " ";
        }
        mainWindow->outputTextEdit->setPlainText(result);
        break;
    }
    case AddressingTypes::BaseRegister:{
        QString value = mainWindow->outputTextEdit->toPlainText();
        if(value.length()!=0)
        {
            value += "\n";
        }
        if(operand2.toInt()==1 && floatRegisters.count(operand1) == 1)
        {
            value += QString::number(floatRegisters[operand1]);
            mainWindow->outputTextEdit->setPlainText(value);
        }
        else
        {
            QMessageBox::warning(new QMessageBox,QMessageBox::tr("Непредвиденная ошибка"),
                                 QMessageBox::tr("Неправильно кол-во значения для вывода из регистра: %1").arg(operand2));
        }
        break;
    }
    default:
        break;
    }
}

void Processor::floatToInt()
{
    double PossibleRegValue = -100000;
    switch (addrType){

    case AddressingTypes::Direct:{
        if(operand1.toInt()!=0)
        {
            QStringList src = parser->returnSplittedCommand(memory->get(operand2.toInt() - 1));
            QStringList destination = parser->returnSplittedCommand(memory->get(operand1.toInt() - 1));
            destination[4] = QString::number(src[4].toInt());
            memory->set(operand1.toInt()-1,parser->formCommand(destination));
        }
        break;
    }
    case AddressingTypes::Realtive:{
        if(operand1.toInt()!=0)
        {
            QStringList src = parser->returnSplittedCommand(memory->get(operand2.toInt() - 1));
            QStringList destination = parser->returnSplittedCommand(memory->get(operand1.toInt() + ECX - 1));
            destination[4] = QString::number(src[4].toInt());
            memory->set(operand1.toInt() + ECX - 1,parser->formCommand(destination));
        }
        break;
    }
    case AddressingTypes::BaseRegister:{
        Registers reg = registers[operand2];
        switch(reg)
        {
        case Registers::FRA:
        {
            PossibleRegValue = FRA;
            break;
        }
        case Registers::FRB:
        {
            PossibleRegValue = FRB;
            break;
        }
        default:
            QMessageBox::warning(new QMessageBox,QMessageBox::tr("Непредвиденная ошибка"),
                                 QMessageBox::tr("Неизвестный тип регистра: %1").arg(operand2));
            break;
        }
        QStringList destination = parser->returnSplittedCommand(memory->get(operand1.toInt() - 1));
        destination[4] = QString::number(static_cast<int>(PossibleRegValue));
        memory->set(operand1.toInt()-1,parser->formCommand(destination));
        break;
    }
    default:
        break;
    }
}

void Processor::jump()
{
    switch(addrType)
    {
    case AddressingTypes::Direct:{
        if(operand1.toInt()>0)
        {
            currentIteration= operand1.toInt() - 2; /* -2(-1 для правильного доступа к ячейке памяти и ещё -1
                                                             так как после выполнения Любой операции итерация увеличивается*/
        }
        break;
    }
    case AddressingTypes::Realtive:
        if(operand1.toInt()>0)
        {
            currentIteration=operand1.toInt() + ECX - 2;
        }
        break;
    default:
        break;
    }
    if(operand1.toInt() <= 0)
        QMessageBox::warning(new QMessageBox,QMessageBox::tr("Непредвиденная ошибка"),
                             QMessageBox::tr("Прыжок за границы памяти: %1").arg(operand1));

}

void Processor::addInt()
{
    switch(addrType)
    {
    case AddressingTypes::Direct:{
        if(operand1.toInt()!=0)
        {
            QStringList src = parser->returnSplittedCommand(memory->get(operand2.toInt() - 1));
            QStringList destination = parser->returnSplittedCommand(memory->get(operand1.toInt() - 1));
            destination[4] = QString::number(destination[4].toInt() + src[4].toInt());
            memory->set(operand1.toInt()-1,parser->formCommand(destination));
        }
        break;
    }
    case AddressingTypes::Realtive:{
        if(operand1.toInt()!=0)
        {
            QStringList src = parser->returnSplittedCommand(memory->get(operand2.toInt() - 1));
            QStringList destination = parser->returnSplittedCommand(memory->get(operand1.toInt() + ECX - 1));
            destination[4] = QString::number(destination[4].toInt() + src[4].toInt());
            memory->set(operand1.toInt() + ECX - 1,parser->formCommand(destination));
        }
        break;
    }
    case AddressingTypes::BaseRegister:{
        if(parser->isContainingLetters(operand1) && !parser->isContainingLetters(operand2))
        {
            QStringList src = parser->returnSplittedCommand(memory->get(operand2.toInt() - 1));
            if(intRegisters.count(operand1)==1)
            {
                intRegisters[operand1] = intRegisters[operand1] + src[4].toInt();
            }

        }
        else if(parser->isContainingLetters(operand2) && !parser->isContainingLetters(operand1))
        {
            QStringList destination = parser->returnSplittedCommand(memory->get(operand1.toInt() -1));
            if(intRegisters.count(operand2)==1)
            {
                destination[4] = QString::number(destination[4].toInt() + intRegisters[operand2]);
            }
            memory->set(operand1.toInt() - 1,parser->formCommand(destination));
        }
        else if(parser->isContainingLetters(operand1) && parser->isContainingLetters(operand2))
        {
            (intRegisters.count(operand1) && intRegisters.count(operand2)) == 1 ?
                        intRegisters[operand1] = intRegisters[operand1] + intRegisters[operand2] : ErrorFlag = true;
        }

        break;
    }
    case AddressingTypes::IndirectRegister:{
        if(parser->isContainingLetters(operand1) && !parser->isContainingLetters(operand2))
        {
            QStringList src = parser->returnSplittedCommand(memory->get(operand2.toInt() - 1));
            if(intRegisters.count(operand1)==1)
            {
                QStringList destination = parser->returnSplittedCommand(memory->get(intRegisters[operand1] - 1));
                destination[4] = QString::number(destination[4].toInt() + src[4].toInt());
                memory->set(intRegisters[operand1] - 1,parser->formCommand(destination));
            }
        }
        else if(parser->isContainingLetters(operand2) && !parser->isContainingLetters(operand1))
        {
            QStringList destination = parser->returnSplittedCommand(memory->get(operand1.toInt() -1));
            if(intRegisters.count(operand2)==1)
            {
                QStringList src = parser->returnSplittedCommand(memory->get(intRegisters[operand2] - 1));
                destination[4] = QString::number(destination[4].toInt() + src[4].toInt());
                memory->set(operand1.toInt() - 1, parser->formCommand(destination));
            }
            memory->set(operand1.toInt() - 1,parser->formCommand(destination));
        }
        else if(parser->isContainingLetters(operand1) && parser->isContainingLetters(operand2))
        {
            if(intRegisters.count(operand1) == 1 && intRegisters.count(operand2) == 1)
            {
                QStringList src = parser->returnSplittedCommand(memory->get(intRegisters[operand2] - 1));
                QStringList destination = parser->returnSplittedCommand(memory->get(intRegisters[operand1] - 1));
                destination[4] = QString::number(destination[4].toInt() + src[4].toInt());
                memory->set(intRegisters[operand1] - 1,parser->formCommand(destination));
            }

        }
        break;
    }
    }
}

void Processor::subInt()
{
    switch(addrType)
    {
    case AddressingTypes::Direct:{
        if(operand1.toInt()!=0)
        {
            QStringList src = parser->returnSplittedCommand(memory->get(operand2.toInt() - 1));
            QStringList destination = parser->returnSplittedCommand(memory->get(operand1.toInt() - 1));
            destination[4] = QString::number(destination[4].toInt() - src[4].toInt());
            memory->set(operand1.toInt()-1,parser->formCommand(destination));
        }
        break;
    }
    case AddressingTypes::Realtive:{
        if(operand1.toInt()!=0)
        {
            QStringList src = parser->returnSplittedCommand(memory->get(operand2.toInt() - 1));
            QStringList destination = parser->returnSplittedCommand(memory->get(operand1.toInt() + ECX - 1));
            destination[4] = QString::number(destination[4].toInt() - src[4].toInt());
            memory->set(operand1.toInt() + ECX - 1,parser->formCommand(destination));
        }
        break;
    }
    case AddressingTypes::BaseRegister:{
        if(parser->isContainingLetters(operand1) && !parser->isContainingLetters(operand2))
        {
            QStringList src = parser->returnSplittedCommand(memory->get(operand2.toInt() - 1));
            if(intRegisters.count(operand1)==1)
            {
                intRegisters[operand1] = intRegisters[operand1] - src[4].toInt();
            }

        }
        else if(parser->isContainingLetters(operand2) && !parser->isContainingLetters(operand1))
        {
            QStringList destination = parser->returnSplittedCommand(memory->get(operand1.toInt() -1));
            if(intRegisters.count(operand2)==1)
            {
                destination[4] = QString::number(destination[4].toInt() - intRegisters[operand2]);
            }
            memory->set(operand1.toInt() - 1,parser->formCommand(destination));
        }
        else if(parser->isContainingLetters(operand1) && parser->isContainingLetters(operand2))
        {
            (intRegisters.count(operand1) && intRegisters.count(operand2)) == 1 ?
                        intRegisters[operand1] = intRegisters[operand1] - intRegisters[operand2] : ErrorFlag = true;
        }

        break;
    }
    case AddressingTypes::IndirectRegister:{
        if(parser->isContainingLetters(operand1) && !parser->isContainingLetters(operand2))
        {
            QStringList src = parser->returnSplittedCommand(memory->get(operand2.toInt() - 1));
            if(intRegisters.count(operand1)==1)
            {
                QStringList destination = parser->returnSplittedCommand(memory->get(intRegisters[operand1] - 1));
                destination[4] = QString::number(destination[4].toInt() - src[4].toInt());
                memory->set(intRegisters[operand1] - 1,parser->formCommand(destination));
            }
        }
        else if(parser->isContainingLetters(operand2) && !parser->isContainingLetters(operand1))
        {
            QStringList destination = parser->returnSplittedCommand(memory->get(operand1.toInt() -1));
            if(intRegisters.count(operand2)==1)
            {
                QStringList src = parser->returnSplittedCommand(memory->get(intRegisters[operand2] - 1));
                destination[4] = QString::number(destination[4].toInt() - src[4].toInt());
                memory->set(operand1.toInt() - 1, parser->formCommand(destination));
            }
            memory->set(operand1.toInt() - 1,parser->formCommand(destination));
        }
        else if(parser->isContainingLetters(operand1) && parser->isContainingLetters(operand2))
        {
            if(intRegisters.count(operand1) == 1 && intRegisters.count(operand2) == 1)
            {
                QStringList src = parser->returnSplittedCommand(memory->get(intRegisters[operand2] - 1));
                QStringList destination = parser->returnSplittedCommand(memory->get(intRegisters[operand1] - 1));
                destination[4] = QString::number(destination[4].toInt() - src[4].toInt());
                memory->set(intRegisters[operand1] - 1,parser->formCommand(destination));
            }

        }
        break;
    }
    }
}

void Processor::multInt()
{
    switch(addrType)
    {
    case AddressingTypes::Direct:{
        if(operand1.toInt()!=0)
        {
            QStringList src = parser->returnSplittedCommand(memory->get(operand2.toInt() - 1));
            QStringList destination = parser->returnSplittedCommand(memory->get(operand1.toInt() - 1));
            destination[4] = QString::number(destination[4].toInt() * src[4].toInt());
            memory->set(operand1.toInt()-1,parser->formCommand(destination));
        }
        break;
    }
    case AddressingTypes::Realtive:{
        if(operand1.toInt()!=0)
        {
            QStringList src = parser->returnSplittedCommand(memory->get(operand2.toInt() - 1));
            QStringList destination = parser->returnSplittedCommand(memory->get(operand1.toInt() + ECX - 1));
            destination[4] = QString::number(destination[4].toInt() * src[4].toInt());
            memory->set(operand1.toInt() + ECX - 1,parser->formCommand(destination));
        }
        break;
    }
    case AddressingTypes::BaseRegister:{
        if(parser->isContainingLetters(operand1) && !parser->isContainingLetters(operand2))
        {
            QStringList src = parser->returnSplittedCommand(memory->get(operand2.toInt() - 1));
            if(intRegisters.count(operand1)==1)
            {
                intRegisters[operand1] = intRegisters[operand1] * src[4].toInt();
            }

        }
        else if(parser->isContainingLetters(operand2) && !parser->isContainingLetters(operand1))
        {
            QStringList destination = parser->returnSplittedCommand(memory->get(operand1.toInt() -1));
            if(intRegisters.count(operand2)==1)
            {
                destination[4] = QString::number(destination[4].toInt() * intRegisters[operand2]);
            }
            memory->set(operand1.toInt() - 1,parser->formCommand(destination));
        }
        else if(parser->isContainingLetters(operand1) && parser->isContainingLetters(operand2))
        {
            (intRegisters.count(operand1) && intRegisters.count(operand2)) == 1 ?
                        intRegisters[operand1] = intRegisters[operand1] * intRegisters[operand2] : ErrorFlag = true;
        }

        break;
    }
    case AddressingTypes::IndirectRegister:{
        if(parser->isContainingLetters(operand1) && !parser->isContainingLetters(operand2))
        {
            QStringList src = parser->returnSplittedCommand(memory->get(operand2.toInt() - 1));
            if(intRegisters.count(operand1)==1)
            {
                QStringList destination = parser->returnSplittedCommand(memory->get(intRegisters[operand1] - 1));
                destination[4] = QString::number(destination[4].toInt() * src[4].toInt());
                memory->set(intRegisters[operand1] - 1,parser->formCommand(destination));
            }
        }
        else if(parser->isContainingLetters(operand2) && !parser->isContainingLetters(operand1))
        {
            QStringList destination = parser->returnSplittedCommand(memory->get(operand1.toInt() -1));
            if(intRegisters.count(operand2)==1)
            {
                QStringList src = parser->returnSplittedCommand(memory->get(intRegisters[operand2] - 1));
                destination[4] = QString::number(destination[4].toInt() * src[4].toInt());
                memory->set(operand1.toInt() - 1, parser->formCommand(destination));
            }
            memory->set(operand1.toInt() - 1,parser->formCommand(destination));
        }
        else if(parser->isContainingLetters(operand1) && parser->isContainingLetters(operand2))
        {
            if(intRegisters.count(operand1) == 1 && intRegisters.count(operand2) == 1)
            {
                QStringList src = parser->returnSplittedCommand(memory->get(intRegisters[operand2] - 1));
                QStringList destination = parser->returnSplittedCommand(memory->get(intRegisters[operand1] - 1));
                destination[4] = QString::number(destination[4].toInt() * src[4].toInt());
                memory->set(intRegisters[operand1] - 1,parser->formCommand(destination));
            }

        }
        break;
    }
    }
}

void Processor::divInt()
{
    switch(addrType)
    {
    case AddressingTypes::Direct:{
        if(operand1.toInt()!=0)
        {
            QStringList src = parser->returnSplittedCommand(memory->get(operand2.toInt() - 1));
            QStringList destination = parser->returnSplittedCommand(memory->get(operand1.toInt() - 1));
            destination[4] = QString::number(destination[4].toInt() / src[4].toInt());
            memory->set(operand1.toInt()-1,parser->formCommand(destination));
        }
        break;
    }
    case AddressingTypes::Realtive:{
        if(operand1.toInt()!=0)
        {
            QStringList src = parser->returnSplittedCommand(memory->get(operand2.toInt() - 1));
            QStringList destination = parser->returnSplittedCommand(memory->get(operand1.toInt() + ECX - 1));
            destination[4] = QString::number(destination[4].toInt() / src[4].toInt());
            memory->set(operand1.toInt() + ECX - 1,parser->formCommand(destination));
        }
        break;
    }
    case AddressingTypes::BaseRegister:{
        if(parser->isContainingLetters(operand1) && !parser->isContainingLetters(operand2))
        {
            QStringList src = parser->returnSplittedCommand(memory->get(operand2.toInt() - 1));
            if(intRegisters.count(operand1)==1)
            {
                intRegisters[operand1] = intRegisters[operand1] / src[4].toInt();
            }

        }
        else if(parser->isContainingLetters(operand2) && !parser->isContainingLetters(operand1))
        {
            QStringList destination = parser->returnSplittedCommand(memory->get(operand1.toInt() -1));
            if(intRegisters.count(operand2)==1)
            {
                destination[4] = QString::number(destination[4].toInt() / intRegisters[operand2]);
            }
            memory->set(operand1.toInt() - 1,parser->formCommand(destination));
        }
        else if(parser->isContainingLetters(operand1) && parser->isContainingLetters(operand2))
        {
            (intRegisters.count(operand1) && intRegisters.count(operand2)) == 1 ?
                        intRegisters[operand1] = intRegisters[operand1] / intRegisters[operand2] : ErrorFlag = true;
        }

        break;
    }
    case AddressingTypes::IndirectRegister:{
        if(parser->isContainingLetters(operand1) && !parser->isContainingLetters(operand2))
        {
            QStringList src = parser->returnSplittedCommand(memory->get(operand2.toInt() - 1));
            if(intRegisters.count(operand1)==1)
            {
                QStringList destination = parser->returnSplittedCommand(memory->get(intRegisters[operand1] - 1));
                destination[4] = QString::number(destination[4].toInt() / src[4].toInt());
                memory->set(intRegisters[operand1] - 1,parser->formCommand(destination));
            }
        }
        else if(parser->isContainingLetters(operand2) && !parser->isContainingLetters(operand1))
        {
            QStringList destination = parser->returnSplittedCommand(memory->get(operand1.toInt() -1));
            if(intRegisters.count(operand2)==1)
            {
                QStringList src = parser->returnSplittedCommand(memory->get(intRegisters[operand2] - 1));
                destination[4] = QString::number(destination[4].toInt() / src[4].toInt());
                memory->set(operand1.toInt() - 1, parser->formCommand(destination));
            }
            memory->set(operand1.toInt() - 1,parser->formCommand(destination));
        }
        else if(parser->isContainingLetters(operand1) && parser->isContainingLetters(operand2))
        {
            if(intRegisters.count(operand1) == 1 && intRegisters.count(operand2) == 1)
            {
                QStringList src = parser->returnSplittedCommand(memory->get(intRegisters[operand2] - 1));
                QStringList destination = parser->returnSplittedCommand(memory->get(intRegisters[operand1] - 1));
                destination[4] = QString::number(destination[4].toInt() / src[4].toInt());
                memory->set(intRegisters[operand1] - 1,parser->formCommand(destination));
            }

        }
        break;
    }
    }
}

void Processor::mod() // works only for integers
{
    int PossibleRegValue = 0;
    switch(addrType)
    {
    case AddressingTypes::Direct:{
        if(operand1.toInt()!=0)
        {
            QStringList src = parser->returnSplittedCommand(memory->get(operand2.toInt() - 1));
            QStringList destination = parser->returnSplittedCommand(memory->get(operand1.toInt() - 1));
            destination[4] = QString::number(destination[4].toInt() % src[4].toInt());
            memory->set(operand1.toInt()-1,parser->formCommand(destination));
        }
        break;
    }
    case AddressingTypes::Realtive:{
        if(operand1.toInt()!=0)
        {
            QStringList src = parser->returnSplittedCommand(memory->get(operand2.toInt() - 1));
            QStringList destination = parser->returnSplittedCommand(memory->get(operand1.toInt() + ECX - 1));
            destination[4] = QString::number(destination[4].toInt() % src[4].toInt());
            memory->set(operand1.toInt() + ECX - 1,parser->formCommand(destination));
        }
        break;
    }
    case AddressingTypes::BaseRegister:{
        if(parser->isContainingLetters(operand1) && !parser->isContainingLetters(operand2))
        {
            PossibleRegValue = intRegisters[operand1];
            QStringList src = parser->returnSplittedCommand(memory->get(operand2.toInt() - 1));
            intRegisters[operand1] = PossibleRegValue % src[4].toInt();
        }
        else if(parser->isContainingLetters(operand2) && !parser->isContainingLetters(operand1))
        {
            PossibleRegValue = intRegisters[operand2];
            QStringList destination = parser->returnSplittedCommand(memory->get(operand1.toInt() - 1));
            destination[4] = QString::number(destination[4].toInt()%PossibleRegValue);
            memory->set(operand1.toInt() - 1,parser->formCommand(destination));
        }
        else if(parser->isContainingLetters(operand1) && parser->isContainingLetters(operand2))
        {
            intRegisters[operand1] = intRegisters[operand1] % intRegisters[operand2];
        }
        break;
    }
    case AddressingTypes::IndirectRegister:{
        if(parser->isContainingLetters(operand1) && !parser->isContainingLetters(operand2))
        {
            PossibleRegValue = intRegisters[operand1];
            QStringList destination = parser->returnSplittedCommand(memory->get(PossibleRegValue - 1));
            QStringList src = parser->returnSplittedCommand(memory->get(operand2.toInt() - 1));
            destination[4] = QString::number(destination[4].toInt() % src[4].toInt());
            memory->set(PossibleRegValue - 1,parser->formCommand(destination));
        }
        else if(parser->isContainingLetters(operand2) && !parser->isContainingLetters(operand1))
        {
            PossibleRegValue = parser->returnSplittedCommand(memory->get(intRegisters[operand2] - 1))[4].toInt();
            QStringList destination = parser->returnSplittedCommand(memory->get(operand1.toInt() - 1));
            destination[4] = QString::number(destination[4].toInt()%PossibleRegValue);
            memory->set(operand1.toInt() - 1, parser->formCommand(destination));
        }
        else if(parser->isContainingLetters(operand1) && parser->isContainingLetters(operand2))
        {
            QStringList destination = parser->returnSplittedCommand(memory->get(intRegisters[operand1] - 1));
            destination[4] = QString::number(destination[4].toInt() % parser->returnSplittedCommand(
                        memory->get(intRegisters[operand2] - 1))[4].toInt());
        }
        break;
    }
    }
}

void Processor::inputInt()
{
    QStringList values = possibleInput.split(inputSplit,Qt::SkipEmptyParts);
    switch(addrType)
    {
    case AddressingTypes::Direct:{
      int numberOfValues = operand2.toInt();
      int startPos = operand1.toInt();
      if(numberOfValues == values.size())
      {
          for(int i = 0; i < numberOfValues; i++)
          {
              int value = values[i].toInt();
              QStringList row = parser->returnSplittedCommand(memory->get(startPos + i - 1));
              row[4] = QString::number(value);
              memory->set(startPos + i - 1, parser->formCommand(row));
          }
      }
      else
      {
          QMessageBox::warning(new QMessageBox,QMessageBox::tr("Непредвиденная ошибка"),
                               QMessageBox::tr("Неправильное количество полученных значений: %1 вместо %2").arg(QString::number(values.size())).arg(QString::number(numberOfValues)));

      }
    }
      break;
    case AddressingTypes::BaseRegister:{
        if(values.size()== 1 && intRegisters.count(operand1) == 1)
        {
               intRegisters[operand1] = values[0].toInt();
        }
        else
        {
            QMessageBox::warning(new QMessageBox,QMessageBox::tr("Непредвиденная ошибка"),
                                 QMessageBox::tr("Нельзя ввести более одного значения в регистр: %1").arg(operand2));
        }
        break;
    }
    default:
        break;
    }
}

void Processor::outputInt()
{
    switch(addrType)
    {
    case AddressingTypes::Direct:{
        int numberOfValues = operand2.toInt();
        int startPos = operand1.toInt();
        QString result = mainWindow->outputTextEdit->toPlainText();
        for(int i = 0; i < numberOfValues; i++)
        {
            QStringList info = parser->returnSplittedCommand(memory->get(startPos + i -1));
            int value = info[4].toInt();
            result += QString::number(value) + " ";
        }
        mainWindow->outputTextEdit->setPlainText(result);
        break;
    }
    case AddressingTypes::BaseRegister:{
        QString value = mainWindow->outputTextEdit->toPlainText();
        if(value.length()!=0)
        {
            value += "\n";
        }
        if(operand2.toInt()==1 && intRegisters.count(operand1) == 1)
        {
            value += QString::number(intRegisters[operand1]);
            mainWindow->outputTextEdit->setPlainText(value);
        }
        else
        {
            QMessageBox::warning(new QMessageBox,QMessageBox::tr("Непредвиденная ошибка"),
                                 QMessageBox::tr("Неправильно кол-во значения для вывода из регистра: %1").arg(operand2));

        }
        break;
    }
    default:
        break;
    }
}

void Processor::intToFloat()
{
    int PossibleRegValue = -100000;
    switch (addrType){

    case AddressingTypes::Direct:{
        if(operand1.toInt()!=0)
        {
            QStringList src = parser->returnSplittedCommand(memory->get(operand2.toInt() - 1));
            QStringList destination = parser->returnSplittedCommand(memory->get(operand1.toInt() - 1));
            destination[4] = QString::number(src[4].toDouble());
            memory->set(operand1.toInt()-1,parser->formCommand(destination));
        }
        break;
    }
    case AddressingTypes::Realtive:{
        if(operand1.toInt()!=0)
        {
            QStringList src = parser->returnSplittedCommand(memory->get(operand2.toInt() - 1));
            QStringList destination = parser->returnSplittedCommand(memory->get(operand1.toInt() + ECX - 1));
            destination[4] = QString::number(src[4].toDouble());
            memory->set(operand1.toInt() + ECX - 1,parser->formCommand(destination));
        }
        break;
    }
    case AddressingTypes::BaseRegister:{
        Registers reg = registers[operand2];
        switch(reg)
        {
        case Registers::IRA:
        {
            PossibleRegValue = IRA;
            break;
        }
        case Registers::IRB:
        {
            PossibleRegValue = IRB;
            break;
        }
        case Registers::ECX:
        {
            PossibleRegValue = ECX;
            break;
        }
        case Registers::ESM:
        {
            PossibleRegValue = ESM;
            break;
        }
        default:
            QMessageBox::warning(new QMessageBox,QMessageBox::tr("Непредвиденная ошибка"),
                                 QMessageBox::tr("Неизвестный тип регистра: %1").arg(operand2));
            break;
        }
        QStringList destination = parser->returnSplittedCommand(memory->get(operand1.toInt() - 1));
        destination[4] = QString::number(static_cast<double>(PossibleRegValue));
        memory->set(operand1.toInt()-1,parser->formCommand(destination));
        break;
    }
    default:
        break;
    }
}

void Processor::compare()
{
    switch(addrType)
    {
    case AddressingTypes::Direct:{
        if(operand1.toInt()!=0)
        {
            QStringList value2 = parser->returnSplittedCommand(memory->get(operand2.toInt() - 1));
            QStringList value1 = parser->returnSplittedCommand(memory->get(operand1.toInt() - 1));
            (value1[4].toDouble()-value2[4].toDouble()) > 0 ? Omega = 1 : (value1[4].toDouble()-value2[4].toDouble()) == 0
                    ? Omega = 0 : Omega = -1;
        }
        break;
    }
    case AddressingTypes::Realtive:{
        if(operand1.toInt()!=0)
        {
            QStringList value1 = parser->returnSplittedCommand(memory->get(operand2.toInt() - 1));
            QStringList value2 = parser->returnSplittedCommand(memory->get(operand1.toInt() + ECX - 1));
            (value1[4].toDouble()-value2[4].toDouble()) > 0 ? Omega = 1 : (value1[4].toDouble()-value2[4].toDouble()) == 0
                    ? Omega = 0 : Omega = -1;
        }
        break;
    }
    case AddressingTypes::BaseRegister:{
        if(parser->isContainingLetters(operand1) && !parser->isContainingLetters(operand2))
        {
            QStringList value2 = parser->returnSplittedCommand(memory->get(operand2.toInt() - 1));
            if(intRegisters.count(operand1)==1)
            {
                (intRegisters[operand1]-value2[4].toDouble()) > 0 ? Omega = 1 : (intRegisters[operand1]-value2[4].toDouble()) == 0
                        ? Omega = 0 : Omega = -1;
            }
            else if(floatRegisters.count(operand1) == 1)
            {
                (floatRegisters[operand1]-value2[4].toDouble()) > 0 ? Omega = 1 : (floatRegisters[operand1]-value2[4].toDouble()) == 0
                        ? Omega = 0 : Omega = -1;
            }

        }
        else if(parser->isContainingLetters(operand2) && !parser->isContainingLetters(operand1))
        {
            QStringList value1 = parser->returnSplittedCommand(memory->get(operand1.toInt() -1));
            if(intRegisters.count(operand2)==1)
            {
                (value1[4].toDouble() - intRegisters[operand2]) > 0 ? Omega = 1 : (value1[4].toDouble() - intRegisters[operand2]) == 0
                        ? Omega = 0 : Omega = -1;
            }
            else
            {
                (value1[4].toDouble() - floatRegisters[operand2]) > 0 ? Omega = 1 : (value1[4].toDouble() - floatRegisters[operand2]) == 0
                        ? Omega = 0 : Omega = -1;
            }
        }
        else if(parser->isContainingLetters(operand1) && parser->isContainingLetters(operand2))
        {
            intRegisters.count(operand1) == 1 ? ((intRegisters[operand1] - intRegisters[operand2]) > 0 ? Omega = 1 :
                    (intRegisters[operand1] - intRegisters[operand2]) == 0
                    ? Omega = 0 : Omega = -1) : ((floatRegisters[operand1] - floatRegisters[operand2]) > 0 ? Omega = 1 :
                    (floatRegisters[operand1] - floatRegisters[operand2]) == 0
                    ? Omega = 0 : Omega = -1);
        }

        break;
    }
    case AddressingTypes::IndirectRegister:{
        if(parser->isContainingLetters(operand1) && !parser->isContainingLetters(operand2))
        {
            QStringList value2 = parser->returnSplittedCommand(memory->get(operand2.toInt() - 1));
            QStringList value1;
            if(intRegisters.count(operand1)==1)
            {
                value1 = parser->returnSplittedCommand(memory->get(intRegisters[operand1] - 1));
            }
            else if(floatRegisters.count(operand1) == 1)
            {
                value1 = parser->returnSplittedCommand(memory->get(floatRegisters[operand1] - 1));
            }
            (value1[4].toDouble()-value2[4].toDouble()) > 0 ? Omega = 1 : (value1[4].toDouble()-value2[4].toDouble()) == 0
                    ? Omega = 0 : Omega = -1;
        }
        else if(parser->isContainingLetters(operand2) && !parser->isContainingLetters(operand1))
        {
            QStringList value1 = parser->returnSplittedCommand(memory->get(operand1.toInt() -1));
            QStringList value2;
            if(intRegisters.count(operand1)==1)
            {
                value2 = parser->returnSplittedCommand(memory->get(intRegisters[operand2] - 1));
            }
            else if(floatRegisters.count(operand1) == 1)
            {
                value2 = parser->returnSplittedCommand(memory->get(floatRegisters[operand2] - 1));
            }
            (value1[4].toDouble()-value2[4].toDouble()) > 0 ? Omega = 1 : (value1[4].toDouble()-value2[4].toDouble()) == 0
                    ? Omega = 0 : Omega = -1;
        }
        else if(parser->isContainingLetters(operand1) && parser->isContainingLetters(operand2))
        {
            QStringList value1,value2;
            if(intRegisters.count(operand1) == 1 && intRegisters.count(operand2) == 1)
            {
                value2 = parser->returnSplittedCommand(memory->get(intRegisters[operand2] - 1));
                value1 = parser->returnSplittedCommand(memory->get(intRegisters[operand1] - 1));
            }
            else if(floatRegisters.count(operand1) == 1 && floatRegisters.count(operand2) == 1)
            {
                value2 = parser->returnSplittedCommand(memory->get(floatRegisters[operand2] - 1));
                value1 = parser->returnSplittedCommand(memory->get(floatRegisters[operand1] - 1));
            }
            (value1[4].toDouble()-value2[4].toDouble()) > 0 ? Omega = 1 : (value1[4].toDouble()-value2[4].toDouble()) == 0
                    ? Omega = 0 : Omega = -1;
        }
        break;
    }
    }
}

void Processor::pow()
{
    switch(addrType)
    {
    case AddressingTypes::Direct:{
        if(operand1.toInt()!=0)
        {
            QStringList src = parser->returnSplittedCommand(memory->get(operand2.toInt() - 1));
            QStringList destination = parser->returnSplittedCommand(memory->get(operand1.toInt() - 1));
            destination[4] = QString::number(exp(src[4].toDouble() * log(destination[4].toDouble())));
            memory->set(operand1.toInt()-1,parser->formCommand(destination));
        }
        break;
    }
    case AddressingTypes::Realtive:{
        if(operand1.toInt()!=0)
        {
            QStringList src = parser->returnSplittedCommand(memory->get(operand2.toInt() - 1));
            QStringList destination = parser->returnSplittedCommand(memory->get(operand1.toInt() + ECX - 1));
            destination[4] = QString::number(exp(src[4].toDouble() * log(destination[4].toDouble())));
            memory->set(operand1.toInt() + ECX - 1,parser->formCommand(destination));
        }
        break;
    }
    case AddressingTypes::BaseRegister:{
        if(parser->isContainingLetters(operand1) && !parser->isContainingLetters(operand2))
        {
            QStringList src = parser->returnSplittedCommand(memory->get(operand2.toInt() - 1));
            if(intRegisters.count(operand1)==1)
            {
                intRegisters[operand1] = src[4].toInt();
            }
            else
            {
                floatRegisters[operand1] = src[4].toDouble();
            }

        }
        else if(parser->isContainingLetters(operand2) && !parser->isContainingLetters(operand1))
        {
            QStringList destination = parser->returnSplittedCommand(memory->get(operand1.toInt() -1));
            if(intRegisters.count(operand1)==1)
            {
                destination[4] = QString::number(intRegisters[operand2]);
            }
            else
            {
                destination[4] = QString::number(floatRegisters[operand2]);
            }
            memory->set(operand1.toInt() - 1,parser->formCommand(destination));
        }
        else if(parser->isContainingLetters(operand1) && parser->isContainingLetters(operand2))
        {
            intRegisters.count(operand1) == 1 ? intRegisters[operand1] = intRegisters[operand2] : floatRegisters[operand1] = floatRegisters[operand2];
        }

        break;
    }
    case AddressingTypes::IndirectRegister:{
        if(parser->isContainingLetters(operand1) && !parser->isContainingLetters(operand2))
        {
            QStringList src = parser->returnSplittedCommand(memory->get(operand2.toInt() - 1));
            if(intRegisters.count(operand1)==1)
            {
                QStringList destination = parser->returnSplittedCommand(memory->get(intRegisters[operand1] - 1));
                destination[4] = QString::number(src[4].toInt());
                memory->set(intRegisters[operand1],parser->formCommand(destination));
            }
            else if(floatRegisters.count(operand1) == 1)
            {
                QStringList destination = parser->returnSplittedCommand(memory->get(floatRegisters[operand1] - 1));
                destination[4] = src[4];
                memory->set(floatRegisters[operand1],parser->formCommand(destination));
            }
        }
        else if(parser->isContainingLetters(operand2) && !parser->isContainingLetters(operand1))
        {
            QStringList destination = parser->returnSplittedCommand(memory->get(operand1.toInt() -1));
            if(intRegisters.count(operand1)==1)
            {
                QStringList src = parser->returnSplittedCommand(memory->get(intRegisters[operand2] - 1));
                destination[4] = src[4];
            }
            else if(floatRegisters.count(operand1) == 1)
            {
                QStringList src = parser->returnSplittedCommand(memory->get(floatRegisters[operand2] - 1));
                destination[4] = src[4];
            }
            memory->set(operand1.toInt() - 1,parser->formCommand(destination));
        }
        else if(parser->isContainingLetters(operand1) && parser->isContainingLetters(operand2))
        {
            if(intRegisters.count(operand1) == 1 && intRegisters.count(operand2) == 1)
            {
                QStringList src = parser->returnSplittedCommand(memory->get(intRegisters[operand2] - 1));
                QStringList destination = parser->returnSplittedCommand(memory->get(intRegisters[operand1] - 1));
                destination[4] = src[4];
                memory->set(intRegisters[operand1] - 1,parser->formCommand(destination));
            }
                //ADD FLOAT REGS SITUATION!!!
        }
        break;
    }
    }
}

void Processor::jumpIfZero()
{
    if(Omega==0)
    {
        jump();
    }
}

void Processor::jumpIfNZero()
{
    if(Omega!=0)
    {
        jump();
    }
}

void Processor::jumpIfGreater()
{
    if(Omega==1)
    {
        jump();
    }
}

void Processor::jumpIfLower()
{
    if(Omega==-1)
    {
        jump();
    }
}
