#include <iostream>
#include <string>
#include <vector>
#include <bitset>
#include <fstream>
using namespace std;

#define MemSize 1000

static string joinPath(const string &dir, const string &file) {
    if (dir.empty() || dir == ".") {
        return file;
    }
    if (dir.back() == '/') {
        return dir + file;
    }
    return dir + "/" + file;
}

static void printUsage(const char *prog) {
    cout << "Usage: " << prog << " [--imem <path>] [--dmem <path>] [--outdir <dir>]" << endl;
    cout << "Defaults: imem=./imem.txt dmem=./dmem.txt outdir=." << endl;
}

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

    void outputRF(const string &out_path) {
        ofstream rfout;
        rfout.open(out_path, std::ios_base::app);
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
    explicit INSMem(const string &imem_path) {
        IMem.resize(MemSize);
        ifstream imem;
        string line;
        int i = 0;
        imem.open(imem_path);
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
    explicit DataMem(const string &dmem_path) {
        DMem.resize(MemSize);
        ifstream dmem;
        string line;
        int i = 0;
        dmem.open(dmem_path);
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

    void outputDataMem(const string &out_path) {
        ofstream dmemout;
        dmemout.open(out_path);
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

static string boolOrX(bool value, bool valid) {
    if (!valid) {
        return "X";
    }
    return value ? "1" : "0";
}

static string ulongOrX(unsigned long value, bool valid) {
    if (!valid) {
        return "X";
    }
    return to_string(value);
}

template <size_t N>
static string bitsetOrX(const bitset<N> &value, bool valid) {
    if (!valid) {
        return "X";
    }
    return value.to_string();
}

void printState(stateStruct state, int cycle, const string &out_path) {
    ofstream printstate;
    printstate.open(out_path, std::ios_base::app);
    if (printstate.is_open()) {
        bool if_valid = !state.IF.nop;
        bool id_valid = !state.ID.nop;
        bool ex_valid = !state.EX.nop;
        bool mem_valid = !state.MEM.nop;
        bool wb_valid = !state.WB.nop;

        printstate << "State after executing cycle:\t" << cycle << endl;

        printstate << "IF.PC:\t" << ulongOrX(state.IF.PC.to_ulong(), if_valid) << endl;
        printstate << "IF.nop:\t" << state.IF.nop << endl;

        printstate << "ID.Instr:\t" << bitsetOrX(state.ID.Instr, id_valid) << endl;
        printstate << "ID.nop:\t" << state.ID.nop << endl;


        printstate << "EX.Read_data1:\t" << bitsetOrX(state.EX.Read_data1, ex_valid) << endl;
        printstate << "EX.Read_data2:\t" << bitsetOrX(state.EX.Read_data2, ex_valid) << endl;
        printstate << "EX.Imm:\t" << bitsetOrX(state.EX.Imm, ex_valid) << endl;
        printstate << "EX.Rs:\t" << bitsetOrX(state.EX.Rs, ex_valid) << endl;
        printstate << "EX.Rt:\t" << bitsetOrX(state.EX.Rt, ex_valid) << endl;
        printstate << "EX.Wrt_reg_addr:\t" << bitsetOrX(state.EX.Wrt_reg_addr, ex_valid) << endl;
        printstate << "EX.is_I_type:\t" << boolOrX(state.EX.is_I_type, ex_valid) << endl;
        printstate << "EX.rd_mem:\t" << boolOrX(state.EX.rd_mem, ex_valid) << endl;
        printstate << "EX.wrt_mem:\t" << boolOrX(state.EX.wrt_mem, ex_valid) << endl;
        printstate << "EX.alu_op:\t" << boolOrX(state.EX.alu_op, ex_valid) << endl;
        printstate << "EX.wrt_enable:\t" << boolOrX(state.EX.wrt_enable, ex_valid) << endl;
        printstate << "EX.nop:\t" << state.EX.nop << endl;


        printstate << "MEM.ALUresult:\t" << bitsetOrX(state.MEM.ALUresult, mem_valid) << endl;
        printstate << "MEM.Store_data:\t" << bitsetOrX(state.MEM.Store_data, mem_valid) << endl;
        printstate << "MEM.Rs:\t" << bitsetOrX(state.MEM.Rs, mem_valid) << endl;
        printstate << "MEM.Rt:\t" << bitsetOrX(state.MEM.Rt, mem_valid) << endl;
        printstate << "MEM.Wrt_reg_addr:\t" << bitsetOrX(state.MEM.Wrt_reg_addr, mem_valid) << endl;
        printstate << "MEM.rd_mem:\t" << boolOrX(state.MEM.rd_mem, mem_valid) << endl;
        printstate << "MEM.wrt_mem:\t" << boolOrX(state.MEM.wrt_mem, mem_valid) << endl;
        printstate << "MEM.wrt_enable:\t" << boolOrX(state.MEM.wrt_enable, mem_valid) << endl;
        printstate << "MEM.nop:\t" << state.MEM.nop << endl;


        printstate << "WB.Wrt_data:\t" << bitsetOrX(state.WB.Wrt_data, wb_valid) << endl;
        printstate << "WB.Rs:\t" << bitsetOrX(state.WB.Rs, wb_valid) << endl;
        printstate << "WB.Rt:\t" << bitsetOrX(state.WB.Rt, wb_valid) << endl;
        printstate << "WB.Wrt_reg_addr:\t" << bitsetOrX(state.WB.Wrt_reg_addr, wb_valid) << endl;
        printstate << "WB.wrt_enable:\t" << boolOrX(state.WB.wrt_enable, wb_valid) << endl;
        printstate << "WB.nop:\t" << state.WB.nop << endl;
    }
    else cout << "Unable to open file";
    printstate.close();
}

int main(int argc, char *argv[]) {
    string imem_path = "imem.txt";
    string dmem_path = "dmem.txt";
    string outdir = ".";

    for (int i = 1; i < argc; i++) {
        string arg = argv[i];
        if (arg == "--help") {
            printUsage(argv[0]);
            return 0;
        }
        if (arg == "--imem" && i + 1 < argc) {
            imem_path = argv[++i];
            continue;
        }
        if (arg == "--dmem" && i + 1 < argc) {
            dmem_path = argv[++i];
            continue;
        }
        if (arg == "--outdir" && i + 1 < argc) {
            outdir = argv[++i];
            continue;
        }
        cout << "Unknown or incomplete option: " << arg << endl;
        printUsage(argv[0]);
        return 1;
    }

    RF myRF;
    INSMem myInsMem(imem_path);
    DataMem myDataMem(dmem_path);
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

    bool branch_taken = false;

    int stalled_cycle = -1;

    int cycle = 0;

    while (1) {

        stateStruct prevState = state;
        branch_taken = false;

        //unstall
        if (stalled_cycle == cycle && state.EX.nop) {
            state.ID.nop = false;
            state.EX.nop = false;
            state.IF.nop = false;
            stalled_cycle = -1;
        }

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
                if (state.EX.Imm[15] == 1) {                                                    // neagtive immediate
                    state.EX.Imm.flip();
                    state.EX.Imm = state.EX.Imm.to_ulong() + 1;                                 // two's complement

                    alu_result = state.EX.Read_data1.to_ulong() - state.EX.Imm.to_ulong();
                }
                else {
                    alu_result = state.EX.Read_data1.to_ulong() + state.EX.Imm.to_ulong();
                }
            }
            else if (state.EX.alu_op && !state.EX.is_I_type) {                                  // addu
                alu_result = state.EX.Read_data1.to_ulong() + state.EX.Read_data2.to_ulong();
            }
            else if (!state.EX.alu_op && !state.EX.is_I_type){                                                                              // subu
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
            
            state.EX.Imm = (state.ID.Instr & bitset<32>(0xFFFF)).to_ulong();
            state.EX.Rs = ((state.ID.Instr >> 21) & bitset<32>(0x1F)).to_ulong();
            state.EX.Rt = ((state.ID.Instr >> 16) & bitset<32>(0x1F)).to_ulong();
            state.EX.Read_data1 = myRF.readRF(state.EX.Rs);
            state.EX.Read_data2 = myRF.readRF(state.EX.Rt);
            state.EX.wrt_mem = (opcode == bitset<6>(0x2B));                                                             // set if instruction is sw
            state.EX.is_I_type = (opcode == bitset<6>(0x23) || opcode == bitset<6>(0x2B) || opcode == bitset<6>(0x05)); // set if instruction is I-type (lw, sw, bne)
            state.EX.rd_mem = (opcode == bitset<6>(0x23));                                                              // set if instruction is lw
            state.EX.alu_op = (funct == bitset<6>(0x21) || opcode == bitset<6>(0x23) || opcode == bitset<6>(0x2B));     // set if instruction is addu, lw, or sw
            state.EX.wrt_enable = (opcode == bitset<6>(0x00) || opcode == bitset<6>(0x23));                             // set if instruction is lw or R-type


            //add-lw forwarding from MEM
            if (state.EX.rd_mem && state.MEM.wrt_enable && !state.MEM.rd_mem && state.MEM.Wrt_reg_addr == state.EX.Rs) {
                state.EX.Read_data1 = state.MEM.ALUresult;
            }

            // add-bne forwarding from MEM
            if (opcode == bitset<6>(0x05) && state.MEM.wrt_enable && !state.MEM.rd_mem && state.MEM.Wrt_reg_addr == state.EX.Rs && state.MEM.Wrt_reg_addr == state.EX.Rt) {
                state.EX.Read_data1 = state.MEM.ALUresult;
                state.EX.Read_data2 = state.MEM.ALUresult;
            }
            else if (opcode == bitset<6>(0x05) && state.MEM.wrt_enable && !state.MEM.rd_mem && state.MEM.Wrt_reg_addr == state.EX.Rs) {
                state.EX.Read_data1 = state.MEM.ALUresult;
            }
            else if (opcode == bitset<6>(0x05) && state.MEM.wrt_enable && !state.MEM.rd_mem && state.MEM.Wrt_reg_addr == state.EX.Rt) {
                state.EX.Read_data2 = state.MEM.ALUresult;
            }

            //add-sw forwarding from MEM
            if (state.EX.wrt_mem && state.MEM.Wrt_reg_addr == state.EX.Rt) {
                state.EX.Read_data2 = state.MEM.ALUresult;
            }

            //lw-add forwarding from MEM
            if (state.MEM.rd_mem && opcode == bitset<6>(0x00) && state.MEM.Wrt_reg_addr == state.EX.Rs && state.MEM.Wrt_reg_addr == state.EX.Rt) {
                state.EX.Read_data1 = myDataMem.readDataMem(state.MEM.ALUresult);
                state.EX.Read_data2 = myDataMem.readDataMem(state.MEM.ALUresult);
            }
            else if (state.MEM.rd_mem && opcode == bitset<6>(0x00) && state.MEM.Wrt_reg_addr == state.EX.Rs) {
                // forwarding for Rs
                state.EX.Read_data1 = myDataMem.readDataMem(state.MEM.ALUresult);
            }
            else if (state.MEM.rd_mem && opcode == bitset<6>(0x00) && state.MEM.Wrt_reg_addr == state.EX.Rt) {
                // forwarding for Rt
                state.EX.Read_data2 = myDataMem.readDataMem(state.MEM.ALUresult);
            }

            //lw(1)-lw(2)-addu forwarding lw(1) from WB to addu
            if (state.WB.wrt_enable && state.WB.Wrt_reg_addr == state.EX.Rs && state.WB.Wrt_reg_addr == state.EX.Rt) {
                state.EX.Read_data1 = state.WB.Wrt_data;
                state.EX.Read_data2 = state.WB.Wrt_data;
            }
            else if (opcode == bitset<6>(0x00) && state.WB.wrt_enable && state.WB.Wrt_reg_addr == state.EX.Rs) {
                state.EX.Read_data1 = state.WB.Wrt_data;
            }
            else if (opcode == bitset<6>(0x00) && state.WB.wrt_enable && state.WB.Wrt_reg_addr == state.EX.Rt) {
                state.EX.Read_data2 = state.WB.Wrt_data;
            }

            // forwarding for add-add hazard
            if (!state.EX.nop && !state.MEM.rd_mem && !state.MEM.wrt_mem && opcode == bitset<6>(0x00) && state.MEM.Wrt_reg_addr == state.EX.Rs && state.MEM.Wrt_reg_addr == state.EX.Rt) {// Rs, Rt
                state.EX.Read_data1 = state.MEM.ALUresult;
                state.EX.Read_data2 = state.MEM.ALUresult;
            }
            else if (!state.EX.nop && !state.MEM.rd_mem && !state.MEM.wrt_mem && opcode == bitset<6>(0x00) && state.MEM.Wrt_reg_addr == state.EX.Rs){ // Rs
                state.EX.Read_data1 = state.MEM.ALUresult;
            }
            else if (!state.EX.nop && !state.MEM.rd_mem && !state.MEM.wrt_mem && opcode == bitset<6>(0x00) && state.MEM.Wrt_reg_addr == state.EX.Rt) { // Rt
                state.EX.Read_data2 = state.MEM.ALUresult;
            }

            if (opcode == bitset<6>(0x00)) {                                                                            // addu, subu (Rd)
                state.EX.Wrt_reg_addr = ((state.ID.Instr >> 11) & bitset<32>(0x1F)).to_ulong();
            }
            else {                                                                                                      // lw (Rt)
                state.EX.Wrt_reg_addr = ((state.ID.Instr >> 16) & bitset<32>(0x1F)).to_ulong();
            }
            state.EX.is_I_type = (opcode == bitset<6>(0x23) || opcode == bitset<6>(0x2B) || opcode == bitset<6>(0x05)); // set if instruction is I-type (lw, sw, bne)
            state.EX.rd_mem = (opcode == bitset<6>(0x23));                                                              // set if instruction is lw
            state.EX.alu_op = (funct == bitset<6>(0x21) || opcode == bitset<6>(0x23) || opcode == bitset<6>(0x2B));     // set if instruction is addu, lw, or sw
            state.EX.wrt_enable = (opcode == bitset<6>(0x00) || opcode == bitset<6>(0x23));                             // set if instruction is lw or R-type

            //stall
            if (!state.MEM.nop && state.MEM.rd_mem && (state.MEM.Wrt_reg_addr == state.EX.Rs || state.MEM.Wrt_reg_addr == state.EX.Rt)) {
                state.ID.nop = true;
                state.EX.nop = true;
                stalled_cycle = cycle+2;
            }

            // Additional logic to keep the instruction in the ID stage:
            if (cycle < stalled_cycle) {
                // state.ID = prevState.ID;   // Keep the instruction in the ID stage
                state.ID.nop = true;
                state.EX.nop = true;
            }
            if (state.ID.Instr == bitset<32>(0xFFFFFFFF)){
                state.ID.nop = true;
            }

            //bne
            if (opcode == bitset<6>(0x05) && state.EX.Read_data1 != state.EX.Read_data2) {      // Check the branch condition, bne
                if (state.EX.Imm[15] == 1) {                                                    //immediate is negative
                    state.EX.Imm.flip();
                    state.EX.Imm = state.EX.Imm.to_ulong() + 1;                                 // two's complement

                    bitset<32> address = state.EX.Imm.to_ulong() << 2;                          // left shift 2 is equal to multiplying by 4

                    state.IF.PC = state.IF.PC.to_ulong() - address.to_ulong() - 4;
                }
                else {
                    bitset<32> address = state.EX.Imm.to_ulong() << 2; // left shift 2 is equal to multiplying by 4

                    // Branch is TAKEN
                    state.IF.PC = state.IF.PC.to_ulong() + address.to_ulong();
                }
                branch_taken = true;
            }
            state.EX.nop = state.ID.nop;
            
        }
        else if (state.IF.nop == true && state.ID.nop == true) {
            state.EX.nop = true;
        }

        /* --------------------- IF stage --------------------- */
        if (!state.IF.nop) {

            // IF/ID
            if (cycle < stalled_cycle) {
                // state.IF = prevState.IF;                         // Keep the instruction in the IF stage
                state.ID.Instr = myInsMem.readInstr(state.IF.PC);   // Fetch the new instruction only when not stalling
                state.IF.PC = state.IF.PC.to_ulong() + 4;
                state.IF.nop = true;
            } 
            else if (state.ID.Instr == bitset<32>(0xFFFFFFFF)) {                          //if instruction is halt then set nop of all stages to true
                state.IF.nop = true;
                state.ID.nop = true;
            }
            else {
                    state.ID.Instr = myInsMem.readInstr(state.IF.PC);   // Fetch the new instruction only when not stalling
                    state.ID.nop = false;

                    if (!branch_taken) {
                        state.IF.PC = state.IF.PC.to_ulong() + 4;
                    }
            }
        }

        printState(state, cycle, joinPath(outdir, "stateresult.txt"));
        cycle++;
    }

    myRF.outputRF(joinPath(outdir, "RFresult.txt"));
    myDataMem.outputDataMem(joinPath(outdir, "dmemresult.txt"));

    return 0;
}
