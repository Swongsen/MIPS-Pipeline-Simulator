/* On my honor, I have neither given nor received unauthorized aid on this assignment */

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <math.h>
#include <vector>
#include <utility>
#include <sstream>


using namespace std;

// Creates the memory to data map
map<int,int> calculateDataValues(map<int,string> mem_data){
  int value;
  map<int,int> mem_value;
  for(map<int,string>::iterator it = mem_data.begin(); it != mem_data.end(); it++){
    // Determine if twos complement
    if(it->second.front() == '1'){
      //cout << "\n" << "___________________" << "\n" << "original: " << it->second << "\t";
      value = 0;
      // Logic to convert the twos complemeented number back into normal binary
      // Subtract one first. Logic: turn rightmost 1 to 0 and then convert all the rest of the rightmost 0's to 1
      for(int i = it->second.length()-1; i >= 0; i--){
        if(it->second.at(i) == '1'){
          it->second.at(i) = '0';
          for(int j = it->second.length()-1 - i; j > 0; j--){
            it->second.at(i+j) = '1';
          }

          //cout << "step 1: " << it->second << "\t\n";
          break;
        }
      }
      //cout << "1. " << it->second << "\n";

      // Complement the number back
      for(int i = it->second.length()-1; i >= 0; i--){
        if(it->second.at(i) == '0'){
          it->second.at(i) = '1';
        }
        else if(it->second.at(i) == '1'){
          it->second.at(i) = '0';
        }
      }
      //cout << "2. " << it->second << "\n";

      // Calculate values of the 1's and just multiply it by -1 and add to value.
      for(int i = it->second.length()-1; i>= 0; i--){
        if(it->second.at(i) == '1'){
          //cout << "step 3: " << it->second << "\t\n";
          //cout << "prevalue " << value;
          value = value + pow(2, it->second.length() - i-1);
        //  cout << it->first << "|" << value;
        }
        //cout << "twos\t";
      }
      value = value * -1;
      //cout << "negative:" << value << "\n";
      mem_value.insert(pair<int,int>(it->first, value));
    }
    // Determine if normal representation
    else if(it->second.front() == '0'){
      value = 0;
      // For each bit in the data value, check if it is 1 to add to the sum of the value. Start from right most end and work towards left.
      for(int i = it->second.length()-1; i > 0; i--){
        if(it->second.at(i) == '1'){
          // Make the value equal to previous value + 2^(total length - current length). I.e first value will be 2^(32-31-1) = 2^(0) = 1.
          value = value + pow(2, it->second.length() - i-1);
        }
      }
      //cout << "positive:" << value << "\n";
      mem_value.insert(pair<int, int>(it->first, value));
      //cout << "normal\t";
    }
    //cout << it->second << "\t" << it->first << endl;
  }

  return mem_value;
}

// Prints out the memory data map
void printDataValues(map<int,int> mem_value){
  cout << "\n\nMemory \t\t" << "Numerical Value" << endl;
  for(map<int,int>::iterator it = mem_value.begin(); it != mem_value.end(); it++){
    cout << it->first << "\t\t\t" << it->second << endl;
  }
}

// Prints out the instruction disassembly map
void print_Disassembly(map<string, vector<string>> instruction_disassembly){
  int xx = 0;
  for(map<string ,vector<string>>::iterator it = instruction_disassembly.begin(); it != instruction_disassembly.end(); it++){
    // printed value to make sure that on second iteration through vector list to print instruction, the memory address is not printed as well. Refreshes to false after each row
    bool printed = false;
    for(vector<string>::iterator itr = it->second.begin(); itr != it->second.end(); itr++){
      cout << *itr << "\t";
      if(printed == false){
        cout << it->first << "\t";
        printed = true;
      }
    }
    cout << "\n";
  }
}

// Effectively same as print instruction simulation but it just writes it to text file instead.
void write_Disassembly(map<string, vector<string>> instruction_disassembly, map<int,int> mem_value, map<int,string> mem_data){
  ofstream disassembly("disassembly.txt");
  for(map<string ,vector<string>>::iterator it = instruction_disassembly.begin(); it != instruction_disassembly.end(); it++){
    // printed value to make sure that on second iteration through vector list to print instruction, the memory address is not printed as well. Refreshes to false after each row
    bool printed = false;
    for(vector<string>::iterator itr = it->second.begin(); itr != it->second.end(); itr++){
      disassembly << *itr << "\t";
      if(printed == false){
        disassembly << it->first << "\t";
        printed = true;
      }
    }
    disassembly << "\n";
  }
  // If the memory in mem-value is equal to the memory in mem-instructionbinary then write to disassembly the format: BinaryInstruction | MemoryAddr | Integer value
  for(map<int,int>::iterator it = mem_value.begin(); it != mem_value.end(); it++){
    for(map<int, string>::iterator itr = mem_data.begin(); itr != mem_data.end(); itr++){
      if(it->first == itr->first){
        disassembly << itr->second << "\t" << to_string(it->first) << "\t" << it->second << "\n";
      }
    }
  }
}

void print_Simulation(map<int,int> mem_value,
   int regNum,
   int cycle,
   map<int,string> register_values,
   map<string, vector<string>> instruction_disassembly,
   map<int,string>::iterator it, string instruction,
   map<int, string> preissue_instruction,
   map<int, vector<string>> IFunit_instructions,
   map<int, string> IFunit,
   map<int, string> pre_alu1,
   map<int, string> pre_alu2){
  cout << "--------------------\n";
  // Prints each cycle with format |cycle # | memory address | instruction in english | .......... last part got from instruction dissasembly finding the 2nd vector element at that memory address
  cout << "Cycle: " << cycle << "\n\n";

  // Prints the waiting and executing instructions if there are. If not, print no instruction
  cout << "IF Unit:\n";
  if(IFunit_instructions[0].size() != 0){
    cout << "\tWaiting Instruction: [" << IFunit[0] << "]\n";
  }
  else if(IFunit_instructions[0].size() == 0){
    cout << "\tWaiting Instruction:\n";
  }
  if(IFunit_instructions[1].size() != 0){
    cout << "\tExecuting Instruction: [" << IFunit[1] << "]\n";
  }
  else if(IFunit_instructions[1].size() == 0){
    cout << "\tExecuting Instruction:\n";
  }

  cout << "Pre-Issue Queue:\n";
  for(int i = 0; i < preissue_instruction.size(); i++){
    cout << "\tEntry " << i << ": [" << preissue_instruction.at(i) << "]\n";
  }
  if(preissue_instruction.size() != 4){
    for(int i = preissue_instruction.size(); i < 4; i++){
      cout << "\tEntry " << i << ": " << "\n";
    }
  }

  cout << "Pre-ALU1 Queue: \n";
  for(int i = 0; i < pre_alu1.size(); i++){
    cout << "\tEntry " << i << ": [" << pre_alu1.at(i) << "]\n";
  }
  if(pre_alu1.size() != 2){
    for(int i = pre_alu1.size(); i < 2; i++){
      cout << "\tEntry " << i << ": " << "\n";
    }
  }

  cout << "Pre-MEM Queue: \n";
  cout << "Post-MEM Queue: \n";

  cout << "Pre-ALU2 Queue: \n";
  cout << "\tEntry 0: " << "\n";
  cout << "\tEntry 1: " << "\n";

  cout << "Post-ALU2 Queue: " << "\n\n";
  for(int i = 0; i < pre_alu2.size(); i++){
    cout << "\tEntry " << i << ": [" << pre_alu2.at(i) << "]\n";
  }
  if(pre_alu2.size() != 2){
    for(int i = pre_alu2.size(); i < 2; i++){
      cout << "\tEntry " << i << ": " << "\n";
    }
  }

  cout << "Registers";
  for(map<int,string>::iterator regItr = register_values.begin(); regItr != register_values.end(); regItr++){
    // For printing out the register row names
    if(regNum % 8 == 0){
      if(regNum == 0)
        cout << "\nR00:\t";
      else if(regNum == 8)
        cout << "\nR08:\t";
      else
        cout << "\nR" << regNum << ":\t";
    }
    cout << regItr->second << "\t";
    regNum++;
  }
  cout << "\n";
  cout << "Data";
  int dataCounter = 0;
  for(map<int, int>::iterator it = mem_value.begin(); it != mem_value.end(); it++){
    if(dataCounter == 0){
      cout << "\n" << it->first << ":\t" << it->second << "\t";
      dataCounter = 8;
    }
    else
      cout << it->second << "\t";
    dataCounter--;
  }
  cout << "\n";
}

