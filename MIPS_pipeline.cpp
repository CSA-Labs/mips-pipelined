#include <iostream>
#include <string>
#include <vector>
#include <bitset>
#include <fstream>
using namespace std;

#define MemSize 1000

struct IFStruct {
    bitset<32>  PC;
    bool        nop;
};

struct IDStruct {
    bitset<32>  Instr;
    bool        nop;
};

struct EXStruct {
    bitset<32>  Read_data1;
    bitset<32>  Read_data2;
    bitset<16>  Imm;
    bitset<5>   Rs;
    bitset<5>   Rt;
    bitset<5>   Wrt_reg_addr;
    bool        is_I_type;
    bool        rd_mem;
    bool        wrt_mem;
    bool        alu_op;
    bool        wrt_enable;
    bool        nop;
};

struct MEMStruct {
    bitset<32>  ALUresult;
    bitset<32>  Store_data;
    bitset<5>   Rs;
    bitset<5>   Rt;
    bitset<5>   Wrt_reg_addr;
    bool        rd_mem;
    bool        wrt_mem;
    bool        wrt_enable;
    bool        nop;
};

struct WBStruct {
    bitset<32>  Wrt_data;
    bitset<5>   Rs;
    bitset<5>   Rt;
    bitset<5>   Wrt_reg_addr;
    bool        wrt_enable;
    bool        nop;
};

struct stateStruct {
    IFStruct    IF;
    IDStruct    ID;
    EXStruct    EX;
    MEMStruct   MEM;
    WBStruct    WB;
};

class RF {
public:
    bitset<32> Reg_data;
    RF() {
        Registers.resize(32);
        Registers[0] = bitset<32>(0);
    }

    bitset<32> readRF(bitset<5> Reg_addr) {
        Reg_data = Registers[Reg_addr.to_ulong()];
        return Reg_data;
    }

    void writeRF(bitset<5> Reg_addr, bitset<32> Wrt_reg_data) {
        Registers[Reg_addr.to_ulong()] = Wrt_reg_data;
    }

    void outputRF() {
        ofstream rfout;
        rfout.open("RFresult.txt", std::ios_base::app);
        if (rfout.is_open()) {
            rfout << "State of RF:\t" << endl;
            for (int j = 0; j < 32; j++) {
                rfout << Registers[j] << endl;
            }
        }
        else cout << "Unable to open file";
        rfout.close();
    }

private:
    vector<bitset<32>> Registers;
};

class INSMem {
public:
    bitset<32> Instruction;
    INSMem() {
        IMem.resize(MemSize);
        ifstream imem;
        string line;
        int i = 0;
        imem.open("imem.txt");
        if (imem.is_open()) {
            while (getline(imem, line)) {
                IMem[i] = bitset<8>(line);
                i++;
            }
        }
        else cout << "Unable to open file";
        imem.close();
    }

    bitset<32> readInstr(bitset<32> ReadAddress) {
        string insmem;
        insmem.append(IMem[ReadAddress.to_ulong()].to_string());
        insmem.append(IMem[ReadAddress.to_ulong() + 1].to_string());
        insmem.append(IMem[ReadAddress.to_ulong() + 2].to_string());
        insmem.append(IMem[ReadAddress.to_ulong() + 3].to_string());
        Instruction = bitset<32>(insmem);
        return Instruction;
    }

private:
    vector<bitset<8>> IMem;
};

class DataMem {
public:
    bitset<32> ReadData;
    DataMem() {
        DMem.resize(MemSize);
        ifstream dmem;
        string line;
        int i = 0;
        dmem.open("dmem.txt");
        if (dmem.is_open()) {
            while (getline(dmem, line)) {
                DMem[i] = bitset<8>(line);
                i++;
            }
        }
        else cout << "Unable to open file";
        dmem.close();
    }

    bitset<32> readDataMem(bitset<32> Address) {
        string datamem;
        datamem.append(DMem[Address.to_ulong()].to_string());
        datamem.append(DMem[Address.to_ulong() + 1].to_string());
        datamem.append(DMem[Address.to_ulong() + 2].to_string());
        datamem.append(DMem[Address.to_ulong() + 3].to_string());
        ReadData = bitset<32>(datamem);
        return ReadData;
    }

