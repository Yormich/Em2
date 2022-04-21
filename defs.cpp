#include "defs.h"

QMap<QString,AddressingTypes> addrTypes = {{"BR",AddressingTypes::BaseRegister},
                                           {"DI",AddressingTypes::Direct},
                                           {"IR",AddressingTypes::IndirectRegister},
                                           {"RE",AddressingTypes::Realtive}};

QMap<QString,Operations> operations = {{"MOV",Operations::MOV},{"ADR",Operations::ADR},{"DVR",Operations::DVR},
                                       {"SBR",Operations::SBR},{"MLR",Operations::MLR},{},
                                       {},{}};