// Have to pass in the output file to allow changes every iteration as opposed to writing all at once like the disassembly
void write_Simulation(ofstream &simulation, map<int,int> mem_value, int regNum, int cycle, map<int,string> register_values, map<string, vector<string>> instruction_disassembly, map<int,string>::iterator it, string instruction){
  simulation << "--------------------\n";
  // Prints each cycle with format |cycle # | memory address | instruction in english | .......... last part got from instruction dissasembly finding the 2nd vector element at that memory address
  simulation << "Cycle: " << cycle << "\n\n";

  simulation << "IF Unit:\n";
  simulation << "\tWaiting Instruction: " << "\n";
  simulation << "\tExecuting Instruction: " << "\n";

  simulation << "Pre-Issue Queue:\n";
  simulation << "\tEntry 0: " << "\n";
  simulation << "\tEntry 1: " << "\n";
  simulation << "\tEntry 2: " << "\n";
  simulation << "\tEntry 3: " << "\n";

  simulation << "Pre-ALU1 Queue: \n";
  simulation << "\tEntry 0: " << "\n";
  simulation << "\tEntry 1: " << "\n";

  simulation << "Pre-MEM Queue: \n";
  simulation << "Post-MEM Queue: \n";

  simulation << "Pre-ALU2 Queue: \n";
  simulation << "\tEntry 0: " << "\n";
  simulation << "\tEntry 1: " << "\n";

  simulation << "Post-ALU2 Queue: " << "\n\n";


  simulation << "Registers";
  for(map<int,string>::iterator regItr = register_values.begin(); regItr != register_values.end(); regItr++){
    // For printing out the register row names
    if(regNum % 8 == 0){
      if(regNum == 0)
        simulation << "\nR00:\t";
      else if(regNum == 8)
        simulation << "\nR08:\t";
      else
        simulation << "\nR" << regNum << ":\t";
    }
    simulation << regItr->second << "\t";
    regNum++;
  }
  simulation << "\n";
  simulation << "Data";
  int dataCounter = 0;
  for(map<int, int>::iterator it = mem_value.begin(); it != mem_value.end(); it++){
    if(dataCounter == 0){
      simulation << "\n" << it->first << ":\t" << it->second << "\t";
      dataCounter = 8;
    }
    else
      simulation << it->second << "\t";
    dataCounter--;
  }
  simulation << "\n";
}

// References instruction_disassembly to add instruction to it. Takes in mem_instruction iterator and instruction for key / values
void addto_instruction_disassembly(map<string, vector<string>> &instruction_disassembly, map<int,string>::iterator it, string instruction){
  // Create new map for the simulation output <stringified memory address | instruction binary -> instructions -> >
  instruction_disassembly.insert(pair<string, vector<string>>(to_string(it->first), vector<string>()));
  // Pushes back the memory location of the instruction
  instruction_disassembly[to_string(it->first)].push_back(it->second);
  // Pushes back the instruction
  instruction_disassembly[to_string(it->first)].push_back(instruction);
}

void addto_instructions_list(map<int, string> &instructions_list, int instructionsCount, string instruction){
  instructions_list.insert(pair<int, string>(instructionsCount, instruction));
}

void print_DataReg(map<int,int> mem_value){
  cout << "Data";
  int dataCounter = 0;
  for(map<int, int>::iterator it = mem_value.begin(); it != mem_value.end(); it++){
    if(dataCounter == 0){
      cout << "\n" << it->first << ":\t" << it->second << "\t";
      dataCounter = 8;
    }
    else
      cout << it->second << "\t";
    dataCounter--;
  }
  cout << "\n";
}

// Populate the register values with 0.
void create_registers(map<int, string> &register_values){
  for(int i = 0; i <= 31; i++){
    register_values.insert(pair<int, string>(i, "0"));
  }
}

// For bitwise operations. Take integer number in register and get 32 bit 2s compement
string createRegisterBitsString(int value){
  string bits = "";
  int currentValue = value;
  // If regular number
  if(value >= 0){
    for(int i = 0; i < 32; i++){
      if(pow(2, 31-i) <= currentValue){
        bits = bits + "1";
        currentValue = currentValue - pow(2, 31-i);
      }
      else if(pow(2, 31-i) > currentValue){
        bits = bits + "0";
      }
    }
  }
  // If two's complemented number
  else if(value < 0){
    // Get the original representation
    currentValue = value * -1;
    for(int i = 0; i < 32; i++){
      if(pow(2, 31-i) <= currentValue){
        bits = bits + "1";
        currentValue = currentValue - pow(2, 31-i);
      }
      else if(pow(2, 31-i) > currentValue){
        bits = bits + "0";
      }
    }
    // Complement the 0's and 1's
    for(int i = 0; i < bits.length(); i++){
      if(bits.at(i) == '0')
        bits.at(i) = '1';
      else if(bits.at(i) == '1')
        bits.at(i) = '0';
    }
    // Add 1 to the complement
    for(int i = 31; i >= 0; i--){
      // Should be first 0 found. Turn(add) 1
      if(bits.at(i) == '0'){
        bits.at(i) = '1';
        break;
      }
      // If the bit is already a 1, make it 0 because thats how bit addition works
      else if(bits.at(i) == '1'){
        bits.at(i) = '0';
      }
    }
  }

  return bits;
}

// For immediate operations, need 16 bit string
string createImmediateBitsString(int immediate){
  string immediateString = "";
  int currentValue = immediate;
  if(immediate >= 0){
    for(int i = 0; i < 16; i++){
      if(pow(2, 15-i) <= currentValue){
        immediateString = immediateString + "1";
        currentValue = currentValue - pow(2, 15-i);
      }
      else if(pow(2, 15-i) > currentValue){
        immediateString = immediateString + "0";
      }
    }
  }
  else if(immediate < 0){
    // Get the original representation
    currentValue = immediate * -1;
    for(int i = 0; i < 16; i++){
      if(pow(2, 15-i) <= currentValue){
        immediateString = immediateString + "1";
        currentValue = currentValue - pow(2, 15-i);
      }
      else if(pow(2, 15-i) > currentValue){
        immediateString = immediateString + "0";
      }
    }
    // Complement the 0's and 1's
    for(int i = 0; i < 16; i++){
      if(immediateString.at(i) == '0')
        immediateString.at(i) = '1';
      else if(immediateString.at(i) == '1')
        immediateString.at(i) = '0';
    }
    // Add 1 to the complement
    for(int i = 15; i >= 0; i--){
      // Should be first 0 found. Turn(add) 1
      if(immediateString.at(i) == '0'){
        immediateString.at(i) = '1';
        break;
      }
      // If the bit is already a 1, make it 0 because thats how bit addition works
      else if(immediateString.at(i) == '1'){
        immediateString.at(i) = '0';
      }
    }
  }
  return immediateString;
}