    void writeDataMem(bitset<32> Address, bitset<32> WriteData) {
        DMem[Address.to_ulong()] = bitset<8>(WriteData.to_string().substr(0, 8));
        DMem[Address.to_ulong() + 1] = bitset<8>(WriteData.to_string().substr(8, 8));
        DMem[Address.to_ulong() + 2] = bitset<8>(WriteData.to_string().substr(16, 8));
        DMem[Address.to_ulong() + 3] = bitset<8>(WriteData.to_string().substr(24, 8));
    }

    void outputDataMem() {
        ofstream dmemout;
        dmemout.open("dmemresult.txt");
        if (dmemout.is_open()) {
            for (int j = 0; j < 1000; j++) {
                dmemout << DMem[j] << endl;
            }
        }
        else cout << "Unable to open file";
        dmemout.close();
    }

private:
    vector<bitset<8>> DMem;
};

void printState(stateStruct state, int cycle) {
    ofstream printstate;
    printstate.open("stateresult.txt", std::ios_base::app);
    if (printstate.is_open()) {
        printstate << "State after executing cycle:\t" << cycle << endl;

        printstate << "IF.PC:\t" << state.IF.PC.to_ulong() << endl;
        printstate << "IF.nop:\t" << state.IF.nop << endl;

        printstate << "ID.Instr:\t" << state.ID.Instr << endl;
        printstate << "ID.nop:\t" << state.ID.nop << endl;
        printstate << "---------------------------------------------------------------------------------------------------------------\t"<< endl;


        printstate << "EX.Read_data1:\t" << state.EX.Read_data1 << endl;
        printstate << "EX.Read_data2:\t" << state.EX.Read_data2 << endl;
        printstate << "EX.Imm:\t" << state.EX.Imm << endl;
        printstate << "EX.Rs:\t" << state.EX.Rs << endl;
        printstate << "EX.Rt:\t" << state.EX.Rt << endl;
        printstate << "EX.Wrt_reg_addr:\t" << state.EX.Wrt_reg_addr << endl;
        printstate << "EX.is_I_type:\t" << state.EX.is_I_type << endl;
        printstate << "EX.rd_mem:\t" << state.EX.rd_mem << endl;
        printstate << "EX.wrt_mem:\t" << state.EX.wrt_mem << endl;
        printstate << "EX.alu_op:\t" << state.EX.alu_op << endl;
        printstate << "EX.wrt_enable:\t" << state.EX.wrt_enable << endl;
        printstate << "EX.nop:\t" << state.EX.nop << endl;
        printstate << "---------------------------------------------------------------------------------------------------------------\t"<< endl;


        printstate << "MEM.ALUresult:\t" << state.MEM.ALUresult << endl;
        printstate << "MEM.Store_data:\t" << state.MEM.Store_data << endl;
        printstate << "MEM.Rs:\t" << state.MEM.Rs << endl;
        printstate << "MEM.Rt:\t" << state.MEM.Rt << endl;
        printstate << "MEM.Wrt_reg_addr:\t" << state.MEM.Wrt_reg_addr << endl;
        printstate << "MEM.rd_mem:\t" << state.MEM.rd_mem << endl;
        printstate << "MEM.wrt_mem:\t" << state.MEM.wrt_mem << endl;
        printstate << "MEM.wrt_enable:\t" << state.MEM.wrt_enable << endl;
        printstate << "MEM.nop:\t" << state.MEM.nop << endl;
        printstate << "---------------------------------------------------------------------------------------------------------------\t"<< endl;


        printstate << "WB.Wrt_data:\t" << state.WB.Wrt_data << endl;
        printstate << "WB.Rs:\t" << state.WB.Rs << endl;
        printstate << "WB.Rt:\t" << state.WB.Rt << endl;
        printstate << "WB.Wrt_reg_addr:\t" << state.WB.Wrt_reg_addr << endl;
        printstate << "WB.wrt_enable:\t" << state.WB.wrt_enable << endl;
        printstate << "WB.nop:\t" << state.WB.nop << endl;
        printstate << "---------------------------------------------------------------------------------------------------------------\t"<< endl;
        printstate << "Written to RF\t" << endl;
        printstate << "###################################################################################################################\t"<< endl;

    }
    else cout << "Unable to open file";
    printstate.close();
}

