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
    QString ignoreSymbols;

    bool isCommandValid(QString command) const;

    bool isContainingDigits(const QString& operand) const;

    bool isContainingLettersOrNumbers(const QString& operand) const;

    bool isUnknownOperation(const QString& operation)  const;

    bool isUnknownAddrType(const QString& addrType) const;

    bool isIgnorable(QChar symbol) const;

public:
    Parser(QString templCom,QRegularExpression commandSeparators,QString toIgnore);

    void setProgramText(const QString& prog);

    const QString getProgram();

    bool IsProgramValid();

    bool isContainingLetters(const QString& operand) const;

    const QString getCommandTemplate();

    const QString formCommand(QStringList list);

    const QStringList returnSplittedCommand(QString command);

    bool isUnknownRegister(const QString& regName) const;

    virtual ~Parser();
};

#endif // PARSER_H
