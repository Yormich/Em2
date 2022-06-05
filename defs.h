#ifndef DEFS_H
#define DEFS_H
#include <QMap>
#include <QString>
#include <QTimer>

enum class AddressingTypes
{
    Direct,
    IndirectRegister,
    Realtive,
    BaseRegister
};

enum class Operations
{
    MOV,
    ADR,
    SBR,
    MLR,
    DVR,
    INR,
    OTR,
    JMP,
    RTI,
    ADI,
    SBI,
    MLI,
    DVI,
    MOD,
    INI,
    OTI,
    ITR,
    CMP,
    POW,
    JZE,
    JNZ,
    JGR,
    JLW,
    END
};

enum class Registers
{
    IRA,
    IRB,
    FRA,
    FRB,
    ESM,
    ECX
};

extern QMap<QString,AddressingTypes> addrTypes;

extern QMap<QString,Operations> operations;

extern QMap<QString,Registers> registers;

#endif // DEFS_H