int main() {
    RF myRF;
    INSMem myInsMem;
    DataMem myDataMem;
    stateStruct state;    // Contains the state of the pipeline

    // Initialize the pipeline state
    state.IF.nop = false;
    state.ID.nop = true;
    state.EX.nop = true;
    state.MEM.nop = true;
    state.WB.nop = true;

    // initilizing so that it won't have garbage values
    state.EX.is_I_type = false;
    state.EX.rd_mem = false;
    state.EX.wrt_mem = false;
    state.EX.alu_op = true;
    state.EX.wrt_enable = false;

    state.MEM.rd_mem = false;
    state.MEM.wrt_mem = false;
    state.MEM.wrt_enable = false;

    state.WB.wrt_enable = false;

    bool stalled = false;

    int cycle = 0;

    while (1) {

        if (state.IF.nop && state.ID.nop && state.EX.nop && state.MEM.nop && state.WB.nop)
            break;

        /* --------------------- WB stage --------------------- */
        
        if (!state.WB.nop) {

            if (state.WB.wrt_enable) {
                myRF.writeRF(state.WB.Wrt_reg_addr, state.WB.Wrt_data);
            }
        }

        /* --------------------- MEM stage --------------------- */
        if (state.MEM.nop == true) {
            state.WB.nop = state.MEM.nop;
        }
        else if (!state.MEM.nop) {

            // MEM/WB
            state.WB.Rs = state.MEM.Rs;
            state.WB.Rt = state.MEM.Rt;
            state.WB.wrt_enable = state.MEM.wrt_enable;
            state.WB.Wrt_reg_addr = state.MEM.Wrt_reg_addr;


            if (state.MEM.rd_mem) {                                                     // lw
                state.WB.Wrt_data = myDataMem.readDataMem(state.MEM.ALUresult);
            }
            else if (state.MEM.wrt_mem) {                                               // sw
                myDataMem.writeDataMem(state.MEM.ALUresult, state.MEM.Store_data);
                state.WB.Wrt_data = state.MEM.Store_data;                              // not sure if this is correct
            }
            else {                                                                      // addu, subu
                state.WB.Wrt_data = state.MEM.ALUresult;
            }

            state.WB.nop = state.MEM.nop;
        }

        /* --------------------- EX stage --------------------- */
        if (!state.EX.nop) {

            bitset<32> alu_result;

            if (state.EX.alu_op && state.EX.is_I_type) {                                        // lw, sw
                alu_result = state.EX.Read_data1.to_ulong() + state.EX.Imm.to_ulong();
            }
            else if (state.EX.alu_op && !state.EX.is_I_type) {                                  // addu
                alu_result = state.EX.Read_data1.to_ulong() + state.EX.Read_data2.to_ulong();
            }
            else {                                                                              // subu, bne
                alu_result = state.EX.Read_data1.to_ulong() - state.EX.Read_data2.to_ulong();
            }

            // EXE/MEM
            state.MEM.ALUresult = alu_result;
            state.MEM.Store_data = state.EX.Read_data2;
            state.MEM.Rs = state.EX.Rs;
            state.MEM.Rt = state.EX.Rt;
            state.MEM.Wrt_reg_addr = state.EX.Wrt_reg_addr;
            state.MEM.rd_mem = state.EX.rd_mem;
            state.MEM.wrt_mem = state.EX.wrt_mem;
            state.MEM.wrt_enable = state.EX.wrt_enable;

            state.MEM.nop = state.EX.nop;
        }
        else if (state.EX.nop == true) {
            state.MEM.nop = state.EX.nop;
        }

        /* --------------------- ID stage --------------------- */
        if (!state.ID.nop) {

            bitset<6> opcode = (state.ID.Instr >> 26).to_ulong();
            bitset<6> funct = (state.ID.Instr & bitset<32>(0x3F)).to_ulong();
            
            // forwarding for Register-register hazard
            if (!state.EX.nop && !state.EX.is_I_type && opcode == bitset<6>(0x00)){
                if (state.EX.Wrt_reg_addr == state.ID.Instr.to_ulong() >> 21) {   // Rs
                    state.EX.Read_data1 = state.MEM.ALUresult;  // forward to EX stage
                }
                else if (state.EX.Wrt_reg_addr == state.ID.Instr.to_ulong() >> 16) { // Rt
                    state.EX.Read_data2 = state.MEM.ALUresult;  // forward to EX stage
                }
            }
            else { // read Rs/Rt from RF (not forwarding)
                state.EX.Read_data1 = myRF.readRF(((state.ID.Instr >> 21) & bitset<32>(0x1F)).to_ulong());
                state.EX.Read_data2 = myRF.readRF(((state.ID.Instr >> 16) & bitset<32>(0x1F)).to_ulong());
            }

            // ID/EXE
            state.EX.Imm = (state.ID.Instr & bitset<32>(0xFFFF)).to_ulong();
            state.EX.Rs = ((state.ID.Instr >> 21) & bitset<32>(0x1F)).to_ulong();
            state.EX.Rt = ((state.ID.Instr >> 16) & bitset<32>(0x1F)).to_ulong();
            if (opcode == bitset<6>(0x00)) {                                                                            // addu, subu (Rd)
                state.EX.Wrt_reg_addr = ((state.ID.Instr >> 11) & bitset<32>(0x1F)).to_ulong();
            }
            else {                                                                                                      // lw (Rt)
                state.EX.Wrt_reg_addr = ((state.ID.Instr >> 16) & bitset<32>(0x1F)).to_ulong();
            }
            state.EX.is_I_type = (opcode == bitset<6>(0x23) || opcode == bitset<6>(0x2B) || opcode == bitset<6>(0x05)); // set if instruction is I-type (lw, sw, bne)
            state.EX.rd_mem = (opcode == bitset<6>(0x23));                                                              // set if instruction is lw
            state.EX.wrt_mem = (opcode == bitset<6>(0x2B));                                                             // set if instruction is sw
            state.EX.alu_op = (funct == bitset<6>(0x21) || opcode == bitset<6>(0x23) || opcode == bitset<6>(0x2B));     // set if instruction is addu, lw, or sw
            state.EX.wrt_enable = (opcode == bitset<6>(0x00) || opcode == bitset<6>(0x23));                             // set if instruction is lw or R-type


            if (!state.MEM.nop && state.MEM.rd_mem && (state.MEM.Wrt_reg_addr == state.EX.Rs || state.MEM.Wrt_reg_addr == state.EX.Rt)) {
                state.EX.nop = true;
            }
            else if (state.EX.nop && state.MEM.rd_mem && state.MEM.Wrt_reg_addr == state.EX.Rs) {
                state.EX.nop = false;

                // forwarding for Rs
                state.EX.Read_data1 = myRF.readRF(state.MEM.Wrt_reg_addr);  // forward to EX stage
            }
            else if (state.EX.nop && state.MEM.rd_mem && state.MEM.Wrt_reg_addr == state.EX.Rt) {
                state.EX.nop = false;

                // forwarding for Rt
                state.EX.Read_data2 = myRF.readRF(state.MEM.Wrt_reg_addr);  // forward to EX stage 
            }
            else {
                state.EX.nop = state.ID.nop;
            }
            
        }
        else if (state.IF.nop == true && state.ID.nop == true) {
            state.EX.nop = true;
        }

        /* --------------------- IF stage --------------------- */
        if (!state.IF.nop) {

            // IF/ID
            state.ID.Instr = myInsMem.readInstr(state.IF.PC);
            state.ID.nop = false;

            if (state.ID.Instr == bitset<32>(0xFFFFFFFF)) {                          //if instruction is halt then set nop of all stages to true
                state.IF.nop = true;
                state.ID.nop = true;
            }
            else {
                state.IF.PC = state.IF.PC.to_ulong() + 4;
            }
        }

        printState(state, cycle);
        myRF.outputRF(); // dump RF;
        cycle++;
    }

    //myRF.outputRF();
    myDataMem.outputDataMem();

    return 0;
}
