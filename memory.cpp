#include "memory.h"

Memory::Memory(int size,QString defCommand) : Size(size),
    defaultCommand(defCommand)
{
    for(int i = 1; i < Size; i++)
    {

        QString row = QString::number(i) + "; ";
        row = row.rightJustified(5,'0');
        QString val =row + defaultCommand;
        Cell cell;
        cell.row = val;
        cell.isModified = false;
        memory.push_back(cell);
    }
}

void Memory::setMemory(const QString &program)
{
    QStringList prog = program.split('\n');
    if(prog.size()==memory.size())//pioioyoytoyiotiuotuyo
    {
        for(int i = 0; i < memory.size(); i++)
        {
            Cell cell;
            cell.row = prog[i];
            prog[i] == defaultCommand ? cell.isModified = false :
                     cell.isModified = true;
            memory[i] = cell;
        }
    }
}

const QString Memory::getProgram()
{
    QString prog = "";
    for(int i = 0; i < memory.size(); i++)
    {
        prog+=memory[i].row;
        if(i!=memory.size()-1)
        {
            prog+="\n";
        }
    }

    return prog;
}

const QString Memory::get(int index)
{
    if(index>=0)
    {
         return memory.at(index).row;
    }
    else
    {
        return "";
    }
}

void Memory::set(int index, const QString &value)
{
    //index==(memory.size() -1) ? memory[index].row = value : memory[index].row = value + "\n";
    memory[index].row = value;
    value == defaultCommand ? memory[index].isModified = false :
            memory[index].isModified = true;
}

Memory::~Memory()
{
}
