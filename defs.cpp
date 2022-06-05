#include "defs.h"

QMap<QString,AddressingTypes> addrTypes = {{"BR",AddressingTypes::BaseRegister},
                                           {"DI",AddressingTypes::Direct},
                                           {"IR",AddressingTypes::IndirectRegister},
                                           {"RE",AddressingTypes::Realtive}};

QMap<QString,Operations> operations = {{"MOV",Operations::MOV},{"ADR",Operations::ADR},{"DVR",Operations::DVR},
                                       {"SBR",Operations::SBR},{"MLR",Operations::MLR},{"INR",Operations::INR},
                                       {"OTR",Operations::OTR},{"JMP",Operations::JMP},{"RTI",Operations::RTI},
                                       {"ADI",Operations::ADI},{"SBI",Operations::SBI},{"MLI",Operations::MLI},
                                       {"DVI",Operations::DVI},{"MOD",Operations::MOD},{"INI",Operations::INI},
                                       {"OTI",Operations::OTI},{"ITR",Operations::ITR} ,{"CMP",Operations::CMP},
                                       {"JZE",Operations::JZE},{"POW",Operations::POW},
                                       {"JNZ",Operations::JNZ},{"JGR",Operations::JGR},{"JLW",Operations::JLW},
                                       {"END",Operations::END}};

QMap<QString,Registers> registers = {{"IRA",Registers::IRA},{"IRB",Registers::IRB},{"FRA",Registers::FRA},
                                     {"FRB",Registers::FRB},{"ESM",Registers::ESM},{"ECX",Registers::ECX}};
