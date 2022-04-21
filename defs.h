#ifndef DEFS_H
#define DEFS_H
#include <QMap>
#include <QString>

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
};

extern QMap<QString,AddressingTypes> addrTypes;

extern QMap<QString,Operations> operations;

#endif // DEFS_H