// Gets the integer representation of a string 2s comp.
int int_ofbits(string bits){
  int ans = 0;
  // If non-negative number
  if(bits.at(0) != '1'){
    for(int i = bits.length()-1; i >= 0; i--){
      if(bits.at(i) == '1'){
        ans = ans + pow(2, bits.length()-1-i);
      }
    }
  }
  // If twos bit negative number. Exact same concept as calculating data values (copy paste w/o couts)
  else if(bits.at(0) == '1'){
    // Logic to convert the twos complemeented number back into normal binary
    // Subtract one first. Logic: turn rightmost 1 to 0 and then convert all the rest of the rightmost 0's to 1
    for(int i = bits.length()-1; i >= 0; i--){
      if(bits.at(i) == '1'){
        bits.at(i) = '0';
        for(int j = bits.length()-1 - i; j > 0; j--){
          bits.at(i+j) = '1';
        }
        break;
      }
    }
    // Complement the number back
    for(int i = bits.length()-1; i >= 0; i--){
      if(bits.at(i) == '0'){
        bits.at(i) = '1';
      }
      else if(bits.at(i) == '1'){
        bits.at(i) = '0';
      }
    }
    // Calculate values of the 1's and just multiply it by -1 and add to value.
    for(int i = bits.length()-1; i>= 0; i--){
      if(bits.at(i) == '1'){
        ans = ans + pow(2, bits.length() - i-1);
      }
    }
    ans = ans * -1;
  }

  return ans;
}

// AND function
string and_function(string xbits, string ybits){
  string bits = "";

  for(int i = 0; i < 32; i++){
    if(xbits.at(i) == '1' && ybits.at(i) == '1'){
      bits = bits + '1';
    }
    else
      bits = bits + '0';
  }
  return bits;
}

// OR function
string or_function(string xbits, string ybits){
  string bits = "";

  for(int i = 0; i < 32; i++){
    if(xbits.at(i) == '1' || ybits.at(i) == '1')
      bits = bits + '1';
    else if(xbits.at(i) == '0' && ybits.at(i) == '0')
      bits = bits + '0';
  }
  return bits;
}

// XOR function
string xor_function(string xbits, string ybits){
  string bits = "";

  for(int i = 0; i < 32; i++){
    if((xbits.at(i) == '1' && ybits.at(i) == '0') || (xbits.at(i) == '0' && ybits.at(i) == '1'))
      bits = bits + '1';
    else
      bits = bits + '0';
  }
  return bits;
}

// NOR function
string nor_function(string xbits, string ybits){
  string bits = "";

  for(int i = 0; i < 32; i++){
    if(xbits.at(i) == '0' && ybits.at(i) == '0')
      bits = bits + '1';
    else
      bits = bits + '0';
  }
  return bits;
}

// Used for checking the bitwise functions
void print_bits(string instruction, string xbits, string ybits, string zbits){
  cout << instruction << "\n";
  cout << "first register  : " << xbits << "\n";
  cout << "second/immediate: " << ybits << "\n";
  cout << "result          : " << zbits << "\n";
}

void instruction_fetch_decode(map<int, string> instrFet, string instruction){

}

// Adds instruction to the preissue vector. Use the preissue_instructions_tokened mapping to cross check with this
void addto_preissue(string instruction, map<int, string> &preissue_instruction, int preissueSpot){
  preissue_instruction.insert(pair<int, string>(preissueSpot, instruction));

}


