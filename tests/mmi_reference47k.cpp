#include "../third_party/pcsx2-mmi47k/MMI.cpp"
rr::MmiState cpuRegs{};
bool referenceMmi47k(rr::MmiState& state, uint32_t instruction) noexcept {
    if((instruction >> 26)!=0x1cu) return false;
    cpuRegs=state; cpuRegs.code=instruction;
    auto run=[&]() {
    switch (instruction & 63u) {
    case 0: R5900::Interpreter::OpcodeImpl::MADD(); return true;
    case 1: R5900::Interpreter::OpcodeImpl::MADDU(); return true;
    case 4: R5900::Interpreter::OpcodeImpl::MMI::PLZCW(); return true;
    case 8: switch ((instruction >> 6) & 31u) {
        case 0: R5900::Interpreter::OpcodeImpl::MMI::PADDW(); return true;
        case 1: R5900::Interpreter::OpcodeImpl::MMI::PSUBW(); return true;
        case 2: R5900::Interpreter::OpcodeImpl::MMI::PCGTW(); return true;
        case 3: R5900::Interpreter::OpcodeImpl::MMI::PMAXW(); return true;
        case 4: R5900::Interpreter::OpcodeImpl::MMI::PADDH(); return true;
        case 5: R5900::Interpreter::OpcodeImpl::MMI::PSUBH(); return true;
        case 6: R5900::Interpreter::OpcodeImpl::MMI::PCGTH(); return true;
        case 7: R5900::Interpreter::OpcodeImpl::MMI::PMAXH(); return true;
        case 8: R5900::Interpreter::OpcodeImpl::MMI::PADDB(); return true;
        case 9: R5900::Interpreter::OpcodeImpl::MMI::PSUBB(); return true;
        case 10: R5900::Interpreter::OpcodeImpl::MMI::PCGTB(); return true;
        case 16: R5900::Interpreter::OpcodeImpl::MMI::PADDSW(); return true;
        case 17: R5900::Interpreter::OpcodeImpl::MMI::PSUBSW(); return true;
        case 18: R5900::Interpreter::OpcodeImpl::MMI::PEXTLW(); return true;
        case 19: R5900::Interpreter::OpcodeImpl::MMI::PPACW(); return true;
        case 20: R5900::Interpreter::OpcodeImpl::MMI::PADDSH(); return true;
        case 21: R5900::Interpreter::OpcodeImpl::MMI::PSUBSH(); return true;
        case 22: R5900::Interpreter::OpcodeImpl::MMI::PEXTLH(); return true;
        case 23: R5900::Interpreter::OpcodeImpl::MMI::PPACH(); return true;
        case 24: R5900::Interpreter::OpcodeImpl::MMI::PADDSB(); return true;
        case 25: R5900::Interpreter::OpcodeImpl::MMI::PSUBSB(); return true;
        case 26: R5900::Interpreter::OpcodeImpl::MMI::PEXTLB(); return true;
        case 27: R5900::Interpreter::OpcodeImpl::MMI::PPACB(); return true;
        case 30: R5900::Interpreter::OpcodeImpl::MMI::PEXT5(); return true;
        case 31: R5900::Interpreter::OpcodeImpl::MMI::PPAC5(); return true;
        default: return false; }
    case 9: switch ((instruction >> 6) & 31u) {
        case 0: R5900::Interpreter::OpcodeImpl::MMI::PMADDW(); return true;
        case 2: R5900::Interpreter::OpcodeImpl::MMI::PSLLVW(); return true;
        case 3: R5900::Interpreter::OpcodeImpl::MMI::PSRLVW(); return true;
        case 4: R5900::Interpreter::OpcodeImpl::MMI::PMSUBW(); return true;
        case 8: R5900::Interpreter::OpcodeImpl::MMI::PMFHI(); return true;
        case 9: R5900::Interpreter::OpcodeImpl::MMI::PMFLO(); return true;
        case 10: R5900::Interpreter::OpcodeImpl::MMI::PINTH(); return true;
        case 12: R5900::Interpreter::OpcodeImpl::MMI::PMULTW(); return true;
        case 13: R5900::Interpreter::OpcodeImpl::MMI::PDIVW(); return true;
        case 14: R5900::Interpreter::OpcodeImpl::MMI::PCPYLD(); return true;
        case 16: R5900::Interpreter::OpcodeImpl::MMI::PMADDH(); return true;
        case 17: R5900::Interpreter::OpcodeImpl::MMI::PHMADH(); return true;
        case 18: R5900::Interpreter::OpcodeImpl::MMI::PAND(); return true;
        case 19: R5900::Interpreter::OpcodeImpl::MMI::PXOR(); return true;
        case 20: R5900::Interpreter::OpcodeImpl::MMI::PMSUBH(); return true;
        case 21: R5900::Interpreter::OpcodeImpl::MMI::PHMSBH(); return true;
        case 26: R5900::Interpreter::OpcodeImpl::MMI::PEXEH(); return true;
        case 27: R5900::Interpreter::OpcodeImpl::MMI::PREVH(); return true;
        case 28: R5900::Interpreter::OpcodeImpl::MMI::PMULTH(); return true;
        case 29: R5900::Interpreter::OpcodeImpl::MMI::PDIVBW(); return true;
        case 30: R5900::Interpreter::OpcodeImpl::MMI::PEXEW(); return true;
        case 31: R5900::Interpreter::OpcodeImpl::MMI::PROT3W(); return true;
        default: return false; }
    case 16: R5900::Interpreter::OpcodeImpl::MFHI1(); return true;
    case 17: R5900::Interpreter::OpcodeImpl::MTHI1(); return true;
    case 18: R5900::Interpreter::OpcodeImpl::MFLO1(); return true;
    case 19: R5900::Interpreter::OpcodeImpl::MTLO1(); return true;
    case 24: R5900::Interpreter::OpcodeImpl::MULT1(); return true;
    case 25: R5900::Interpreter::OpcodeImpl::MULTU1(); return true;
    case 26: R5900::Interpreter::OpcodeImpl::DIV1(); return true;
    case 27: R5900::Interpreter::OpcodeImpl::DIVU1(); return true;
    case 32: R5900::Interpreter::OpcodeImpl::MADD1(); return true;
    case 33: R5900::Interpreter::OpcodeImpl::MADDU1(); return true;
    case 40: switch ((instruction >> 6) & 31u) {
        case 1: R5900::Interpreter::OpcodeImpl::MMI::PABSW(); return true;
        case 2: R5900::Interpreter::OpcodeImpl::MMI::PCEQW(); return true;
        case 3: R5900::Interpreter::OpcodeImpl::MMI::PMINW(); return true;
        case 4: R5900::Interpreter::OpcodeImpl::MMI::PADSBH(); return true;
        case 5: R5900::Interpreter::OpcodeImpl::MMI::PABSH(); return true;
        case 6: R5900::Interpreter::OpcodeImpl::MMI::PCEQH(); return true;
        case 7: R5900::Interpreter::OpcodeImpl::MMI::PMINH(); return true;
        case 10: R5900::Interpreter::OpcodeImpl::MMI::PCEQB(); return true;
        case 16: R5900::Interpreter::OpcodeImpl::MMI::PADDUW(); return true;
        case 17: R5900::Interpreter::OpcodeImpl::MMI::PSUBUW(); return true;
        case 18: R5900::Interpreter::OpcodeImpl::MMI::PEXTUW(); return true;
        case 20: R5900::Interpreter::OpcodeImpl::MMI::PADDUH(); return true;
        case 21: R5900::Interpreter::OpcodeImpl::MMI::PSUBUH(); return true;
        case 22: R5900::Interpreter::OpcodeImpl::MMI::PEXTUH(); return true;
        case 24: R5900::Interpreter::OpcodeImpl::MMI::PADDUB(); return true;
        case 25: R5900::Interpreter::OpcodeImpl::MMI::PSUBUB(); return true;
        case 26: R5900::Interpreter::OpcodeImpl::MMI::PEXTUB(); return true;
        case 27: R5900::Interpreter::OpcodeImpl::MMI::QFSRV(); return true;
        default: return false; }
    case 41: switch ((instruction >> 6) & 31u) {
        case 0: R5900::Interpreter::OpcodeImpl::MMI::PMADDUW(); return true;
        case 3: R5900::Interpreter::OpcodeImpl::MMI::PSRAVW(); return true;
        case 8: R5900::Interpreter::OpcodeImpl::MMI::PMTHI(); return true;
        case 9: R5900::Interpreter::OpcodeImpl::MMI::PMTLO(); return true;
        case 10: R5900::Interpreter::OpcodeImpl::MMI::PINTEH(); return true;
        case 12: R5900::Interpreter::OpcodeImpl::MMI::PMULTUW(); return true;
        case 13: R5900::Interpreter::OpcodeImpl::MMI::PDIVUW(); return true;
        case 14: R5900::Interpreter::OpcodeImpl::MMI::PCPYUD(); return true;
        case 18: R5900::Interpreter::OpcodeImpl::MMI::POR(); return true;
        case 19: R5900::Interpreter::OpcodeImpl::MMI::PNOR(); return true;
        case 26: R5900::Interpreter::OpcodeImpl::MMI::PEXCH(); return true;
        case 27: R5900::Interpreter::OpcodeImpl::MMI::PCPYH(); return true;
        case 30: R5900::Interpreter::OpcodeImpl::MMI::PEXCW(); return true;
        default: return false; }
    case 48: R5900::Interpreter::OpcodeImpl::MMI::PMFHL(); return true;
    case 49: R5900::Interpreter::OpcodeImpl::MMI::PMTHL(); return true;
    case 52: R5900::Interpreter::OpcodeImpl::MMI::PSLLH(); return true;
    case 54: R5900::Interpreter::OpcodeImpl::MMI::PSRLH(); return true;
    case 55: R5900::Interpreter::OpcodeImpl::MMI::PSRAH(); return true;
    case 60: R5900::Interpreter::OpcodeImpl::MMI::PSLLW(); return true;
    case 62: R5900::Interpreter::OpcodeImpl::MMI::PSRLW(); return true;
    case 63: R5900::Interpreter::OpcodeImpl::MMI::PSRAW(); return true;
    default: return false;
    }
    };
    bool handled=run(); if(handled) state=cpuRegs; return handled;
}
