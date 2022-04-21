#include "parser.h"

Parser::Parser(QString templateProg,QRegularExpression commandSeparators) : templateCommand(templateProg),
    separators(commandSeparators)
{
}

Parser::~Parser()
{
    delete this;
}


bool Parser::IsProgramValid()
{
    if(!program.isEmpty())
    {
        QStringList commands = program.split("\n");
         for(auto el : commands)
         {
             if(!isCommandValid(el))
             {
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

const bool Parser::isContainingLetters(const QString &operand)
{
    for(const auto& symbol : operand)
    {
        if(!symbol.isLetter())
        {
            return false;
        }
    }
    return true;
}

const bool Parser::isContainingDigits(const QString& operand)
{
    for(const auto& symbol : operand)
    {
        if(!symbol.isDigit())
        {
            return false;
        }
    }
    return true;
}

const bool Parser::isContainingLettersOrNumbers(const QString &operand)
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

const bool Parser::isUnknownAddrType(const QString &addrType)
{
    if(addrTypes.count(addrType)==1)
    {
        return false;
    }
    return true;
}

const bool Parser::isUnknownOperation(const QString &operation)
{
    if(operations.count(operation)==1)
    {
        return false;
    }
    return true;
}

const bool Parser::isCommandValid(QString command)
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
                    QMessageBox::warning(new QMessageBox(),QMessageBox::tr("Wrong oper type"),
                                         QMessageBox::tr("Wrong operation: %1\n Line: %2").arg(els[i]).arg(els[0]));
                    return false;
                }
            }
            else if(i==addrTypePos)
            {
                if(isUnknownAddrType(els[i]) || !isContainingLetters(els[i]))
                {
                    QMessageBox::warning(new QMessageBox(),QMessageBox::tr("Wrong addR type"),
                                         QMessageBox::tr("Wrong addr type: %1 \n Line: %2").arg(els[i]).arg(els[0]));
                    return false;
                }

            }
            else
            {
                if(i!=0)//строка
                {
                      if(els[i].size()==templElements[i].size())
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
                              if(!isContainingDigits(els[i]) || !isContainingLetters(els[i]))
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
                    if(!isContainingDigits(els[i]) || els[i].toInt()>511)//размер памяти в параметры
                    {
                        return false;
                    }
                }
            }
        }
    }
    else
    {
        QMessageBox::warning(new QMessageBox(),QMessageBox::tr("Error in reading a command"),
                             QMessageBox::tr("Wrong size of command: %1").arg(els.size()));
        return false;
    }

    return true;
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