int main(int args, char **argv){

  // #argv[1] should contain the input file
  fstream file;
  // open up output channel
  ofstream simulation("simulation.txt");
  // Open file and set direction to be input
  file.open(argv[1], ios::in);
  int linenum = 0;
  map<int, string> mem_instruction;
  map<int, string> mem_data;
  map<int, int> mem_value;
  // map<memoryaddr, instruction>
  map<string, vector<string>> instruction_disassembly;
  map<int, string> register_values;

  // ------- Functional Units for the pipeline ------- //
  // These maps are just for printing purposes mostly
  map<int, string> instructions_list;
  map<int, string> preissue_instruction; // Stores the preissue instruction (with commas). Mainly only used for printing purposes
  map<int, string> IFunit; // Stores the 2 IF unit instructions (waiting / executing). Mainly only used for printing purposes
  map<int, string> pre_alu1;
  map<int, string> pre_alu2;
  // These are tokenized mappings of functional units
  map<int, vector<string>> preissue_instructions_tokened; // This should be a <0->3, instruction> mapping to tell what is in the preissue queue
  map<int, vector<string>> IFunit_instructions; // This should be at <0/1, instruction> mapping to tell what is in waiting/executing
  map<int, vector<string>> pre_alu1_instructions;
  map<int, vector<string>> pre_alu2_instructions;

  // ------------------------------------------------- //
  // --------- Variables involved in pipeline -------- //
  int preissueSize = 0;
  int preissueSpot = 0;
  int ifunitSize = 0;
  int ifunitSpot = 0;
  int ifNum = 0;
  bool stalled = false;
  bool prealu1_signal = true;  // Used to tell if the preAlu can load another instruction (1 instruction per cycle)
  bool prealu2_signal = true;
  // ------------------------------------------------- //

  // Base address to start at
  int address = 256;
  // String used to get the category categorybits
  string categorybits;
  // String used to get the opcode after categorybits
  string opcode;
  bool breakhit = false;
  string instruction = "";
  // Initialize cycle to start at 1
  int cycle = 1;
  int instructionsCount = 0;

  if(file.good()){
    //cout << "File open \n";
    while(!file.eof()){
      string line;

      getline(file, line);
      // if(linenum == 0){
      //   cout << "Input line length: " << line.length() << '\n' << endl;
      // }
      // If there is something on the line (not empty)
      if(line != ""){
        categorybits = line.substr(0,2);
        opcode = line.substr(2,4);

        if(breakhit == false){
          mem_instruction.insert(pair<int, string>(address, line));
          instructions_list.insert(pair<int,string>(instructionsCount, ""));
          instructionsCount++;
        }

        if(breakhit == true)
          mem_data.insert(pair<int,string>(address, line));

        if(categorybits == "01" && opcode == "0101")
          breakhit = true;

      }

      address = address + 4;
      linenum++;
    }
  }
  else
    return 0;

  // Store the data values into the memory->values map
  mem_value = calculateDataValues(mem_data);
  create_registers(register_values);

  //cout << "MIPS Code \t\t\t" << "  Category \t" << "Opcode \t" << "Memory address" << endl;
  //cout << "MIPS Code \t\t\t" << "  Memory \t" <<  "Instruction" << endl;
  int iteration = 1;
  int instructionsAmt = instructionsCount;
  while(iteration <= 2){
    // Resetting the instructionsCount to place correctly place in the instructionList map
    instructionsCount = 0;
    for(map<int,string>::iterator it = mem_instruction.begin(); it != mem_instruction.end(); it++){
      // Makes a substring out of the line to find out if the instruction is in category 1 or category 2
      // Start at position 0 - 2 length
      categorybits = it->second.substr(0,2);
      // Start at position 2 - 4 length
      opcode = it->second.substr(2,4);
      instruction = "";
      int rsReg = 0;
      int rtReg = 0;
      int rdReg = 0;
      int immediate = 0;
      int sa = 0; // shift amount
      int shftedRegAmt = 0; // how much to shift register by
      int offset = 0;
      int base = 0; // for LW? SW
      int x = 0;
      int y = 0;
      int z = 0;
      // For the bitwise arithmetic, string of bits of register values used to store back the value
      string xbits = "";
      string ybits = "";
      string zbits = "";
      // For jumps and branches. change iterator to jumper at end of iteration.
      map<int, string>::iterator jumper;

      //cout << "\n" << it->first << "\t" << it->second << " ::";
      if(categorybits == "01"){
        // J Instruction. Shifted right 2 on the instruction bits so shifted left twice to account for that.
        if(opcode == "0000"){
          int jumpaddr = 0;
          //cout << "Jumping to memory: ";
          for(int i = it->second.length() - 1; i > 5; i--){
            if(it->second.at(i) == '1')
              jumpaddr = jumpaddr + pow(2, it->second.length() - i - 1);
          }
          jumpaddr = jumpaddr << 2;
          //cout << jumpaddr << "\t";
          // Make string and append the jump address to it so i can store it for outputting
          instruction = "J #";
          instruction += to_string(jumpaddr);
          if(iteration == 1){
            addto_instruction_disassembly(instruction_disassembly, it, instruction);
            instructions_list[instructionsCount] = instruction;
            instructionsCount++;
          }
          else if(iteration == 2){
            //  cout << "jumpaddr:" << jumpaddr;
            // finds the iterator that has the jump addr as the key.
            jumper = mem_instruction.find(jumpaddr-4);
            //cout << "it: " << it->first << "\t" << it->second << "\n";

          }
        }
        // JR Instruction
        if(opcode == "0001"){

          for(int i = it->second.length() - 22; i >= 6; i--){
            if(it->second.at(i) == '1')
              rsReg = rsReg + pow(2, it->second.length() - 22 - i);
          }
          instruction = "JR R";
          instruction += to_string(rsReg);
          if(iteration == 1){
            addto_instruction_disassembly(instruction_disassembly, it, instruction);
            instructions_list[instructionsCount] = instruction;
            instructionsCount++;
          }
          else if(iteration == 2){
            for(map<int,string>::iterator regItr = register_values.begin(); regItr != register_values.end(); regItr++){
              if(regItr->first == rsReg){
                // Finds the register value stored at the register and stores it to x
                x = stoi(register_values.find(rsReg)->second);
                // make the iterator start where x's value (memory?) is
                jumper = mem_instruction.find(x-4);
              }
            }
          }

        }
        // BEQ Instruction
        if(opcode == "0010"){
          int offset = 0;
          // Offset bits are from 31-16
          for(int i = it->second.length()-1; i >= 16; i--){
            if(it->second.at(i) == '1'){
              offset = offset + pow(2,it->second.length() - i - 1);
            }
          }
          offset = offset << 2;
          // RT Register are from bits 15-11
          for(int i = 15; i >= 11; i--){
            if(it->second.at(i) == '1'){
              rtReg = rtReg + pow(2, 15 - i);
            }
          }
          // RS Register are from bits 10-6
          for(int i = 10; i >= 6; i--){
            if(it->second.at(i) == '1'){
              rsReg = rsReg + pow(2, 10 - i);
            }
          }

          // Combine everything into string for adding to instruction simulation
          instruction = "BEQ R" + to_string(rsReg) + ", R" + to_string(rtReg) + ", #" + to_string(offset);
          if(iteration == 1){
            addto_instruction_disassembly(instruction_disassembly, it, instruction);
            instructions_list[instructionsCount] = instruction;
            instructionsCount++;
          }
          else if(iteration == 2){
            //cout << "reg1. :" << register_values.find(rsReg) -> second << "\n";
            //cout << "reg2. :" << register_values.find(rtReg) -> second << "\n";
            //cout << "rs:" << rsReg << ", " << stoi(register_values.find(rsReg)->second) << "\n";
            if(register_values.find(rsReg)->second == register_values.find(rtReg)->second){
              //cout << "MEMORY ADDRESS:" << (it->first + offset + 4) << "\n";
              jumper = mem_instruction.find(it->first + offset);
            }
            else{
              jumper = it;
            }
          }
        }
        // BLTZ Instruction
        if(opcode == "0011"){
          for(int i = it->second.length() - 1; i >= 16; i--){
            if(it->second.at(i) == '1'){
              offset = offset + pow(2, it->second.length() - 1 - i);
            }
          }
          for(int i = 10; i >= 6; i--){
            if(it->second.at(i) == '1'){
              rsReg = rsReg + pow(2, 10 - i);
            }
          }
          // offset is shifted left 2 bits
          offset = offset << 2;
          instruction = "BLTZ R" + to_string(rsReg) + ", #" + to_string(offset);
          if(iteration == 1){
            addto_instruction_disassembly(instruction_disassembly, it, instruction);
            instructions_list[instructionsCount] = instruction;
            instructionsCount++;
          }
          else if(iteration == 2){
            //cout << "rs:" << rsReg << ", " << stoi(register_values.find(rsReg)->second) << "\n";
            if(stoi(register_values.find(rsReg)->second) < 0){
              jumper = mem_instruction.find((it->first + offset));
            }
            else{
              jumper = it;
            }
          }
        }
        // BGTZ Instruction
        if(opcode == "0100"){
          for(int i = it->second.length() - 1; i >= 16; i--){
            if(it->second.at(i) == '1'){
              offset = offset + pow(2, it->second.length() - 1 - i);
            }
          }
          for(int i = 10; i >= 6; i--){
            if(it->second.at(i) == '1'){
              rsReg = rsReg + pow(2, 10 - i);
            }
          }
          // offset is shifted left 2 bits
          offset = offset << 2;
          instruction = "BGTZ R" + to_string(rsReg) + ", #" + to_string(offset);
          if(iteration == 1){
            addto_instruction_disassembly(instruction_disassembly, it, instruction);
            instructions_list[instructionsCount] = instruction;
            instructionsCount++;
          }
          else if(iteration == 2){
            //cout << "rs:" << rsReg << ", " << stoi(register_values.find(rsReg)->second) << "\n";
            if(stoi(register_values.find(rsReg)->second) > 0){
              jumper = mem_instruction.find((it->first + offset));
            }
            else{
              jumper = it;
            }
          }
        }
        // BREAK Instruction **functionality other than denoting data is next??
        if(opcode == "0101"){
          instruction = "BREAK";
          if(iteration == 1){
            addto_instruction_disassembly(instruction_disassembly, it, instruction);
            instructions_list[instructionsCount] = instruction;
            instructionsCount++;
          }
          else if(iteration == 2){
            cycle++;
          }
        }
        // SW Instruction
        if(opcode == "0110"){
          for(int i = it->second.length() - 1; i >= 16; i--){
            if(it->second.at(i) == '1'){
              offset = offset + pow(2, it->second.length() - i - 1);
            }
          }
          for(int i = 15; i >= 11; i--){
            if(it->second.at(i) == '1'){
              rtReg = rtReg + pow(2, 15-i);
            }
          }
          for(int i = 10; i >= 6; i--){
            if(it->second.at(i) == '1'){
              base = base + pow(2, 10 - i);
            }
          }
          // For the actual word to load, take the offset 340 to base R0 and every increment of 4 is the next data register.
          instruction = "SW R" + to_string(rtReg) + ", " + to_string(offset) + "(R" + to_string(base) + ")";
          if(iteration == 1){
            addto_instruction_disassembly(instruction_disassembly, it, instruction);
            instructions_list[instructionsCount] = instruction;
            instructionsCount++;
          }
          else if(iteration == 2){
            int addr = (offset + stoi(register_values.find(base)->second));
            for(map<int,int>::iterator dataItr = mem_value.begin(); dataItr != mem_value.end(); dataItr++){
              if(addr == dataItr->first){
                //cout << "addr:" << addr;
                //cout << "value: " << register_values.find(rtReg)->second;
                mem_value.at(addr) = stoi(register_values.find(rtReg)->second);
              }
            }
            cycle++;
          }
        }
        // LW Instruction
        if(opcode == "0111"){

          for(int i = it->second.length() - 1; i >= 16; i--){
            if(it->second.at(i) == '1'){
              offset = offset + pow(2, it->second.length() - i - 1);
            }
          }
          for(int i = 15; i >= 11; i--){
            if(it->second.at(i) == '1'){
              rtReg = rtReg + pow(2, 15-i);
            }
          }
          for(int i = 10; i >= 6; i--){
            if(it->second.at(i) == '1'){
              base = base + pow(2, 10 - i);
            }
          }
          // For the actual word to load, take the offset 340 to base R0 and every increment of 4 is the next data register.
          instruction = "LW R" + to_string(rtReg) + ", " + to_string(offset) + "(R" + to_string(base) + ")";
          if(iteration == 1){
            addto_instruction_disassembly(instruction_disassembly, it, instruction);
            instructions_list[instructionsCount] = instruction;
            instructionsCount++;
          }
          else if(iteration == 2){
            int addr = (offset + stoi(register_values.find(base)->second));
            int memoryval = mem_value.find(addr)->second;
            //cout << "addr:" << addr << " |memval:" << memoryval;
            register_values.erase(rtReg);
            register_values.insert(pair<int,string>(rtReg, to_string(memoryval)));
          }
          cycle++;
        }
        // SLL Instruction  //31-26
        if(opcode == "1000"){
          for(int i = it->second.length() - 7; i >= 21; i--){
            if(it->second.at(i) == '1')
              sa = sa + pow(2, 25 - i);
          }
          for(int i = 20; i >= 16; i--){
            if(it->second.at(i) == '1')
              rdReg = rdReg + pow(2, 20 - i);
          }
          for(int i = 15; i >= 11; i--){
            if(it->second.at(i) == '1')
              rtReg = rtReg + pow(2, 15 - i);
          }
          // Keep the new value of rdReg in another variable
          shftedRegAmt = stoi(register_values.find(rtReg)->second);
          // For each shift amount, shift left bits 1.
          for(int i = sa; i > 0; i--){
            shftedRegAmt = shftedRegAmt << 1;
          }
          instruction = "SLL R" + to_string(rdReg) + ", R" + to_string(rtReg) + ", #" + to_string(sa);
          if(iteration == 1){
            addto_instruction_disassembly(instruction_disassembly, it, instruction);
            instructions_list[instructionsCount] = instruction;
            instructionsCount++;
          }
          else if(iteration == 2){
            // Remove current register value and replace with the new one
            register_values.erase(rdReg);
            register_values.insert(pair<int, string>(rdReg, to_string(shftedRegAmt)));
            // cout << "rdreg:" << rdReg << "|amt: " << to_string(shftedRegAmt);
            cycle++;
          }
        }
        // SRL Instruction
        if(opcode == "1001"){
          for(int i = it->second.length() - 7; i >= 21; i--){
            if(it->second.at(i) == '1')
              sa = sa + pow(2, 25 - i);
          }
          for(int i = 20; i >= 16; i--){
            if(it->second.at(i) == '1')
              rdReg = rdReg + pow(2, 20 - i);
          }
          for(int i = 15; i >= 11; i--){
            if(it->second.at(i) == '1')
              rtReg = rtReg + pow(2, 15 - i);
          }
          // Keep the new value of rdReg in another variable
          shftedRegAmt = stoi(register_values.find(rtReg)->second);
          // For each shift amount, shift left bits 1.
          for(int i = sa; i > 0; i--){
            shftedRegAmt = shftedRegAmt >> 1;
          }
          instruction = "SRL R" + to_string(rdReg) + ", R" + to_string(rtReg) + ", #" + to_string(sa);
          if(iteration == 1){
            addto_instruction_disassembly(instruction_disassembly, it, instruction);
            instructions_list[instructionsCount] = instruction;
            instructionsCount++;
          }
          else if(iteration == 2){
            // Remove current register value and replace with the new one
            register_values.erase(rdReg);
            register_values.insert(pair<int, string>(rdReg, to_string(shftedRegAmt)));
            cycle++;
          }
        }
        // SRA Instruction
        if(opcode == "1010"){

          for(int i = it->second.length() - 7; i >= 21; i--){
            if(it->second.at(i) == '1')
              sa = sa + pow(2, 25 - i);
          }
          for(int i = 20; i >= 16; i--){
            if(it->second.at(i) == '1')
              rdReg = rdReg + pow(2, 20 - i);
          }
          for(int i = 15; i >= 11; i--){
            if(it->second.at(i) == '1')
              rtReg = rtReg + pow(2, 15 - i);
          }
          // Keep the new value of rdReg in another variable
          shftedRegAmt = stoi(register_values.find(rtReg)->second);
          // For each shift amount, shift left bits 1.
          for(int i = sa; i > 0; i--){
            shftedRegAmt = shftedRegAmt >> 1;
          }
          // turn shiftedregamt integer into 32 bit string and then manipulate
          // currently working as SRL? (need to implement sign bit thing)?
          // tested with negative numbers and positive and seem to work same??
          //string signbit = "";
          //string arithString = "";
          instruction = "SRA R" + to_string(rdReg) + ", R" + to_string(rtReg) + ", #" + to_string(sa);
          if(iteration == 1){
            addto_instruction_disassembly(instruction_disassembly, it, instruction);
            instructions_list[instructionsCount] = instruction;
            instructionsCount++;
          }
          else if(iteration == 2){
            // Remove current register value and replace with the new one
            register_values.erase(rdReg);
            register_values.insert(pair<int, string>(rdReg, to_string(shftedRegAmt)));
            cycle++;
          }
         }
        // NOP Instruction
        if(opcode == "1011"){
          instruction = "NOP";
          if(iteration == 1){
            addto_instruction_disassembly(instruction_disassembly, it, instruction);
            instructions_list[instructionsCount] = instruction;
            instructionsCount++;
          }
          else if(iteration == 2){
            cycle++;
          }
        }
      }
      else if(categorybits == "11"){
        // Use struct and create function to reduce redundancy if there is extra time

        // ADD Instruction
        if(opcode == "0000"){
          for(int i = 20; i >= 16; i--){
            if(it->second.at(i) == '1'){
              rdReg = rdReg + pow(2, 20 - i);
            }
          }
          for(int i = 15; i >= 11; i--){
            if(it->second.at(i) == '1'){
              rtReg = rtReg + pow(2, 15 - i);
            }
          }
          for(int i = 10; i >= 6; i--){
            if(it->second.at(i) == '1'){
              rsReg = rsReg + pow(2, 10 - i);
            }
          }
          instruction = "ADD R" + to_string(rdReg) + ", R" + to_string(rsReg) + ", R" + to_string(rtReg);
          if(iteration == 1){
            addto_instruction_disassembly(instruction_disassembly, it, instruction);
            instructions_list[instructionsCount] = instruction;
            instructionsCount++;
          }
          else if(iteration == 2){
            for(map<int,string>::iterator regItr = register_values.begin(); regItr != register_values.end(); regItr++){
              if(regItr->first == rdReg){
                x = stoi(register_values.at(rsReg));
                y = stoi(register_values.at(rtReg));
                // result of the two registers
                z = x + y;

                register_values.at(rdReg) = to_string(z);
              }
            }
            cycle++;

          }
        }
        // SUB Instruction
        if(opcode == "0001"){
          for(int i = 20; i >= 16; i--){
            if(it->second.at(i) == '1'){
              rdReg = rdReg + pow(2, 20 - i);
            }
          }
          for(int i = 15; i >= 11; i--){
            if(it->second.at(i) == '1'){
              rtReg = rtReg + pow(2, 15 - i);
            }
          }

          for(int i = 10; i >= 6; i--){
            if(it->second.at(i) == '1'){
              rsReg = rsReg + pow(2, 10 - i);
            }
          }

          instruction = "SUB R" + to_string(rdReg) + ", R" + to_string(rsReg) + ", R" + to_string(rtReg);
          if(iteration == 1){
            addto_instruction_disassembly(instruction_disassembly, it, instruction);
            instructions_list[instructionsCount] = instruction;
            instructionsCount++;
          }
          else if(iteration == 2){
            for(map<int,string>::iterator regItr = register_values.begin(); regItr != register_values.end(); regItr++){
              if(regItr->first == rdReg){
                x = stoi(register_values.at(rsReg));
                y = stoi(register_values.at(rtReg));
                // result of the two registers
                z = x - y;

                register_values.at(rdReg) = to_string(z);
              }
            }

            cycle++;

          }
        }
        // MUL Instruction
        if(opcode == "0010"){
          for(int i = 20; i >= 16; i--){
            if(it->second.at(i) == '1'){
              rdReg = rdReg + pow(2, 20 - i);
            }
          }
          for(int i = 15; i >= 11; i--){
            if(it->second.at(i) == '1'){
              rtReg = rtReg + pow(2, 15 - i);
            }
          }

          for(int i = 10; i >= 6; i--){
            if(it->second.at(i) == '1'){
              rsReg = rsReg + pow(2, 10 - i);
            }
          }

          instruction = "MUL R" + to_string(rdReg) + ", R" + to_string(rsReg) + ", R" + to_string(rtReg);
          if(iteration == 1){
            addto_instruction_disassembly(instruction_disassembly, it, instruction);
            instructions_list[instructionsCount] = instruction;
            instructionsCount++;
          }
          else if(iteration == 2){
            for(map<int,string>::iterator regItr = register_values.begin(); regItr != register_values.end(); regItr++){
              if(regItr->first == rdReg){
                x = stoi(register_values.at(rsReg));
                y = stoi(register_values.at(rtReg));
                // cout << "x:" << x << "  | y:" << y;
                // result of the two registers
                z = x * y;

                register_values.at(rdReg) = to_string(z);
              }
            }
            cycle++;

          }
        }
        // AND Instruction
        if(opcode == "0011"){
          for(int i = 20; i >= 16; i--){
            if(it->second.at(i) == '1'){
              rdReg = rdReg + pow(2, 20 - i);
            }
          }
          for(int i = 15; i >= 11; i--){
            if(it->second.at(i) == '1'){
              rtReg = rtReg + pow(2, 15 - i);
            }
          }

          for(int i = 10; i >= 6; i--){
            if(it->second.at(i) == '1'){
              rsReg = rsReg + pow(2, 10 - i);
            }
          }

          instruction = "AND R" + to_string(rdReg) + ", R" + to_string(rsReg) + ", R" + to_string(rtReg);
          if(iteration == 1){
            addto_instruction_disassembly(instruction_disassembly, it, instruction);
            instructions_list[instructionsCount] = instruction;
            instructionsCount++;
          }
          else if(iteration == 2){
            x = stoi(register_values.find(rsReg)->second);
            y = stoi(register_values.find(rtReg)->second);
            xbits = createRegisterBitsString(x);
            ybits = createRegisterBitsString(y);
            zbits = and_function(xbits, ybits);
            //print_bits(instruction, xbits, ybits, zbits);
            z = int_ofbits(zbits);
            //cout << "resultInt:" << z << "\n\n";
            register_values.at(rdReg) = to_string(z);
            //cout << z;
            //cout << "x, xbits:" << x << "," << xbits << "|" << xbits.length();
            //cout << "\ny, ybits:" << y << "," << ybits << "|" << ybits.length();
            //cout << "\nzbits: " << zbits;
            cycle++;
          }
        }
        // OR Instruction
        if(opcode == "0100"){
          for(int i = 20; i >= 16; i--){
            if(it->second.at(i) == '1'){
              rdReg = rdReg + pow(2, 20 - i);
            }
          }
          for(int i = 15; i >= 11; i--){
            if(it->second.at(i) == '1'){
              rtReg = rtReg + pow(2, 15 - i);
            }
          }

          for(int i = 10; i >= 6; i--){
            if(it->second.at(i) == '1'){
              rsReg = rsReg + pow(2, 10 - i);
            }
          }

          instruction = "OR R" + to_string(rdReg) + ", R" + to_string(rsReg) + ", R" + to_string(rtReg);
          if(iteration == 1){
            addto_instruction_disassembly(instruction_disassembly, it, instruction);
            instructions_list[instructionsCount] = instruction;
            instructionsCount++;
          }
          else if(iteration == 2){
            x = stoi(register_values.find(rsReg)->second);
            y = stoi(register_values.find(rtReg)->second);
            xbits = createRegisterBitsString(x);
            ybits = createRegisterBitsString(y);
            zbits = or_function(xbits, ybits);
            //print_bits(instruction, xbits, ybits, zbits);
            z = int_ofbits(zbits);
            register_values.at(rdReg) = to_string(z);
            cycle++;
          }
        }
        // XOR Instruction
        if(opcode == "0101"){
          for(int i = 20; i >= 16; i--){
            if(it->second.at(i) == '1'){
              rdReg = rdReg + pow(2, 20 - i);
            }
          }
          for(int i = 15; i >= 11; i--){
            if(it->second.at(i) == '1'){
              rtReg = rtReg + pow(2, 15 - i);
            }
          }

          for(int i = 10; i >= 6; i--){
            if(it->second.at(i) == '1'){
              rsReg = rsReg + pow(2, 10 - i);
            }
          }

          instruction = "XOR R" + to_string(rdReg) + ", R" + to_string(rsReg) + ", R" + to_string(rtReg);
          if(iteration == 1){
            addto_instruction_disassembly(instruction_disassembly, it, instruction);
            instructions_list[instructionsCount] = instruction;
            instructionsCount++;
          }
          else if(iteration == 2){
            x = stoi(register_values.find(rsReg)->second);
            y = stoi(register_values.find(rtReg)->second);
            xbits = createRegisterBitsString(x);
            ybits = createRegisterBitsString(y);
            zbits = xor_function(xbits, ybits);
            //print_bits(instruction, xbits, ybits, zbits);
            z = int_ofbits(zbits);
            register_values.at(rdReg) = to_string(z);
            cycle++;
          }
        }
        // NOR Instruction
        if(opcode == "0110"){
          for(int i = 20; i >= 16; i--){
            if(it->second.at(i) == '1'){
              rdReg = rdReg + pow(2, 20 - i);
            }
          }
          for(int i = 15; i >= 11; i--){
            if(it->second.at(i) == '1'){
              rtReg = rtReg + pow(2, 15 - i);
            }
          }

          for(int i = 10; i >= 6; i--){
            if(it->second.at(i) == '1'){
              rsReg = rsReg + pow(2, 10 - i);
            }
          }

          instruction = "NOR R" + to_string(rdReg) + ", R" + to_string(rsReg) + ", R" + to_string(rtReg);
          if(iteration == 1){
            addto_instruction_disassembly(instruction_disassembly, it, instruction);
            instructions_list[instructionsCount] = instruction;
            instructionsCount++;
          }
          else if(iteration == 2){
            x = stoi(register_values.find(rsReg)->second);
            y = stoi(register_values.find(rtReg)->second);
            xbits = createRegisterBitsString(x);
            ybits = createRegisterBitsString(y);
            zbits = nor_function(xbits, ybits);
            //print_bits(instruction, xbits, ybits, zbits);
            z = int_ofbits(zbits);
            register_values.at(rdReg) = to_string(z);
            cycle++;
          }
        }
        // SLT Instruction
        if(opcode == "0111"){
          for(int i = 20; i >= 16; i--){
            if(it->second.at(i) == '1'){
              rdReg = rdReg + pow(2, 20 - i);
            }
          }
          for(int i = 15; i >= 11; i--){
            if(it->second.at(i) == '1'){
              rtReg = rtReg + pow(2, 15 - i);
            }
          }

          for(int i = 10; i >= 6; i--){
            if(it->second.at(i) == '1'){
              rsReg = rsReg + pow(2, 10 - i);
            }
          }

          instruction = "SLT R" + to_string(rdReg) + ", R" + to_string(rsReg) + ", R" + to_string(rtReg);
          if(iteration == 1){
            addto_instruction_disassembly(instruction_disassembly, it, instruction);
            instructions_list[instructionsCount] = instruction;
            instructionsCount++;
          }
          else if(iteration == 2){
            x = stoi(register_values.find(rsReg)->second);
            y = stoi(register_values.find(rtReg)->second);
            // if rs < rt, set rd equal to 1. if not, set rd equal to 0
            if(x < y)
              z = 1;
            else
              z = 0;
            register_values.at(rdReg) = to_string(z);
            cycle++;
          }
        }
        // ADDI Instruction
        if(opcode == "1000"){
          // Calculate the immediate value (16 bits)
          for(int i = it->second.length() - 1; i >= 16; i--){
            if(it->second.at(i) == '1'){
              immediate = immediate + pow(2, it->second.length() - i - 1);
            }
          }
          // Calculate the rt register (5 bits)
          for(int i = 15; i >= 11; i--){
            if(it->second.at(i) == '1'){
              rtReg = rtReg + pow(2, 15 - i);
            }
          }

          // Calculate the rs register (5 bits)
          for(int i = 10; i >= 6; i--){
            if(it->second.at(i) == '1'){
              rsReg = rsReg + pow(2, 10 - i);
            }
          }
          string immediateString = createImmediateBitsString(immediate);
          //cout << "immediatestring:" << immediateString << "  | " << immediateString.length();
          immediate = int_ofbits(immediateString);
          instruction = "ADDI R" + to_string(rtReg) + ", R" + to_string(rsReg) + ", #" + to_string(immediate);
          if(iteration == 1){
            addto_instruction_disassembly(instruction_disassembly, it, instruction);
            instructions_list[instructionsCount] = instruction;
            instructionsCount++;
          }
          else if(iteration == 2){
            // Remove current register value and replace with the new one
            x = stoi(register_values.find(rsReg)->second);
            z = x + immediate;
            register_values.at(rtReg) = to_string(z);
            cycle++;
          }
        }
        // ANDI Instruction
        if(opcode == "1001"){
          for(int i = it->second.length() - 1; i >= 16; i--){
            if(it->second.at(i) == '1'){
              immediate = immediate + pow(2, it->second.length() - i - 1);
            }
          }
          for(int i = 15; i >= 11; i--){
            if(it->second.at(i) == '1'){
              rtReg = rtReg + pow(2, 15-i);
            }
          }
          for(int i = 10; i >= 6; i--){
            if(it->second.at(i) == '1'){
              rsReg = rsReg + pow(2, 10 - i);
            }
          }
          string immediateString = createImmediateBitsString(immediate);
          //cout << "immediatestring:" << immediateString << "  | " << immediateString.length();
          immediate = int_ofbits(immediateString);
          instruction = "ANDI R" + to_string(rtReg) + ", R" + to_string(rsReg) + ", #" + to_string(immediate);
          if(iteration == 1){
            addto_instruction_disassembly(instruction_disassembly, it, instruction);
            instructions_list[instructionsCount] = instruction;
            instructionsCount++;
          }
          else if(iteration == 2){
            x = stoi(register_values.find(rsReg)->second);
            xbits = createRegisterBitsString(x);
            if(immediate >= 0 ){
              immediateString = "0000000000000000" + immediateString;
              ybits = immediateString;
            }
            else if(immediate < 0){
              immediateString = "1111111111111111" + immediateString;
              ybits = immediateString;
            }
            zbits = and_function(xbits, immediateString);
            //print_bits(instruction, xbits, ybits, zbits);
            z = int_ofbits(zbits);
            //cout << "resultInt:" << z << "\n\n";
            register_values.at(rtReg) = to_string(z);
            cycle++;
          }
        }
        // ORI Instruction
        if(opcode == "1010"){

          for(int i = it->second.length() - 1; i >= 16; i--){
            if(it->second.at(i) == '1'){
              immediate = immediate + pow(2, it->second.length() - i - 1);
            }
          }
          for(int i = 15; i >= 11; i--){
            if(it->second.at(i) == '1'){
              rtReg = rtReg + pow(2, 15-i);
            }
          }
          for(int i = 10; i >= 6; i--){
            if(it->second.at(i) == '1'){
              rsReg = rsReg + pow(2, 10 - i);
            }
          }
          string immediateString = createImmediateBitsString(immediate);
          //cout << "immediatestring:" << immediateString << "  | " << immediateString.length();
          immediate = int_ofbits(immediateString);
          instruction = "ORI R" + to_string(rtReg) + ", R" + to_string(rsReg) + ", #" + to_string(immediate);
          if(iteration == 1){
            addto_instruction_disassembly(instruction_disassembly, it, instruction);
            instructions_list[instructionsCount] = instruction;
            instructionsCount++;
          }
          else if(iteration == 2){
            x = stoi(register_values.find(rsReg)->second);
            xbits = createRegisterBitsString(x);
            if(immediate >= 0 ){
              immediateString = "0000000000000000" + immediateString;
              ybits = immediateString;
            }
            else if(immediate < 0){
              immediateString = "1111111111111111" + immediateString;
              ybits = immediateString;
            }
            zbits = or_function(xbits, immediateString);
            //print_bits(instruction, xbits, ybits, zbits);
            z = int_ofbits(zbits);
            //cout << "resultInt:" << z << "\n\n";
            register_values.at(rtReg) = to_string(z);
            cycle++;
          }
        }
        // XORI Instruction
        if(opcode == "1011"){
          for(int i = it->second.length() - 1; i >= 16; i--){
            if(it->second.at(i) == '1'){
              immediate = immediate + pow(2, it->second.length() - i - 1);
            }
          }
          for(int i = 15; i >= 11; i--){
            if(it->second.at(i) == '1'){
              rtReg = rtReg + pow(2, 15-i);
            }
          }
          for(int i = 10; i >= 6; i--){
            if(it->second.at(i) == '1'){
              rsReg = rsReg + pow(2, 10 - i);
            }
          }
          string immediateString = createImmediateBitsString(immediate);
          //cout << "immediatestring:" << immediateString << "  | " << immediateString.length();
          immediate = int_ofbits(immediateString);
          instruction = "XORI R" + to_string(rtReg) + ", R" + to_string(rsReg) + ", #" + to_string(immediate);
          if(iteration == 1){
            addto_instruction_disassembly(instruction_disassembly, it, instruction);
            instructions_list[instructionsCount] = instruction;
            instructionsCount++;
          }
          else if(iteration == 2){
            x = stoi(register_values.find(rsReg)->second);
            xbits = createRegisterBitsString(x);
            if(immediate >= 0 ){
              immediateString = "0000000000000000" + immediateString;
              ybits = immediateString;
            }
            else if(immediate < 0){
              immediateString = "1111111111111111" + immediateString;
              ybits = immediateString;
            }
            zbits = xor_function(xbits, immediateString);
            //print_bits(instruction, xbits, ybits, zbits);
            z = int_ofbits(zbits);
            //cout << "resultInt:" << z << "\n\n";
            register_values.at(rtReg) = to_string(z);
            cycle++;
          }
        }
      }
      // Start of the actual simulation. if(iteration ==1) is only for disassembly purposes.
      if(iteration == 2){
        int regNum = 0;
        // If it's a jump, I have to increase the cycle number so that it'll print out its jump state before changing the iterator to the one right before current iterator??
        if( (opcode == "0000" || opcode == "0001" || opcode == "0010" || opcode == "0011" || opcode == "0100") && (categorybits == "01") ){
          cycle++;
        }

        cout << "Instructions: " << instructionsAmt << "\n";
        string instruction1 = instructions_list[ifNum];
        string instruction2 = instructions_list[ifNum+1];

        // If there are 2 or more spots open in the preissue
        cout << "preissuesize: " << preissueSize << "\n";

        // Loading to the IF or preissue
        if(preissueSize <= 2 && stalled != true){
          // Variables needed to tokenize the string
          stringstream ss(instruction1);
          stringstream ss2(instruction2);
          vector<string> tokens;
          string temp_str = "";
          int pos = 0;
          string instruct_temp = "";
          bool branchInstruction = false;
          bool breakInstr = false;
          bool ready = true;

          // Takes instruction1 and turns it into tokens, storing the first one as map key and the rest as a vector of strings to the key.
          while(getline(ss, temp_str, ' ')){
            if(pos == 0){
              // These commands do not go to the preissue Queue
              if(temp_str == "BREAK" || temp_str == "NOP" || temp_str == "J" || temp_str == "JR" || temp_str == "BEQ" || temp_str == "BLTZ" || temp_str == "BGTZ"){
                branchInstruction = true;
              }
              // If the instruction is not any of the not allowed ones then add it to the prequeue map
              if(branchInstruction == false){
                preissue_instructions_tokened.insert(pair<int, vector<string>>(preissueSpot, vector<string>()));
                preissue_instructions_tokened[preissueSpot].push_back(temp_str);
                pos++;
                // Make the first string token (the operation) and save it to allow for pushing back to its vector<string>
                instruct_temp = temp_str;
              }
              // Else, if it is a branch operation, insert it into one of two spots (0 for waiting / stlaled, and 1 for executing )
              else if(branchInstruction == true){
                pos++;
                instruct_temp = temp_str;

                if(temp_str == "J"){
                  ready = true;
                }
                else{

                }

                ready = false;
                // If the operation is waiting on operands or something, it is not ready (add to waiting)
                if(ready == false){
                  ifunitSpot = 0;
                  IFunit.insert(pair<int, string>(0, instruction1));
                  IFunit_instructions.insert(pair<int, vector<string>>(ifunitSpot, vector<string>()));
                  IFunit_instructions[ifunitSpot].push_back(temp_str);
                }
                // If the operation is ready and has an immediate or does not need to wait for anytthing (executing)
                else if(ready == true){
                  ifunitSpot = 1;
                  IFunit.insert(pair<int, string>(1, instruction1));
                  IFunit_instructions.insert(pair<int, vector<string>>(ifunitSpot, vector<string>()));
                  IFunit_instructions[ifunitSpot].push_back(temp_str);
                }

              }
            }
            else{
              // If there is an ',' in the token string, remove it so it can be placed in map without it.
              for(int i = 0; i < temp_str.length(); i++){
                if(temp_str.at(i) == ','){
                  temp_str = temp_str.substr(0, i);
                }
              }
              // Store the token in the preissue map
              if(branchInstruction == false){
                preissue_instructions_tokened[preissueSpot].push_back(temp_str);
              }
              // Store the token in the IFunit map
              else if(branchInstruction == true){
                IFunit_instructions[ifunitSpot].push_back(temp_str);
              }
            }
          }
          if(branchInstruction == false){
            addto_preissue(instruction1, preissue_instruction, preissueSpot);
            preissueSize++;
            preissueSpot++;
          }
          // If the first instruction is a branch type instruction, delete the second instruction
          if(branchInstruction == true)
            instruction2 = "";


          temp_str = "";
          pos = 0;
          // If the first instruction was not a branch type and not a break instruction.
          if(breakInstr == false && instruction2 != ""){
            while(getline(ss2, temp_str, ' ')){
              if(pos == 0){
                preissue_instructions_tokened.insert(pair<int, vector<string>>(preissueSpot, vector<string>()));
                pos++;
                instruct_temp = temp_str;
              }
              else{
                for(int i = 0; i < temp_str.length(); i++){
                  if(temp_str.at(i) == ','){
                    temp_str = temp_str.substr(0, i);
                  }
                }
                preissue_instructions_tokened[preissueSpot].push_back(temp_str);
              }
            }
            addto_preissue(instruction2, preissue_instruction, preissueSpot);
            preissueSize++;
            preissueSpot++;
          }

        }
        // Else if there is only space for 1 more in the preissue. Add case for IF for waiting / executing instructions
        else if(preissueSize == 3 && stalled != true){
          // Variables needed to tokenize the string
          stringstream ss(instruction1);
          vector<string> tokens;
          string temp_str = "";
          int pos = 0;
          string instruct_temp = "";

          // Takes instruction1 and turns it into tokens, storing the first one as map key and the rest as a vector of strings to the key.
          while(getline(ss, temp_str, ' ')){
            if(pos == 0){
              preissue_instructions_tokened.insert(pair<int, vector<string>>(preissueSpot, vector<string>()));
              pos++;
              instruct_temp = temp_str;
            }
            else{
              // If there is an ',' in the token string, remove it so it can be placed in map without it.
              for(int i = 0; i < temp_str.length(); i++){
                if(temp_str.at(i) == ','){
                  temp_str = temp_str.substr(0, i);
                }
              }
              preissue_instructions_tokened[preissueSpot].push_back(temp_str);
            }
          }
          addto_preissue(instruction1, preissue_instruction, preissueSpot);
          preissueSize++;
          preissueSpot++;
        }
        // if there is no space in the preissue, cause a stall for next cycle so it doesnt put more into the pipeline
        if(preissueSize == 4){
          // If preissue size is 4 at the end of this stage, stall it?
          stalled = true;
        }

        // If the cycle of the simulation is not the first, aka there is stuff in the preissue
        if(cycle > 1){
          prealu1_signal = true;
          for(int i = 0; i < preissue_instructions_tokened.size(); i++){
            // Scan through the preissue. If able to load and the instruction is a load or store, take it out of the preissue and move it to the preALU1
            if(preissue_instructions_tokened[i].at(0) == "SW" || preissue_instructions_tokened[i].at(0) == "LW" && prealu1_signal == true){
              int pre1size = pre_alu1_instructions.size();
              pre_alu1_instructions.insert(pair<int, vector<string>>(pre1size, vector<string>()));

              // For the instruction, add it to the respective prealu1 slot
              for(int j = 0; j < preissue_instructions_tokened[i].size(); j++){
                pre_alu1_instructions[pre1size].push_back(preissue_instructions_tokened[i].at(j));
              }
              pre_alu1.insert(pair<int, string>(pre1size, preissue_instruction.at(i)));
              //preissue_instruction.erase(i);
              prealu1_signal = false;
            }
            if(prealu2_signal == true){
              int pre2size = pre_alu2_instructions.size();
              pre_alu2_instructions.insert(pair<int, vector<string>>(pre2size, vector<string>()));

              for(int j = 0; j < preissue_instructions_tokened[i].size(); j++){
                pre_alu2_instructions[pre2size].push_back(preissue_instructions_tokened[i].at(j));
              }
              pre_alu2.insert(pair<int,string>(pre2size, preissue_instruction.at(i)));

              prealu2_signal = false;
            }

          }

        }

        // Prints mapping !! instructions in pipeline !!
        for(map<int, vector<string>>::iterator it = preissue_instructions_tokened.begin(); it != preissue_instructions_tokened.end(); it++){
          for(int i = 0; i < it->second.size(); i++){
            cout << it->second.at(i) << "-> ";
          }
          cout << "\n\n";
        }
        cout << "instruction" << ifNum << ": " << instruction1 << "\n";
        cout << "instruction" << ifNum+1 << ": " << instruction2 << "\n\n";
        ifNum = ifNum + 2;

        cout << "cycle:: " << cycle << "\n";
        print_Simulation(mem_value, regNum, cycle, register_values, instruction_disassembly, it, instruction, preissue_instruction, IFunit_instructions, IFunit, pre_alu1, pre_alu2);
        write_Simulation(simulation, mem_value, regNum, cycle, register_values, instruction_disassembly, it, instruction);
        // If jump, set the current iterator to be equal to the instruction right before where we want so that when it loops around it'll be the actual one we want
        // Jumper is set previously in the other if statement, but the iterator is turned into the jump addr here at the end.
        if( (opcode == "0000" || opcode == "0001" || opcode == "0010" || opcode == "0011" || opcode == "0100") && (categorybits == "01") ){
          it = jumper;
        }

      }

    }
    if(iteration == 1){
      write_Disassembly(instruction_disassembly, mem_value, mem_data);
      // Reset the cycle for the beginning of iteration 2 (simulation)
      cycle = 0;
    }

    // Only reaches here once the code is done due to the it = jumper line in iteration == 2 causing the program to shoot back up.

    // The iterations are first iteration for the disassembly to be finished and then second iteration for actual register work
    iteration++;
  }

  return 0;
}
