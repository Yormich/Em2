#include "parser.h"

Parser::Parser(QString templateProg,QRegularExpression commandSeparators,QString toIgnore) : templateCommand(templateProg),
    separators(commandSeparators),ignoreSymbols(toIgnore)
{
}

Parser::~Parser()
{
    delete this;
}

bool Parser::isIgnorable(const QChar symbol) const
{
    for(const auto symb : ignoreSymbols)
    {
        if(symb == symbol)
        {
            return true;
        }
    }
    return false;
}
bool Parser::IsProgramValid()
{
    if(!program.isEmpty())
    {
        QStringList commands = program.split("\n");
         for(const auto& el : commands)
         {
             if(!isCommandValid(el))
             {
                 QMessageBox::warning(new QMessageBox(),QMessageBox::tr("Неправильная команда"),
                                      QMessageBox::tr("Неправильная команда, %1").arg(el));
                 return false;
             }
         }
    }
    else
    {
        QMessageBox::warning(new QMessageBox(),QMessageBox::tr("Error in reading a program"),
                             QMessageBox::tr("Program is empty"));
        return false;
    }
    return true;
}

bool Parser::isContainingLetters(const QString &operand) const
{
    for(const auto& symbol : operand)
    {
        if(!symbol.isLetter())
        {
            if(!isIgnorable(symbol))
            {
                return false;
            }
        }
    }
    return true;
}

bool Parser::isContainingDigits(const QString& operand) const
{
    for(const auto& symbol : operand)
    {
        if(!symbol.isDigit())
        {
            if(!isIgnorable(symbol))
            {
                return false;
            }
        }
    }
    return true;
}

bool Parser::isContainingLettersOrNumbers(const QString &operand) const
{
    for(const auto& symbol : operand)
    {
        if(!(symbol.isDigit() || symbol.isLetter()))
        {
            return false;
        }
    }
    return true;
}

bool Parser::isUnknownAddrType(const QString &addrType) const
{
    if(addrTypes.count(addrType)==1)
    {
        return false;
    }
    return true;
}

bool Parser::isUnknownOperation(const QString &operation) const
{
    if(operations.count(operation)==1)
    {
        return false;
    }
    return true;
}

bool Parser::isUnknownRegister(const QString &regName) const
{
    if(registers.count(regName)==1)
    {
        return false;
    }
    return true;
}

bool Parser::isCommandValid(QString command) const
{
    QStringList els = command.split(separators,Qt::SkipEmptyParts);
    QStringList templElements = templateCommand.split(separators,Qt::SkipEmptyParts);
    if(els.size()==templElements.size())
    {
        const int operPos = 1;
        const int addrTypePos = 2;

        for(int i = 0; i < els.size(); i++)
        {
            if(i==operPos)
            {
                if(isUnknownOperation(els[i]) || !isContainingLetters(els[i]))
                {
                    QMessageBox::warning(new QMessageBox(),QMessageBox::tr("Неправильный тип операции"),
                                         QMessageBox::tr("Неизвестная операция: %1\n Строка: %2").arg(els[i]).arg(els[0]));
                    return false;
                }
            }
            else if(i==addrTypePos)
            {
                if(isUnknownAddrType(els[i]) || !isContainingLetters(els[i]))
                {
                    QMessageBox::warning(new QMessageBox(),QMessageBox::tr("Неправильный тип аддресации"),
                                         QMessageBox::tr("Неизвестный тип операции: %1 \n Строка: %2").arg(els[i]).arg(els[0]));
                    return false;
                }

            }
            else
            {
                if(i!=0)//строка
                {
                      if(els[i].size()>0)
                      {
                          switch(addrTypes[els[addrTypePos]])
                          {
                          case AddressingTypes::Direct:

                          case AddressingTypes::Realtive:
                              if(!isContainingDigits(els[i]))
                              {
                                  return false;
                              }
                              break;
                          case AddressingTypes::BaseRegister:

                          case AddressingTypes::IndirectRegister:
                              if(!isContainingDigits(els[i]) && !isContainingLetters(els[i]))
                              {
                                  return false;
                              }
                              break;
                          }
                      }
                      else
                      {
                          return false;
                      }
                }
                else
                {
                    if(!isContainingDigits(els[i]))//размер памяти в параметры
                    {
                        return false;
                    }
                }
            }
        }
    }
    else
    {
        QMessageBox::warning(new QMessageBox(),QMessageBox::tr("Ошибка в чтении команды"),
                             QMessageBox::tr("Неправильный размер команды после разбиения: %1").arg(els.size()));
        return false;
    }
    return true;
}

 const QString Parser::formCommand(QStringList list)
 {
     QString command;
     for(int i = 0; i < list.size(); i++)
     {
         i == 0 ? command+=list[i]+"; " : i == 1 ? command+=list[i]+": " :
                 i==2 ? command+="<"+list[i]+"> " :
                 i==3 ? command+=list[i]+", " : command+=list[i]+" ";
     }
     return command;
 }

const QStringList Parser::returnSplittedCommand(QString command)
 {
      QStringList list = command.split(separators,Qt::SkipEmptyParts);
      return list;
 }

void Parser::setProgramText(const QString &prog)
{
    this->program = prog;
}

const QString Parser::getProgram()
{
    return program;
}

const QString Parser::getCommandTemplate()
{
    return templateCommand;
}

