#ifndef PARSER_H
#define PARSER_H
#include <QString>
#include <QList>
#include "defs.h"
#include <QWidget>
#include <QMessageBox>
#include <QChar>
#include <QRegularExpression>

class Parser
{
private:
    QString program;
    QString templateCommand;
    QRegularExpression separators;

    const bool isCommandValid(QString command);

    const bool isContainingDigits(const QString& operand);

    const bool isContainingLetters(const QString& operand);

    const bool isContainingLettersOrNumbers(const QString& operand);

    const bool isUnknownOperation(const QString& operation);

    const bool isUnknownAddrType(const QString& addrType);
public:
    Parser(QString templCom,QRegularExpression commandSeparators);

    void setProgramText(const QString& prog);

    const QString getProgram();

    bool IsProgramValid();

    const QString getCommandTemplate();

    virtual ~Parser();
};

#endif // PARSER_H
