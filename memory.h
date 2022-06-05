#ifndef MEMORY_H
#define MEMORY_H
#include <QString>
#include <QVector>

struct Cell
{
    QString row;
    bool isModified;
};

class Memory
{
private:
    int Size;
    QString defaultCommand;
    QVector<Cell> memory;
public:
    Memory(int size = 512, QString defaultCommand = "MOV: <DI> 0, 0.0");

    void setMemory(const QString& program);

    const QString get(int index);

    const QString getProgram();

    void set(int index, const QString& value);

    virtual ~Memory();
};

#endif // MEMORY_H
