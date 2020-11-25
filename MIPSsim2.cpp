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
   int cycle,
   map<int,string> register_values,
   map<int, string> preissue_instruction,
   map<int, vector<string>> IFunit_instructions_tokened,
   map<int, string> IFunit,
   map<int, string> pre_alu1,
   map<int, string> pre_alu2,
   map<int, string> post_alu2_queue,
   map<int, string> pre_mem_queue,
   map<int,  string> post_mem_queue){
  cout << "--------------------\n";
  // Prints each cycle with format |cycle # | memory address | instruction in english | .......... last part got from instruction dissasembly finding the 2nd vector element at that memory address
  cout << "Cycle: " << cycle << "\n\n";

  // Prints the waiting and executing instructions if there are. If not, print no instruction
  cout << "IF Unit:\n";
  if(IFunit_instructions_tokened[0].size() != 0){
    cout << "\tWaiting Instruction: [" << IFunit[0] << "]\n";
  }
  else if(IFunit_instructions_tokened[0].size() == 0){
    cout << "\tWaiting Instruction:\n";
  }
  if(IFunit_instructions_tokened[1].size() != 0){
    cout << "\tExecuting Instruction: [" << IFunit[1] << "]\n";
  }
  else if(IFunit_instructions_tokened[1].size() == 0){
    cout << "\tExecuting Instruction:\n";
  }

  cout << "Pre-Issue Queue:\n";
  for(map<int, string>::iterator it = preissue_instruction.begin(); it != preissue_instruction.end(); it++){
    cout << "\tEntry " << it->first << ": [" << it->second << "]\n";
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
  if(pre_alu1.size() < 2){
    for(int i = pre_alu1.size(); i < 2; i++){
      cout << "\tEntry " << i << ": " << "\n";
    }
  }

  if(pre_mem_queue.size() > 0){
    if(pre_mem_queue.at(0) != ""){
      cout << "Pre-MEM Queue: [" << pre_mem_queue.at(0) << "]\n";
    }
  }
  else
    cout << "Pre-MEM Queue: \n";

  if(post_mem_queue.size() > 0){
    if(post_mem_queue.at(0) != ""){
      cout << "Post-MEM Queue: [" << post_mem_queue.at(0) << "]\n";
    }
  }
  else
    cout << "Post-MEM Queue: \n";

  cout << "Pre-ALU2 Queue: \n";
  for(int i = 0; i < pre_alu2.size(); i++){
    cout << "\tEntry " << i << ": [" << pre_alu2.at(i) << "]\n";
  }
  if(pre_alu2.size() != 2){
    for(int i = pre_alu2.size(); i < 2; i++){
      cout << "\tEntry " << i << ": " << "\n";
    }
  }

  if(post_alu2_queue.size() > 0){
    if(post_alu2_queue.at(0) != ""){
      cout << "Post-ALU2 Queue: [" << post_alu2_queue.at(0) << "]\n";
    }
  }
  else
    cout << "Post-ALU2 Queue: \n";

  cout << "Registers";
  int regNum = 0;
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
void write_Simulation(ofstream &simulation, map<int,int> mem_value, int cycle, map<int,string> register_values, map<string, vector<string>> instruction_disassembly, map<int,string>::iterator it, string instruction){
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
  int regNum = 0;
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

string memory_sourceString(map<int, vector<string>> preissue_instructions_tokened, int i, string val){
  val= "";
  // If the instruction is a load or store, capture the source register from the instruction
  for(int z = 0; z < preissue_instructions_tokened[i].at(3).length(); z++){
    if(preissue_instructions_tokened[i].at(3).at(z) == 'R'){
      for(int y = z; y < preissue_instructions_tokened[i].at(3).length() - 1; y++){
        val = val + preissue_instructions_tokened[i].at(3).at(y);
      }
    }
  } // end of getting source register
  return val;
}

void print_pipelinedebugging_states(map<int, vector<string>> registers_waited_on,
                                    map<int, vector<string>> destination_registers_inpipeline,
                                    map<int, vector<string>> source_registers_inpipeline,
                                    map<int, vector<string>> memory_registers_waited,
                                    map<int, vector<string>> IFunit_instructions_tokened,
                                    map<int, vector<string>> preissue_instructions_tokened,
                                    map<int, vector<string>> pre_alu1_instructions_tokened,
                                    map<int, vector<string>> pre_mem_queue_tokened,
                                    map<int, vector<string>> post_mem_queue_tokened,
                                    map<int, vector<string>> pre_alu2_instructions_tokened,
                                    map<int, vector<string>> post_alu2_queue_tokened,
                                    map<int, vector<string>> write_back_tokened,
                                    int cycle, int preissueSize){
   cout << "Registers being waited on to branch... ";
   for(int i = 0; i < registers_waited_on.size(); i++){
     for(int j = 0; j < registers_waited_on[i].size(); j++){
       cout << registers_waited_on[i].at(j) << " ";
     }
     cout << "\n";
   }
   cout << "\n";

   cout << "Destys: ";
   if(destination_registers_inpipeline.size() > 0){
     for(map<int, vector<string>>::iterator destItr = destination_registers_inpipeline.begin(); destItr != destination_registers_inpipeline.end(); destItr++){
       cout << "Cycle entered: " << destItr->first << "|";
       for(int i = 0; i < destItr->second.size(); i++){
         cout << destItr->second.at(i) << " ";
       }
       cout << ", ";
     }
   }

   cout << "\nSourceys: ";
   if(source_registers_inpipeline.size() > 0){
     for(map<int, vector<string>>::iterator srcItr = source_registers_inpipeline.begin(); srcItr != source_registers_inpipeline.end(); srcItr++){
       cout << "Cycle entered: " << srcItr->first << " |";
       for(int i = 0; i < srcItr->second.size(); i++){
         cout << srcItr->second.at(i) << " ";
       }
       cout << ",";
     }
   }

   cout << "\nMemory Registers being waited on...";
   for(map<int, vector<string>>::iterator it = memory_registers_waited.begin(); it != memory_registers_waited.end(); it++){
     cout << it->first << " | ";
     for(int i = 0; i < it->second.size(); i++){
       cout << it->second.at(i) << " ";
     }
     cout << "||||";
   }

   cout << "\n";
   cout << "IF_UNIT_QUEUE at end of " << cycle << "\t";
   for(int i = 0; i < IFunit_instructions_tokened.size(); i++){
     cout << "size: " << IFunit_instructions_tokened[i].size() << ", ";
     for(int j = 0; j < IFunit_instructions_tokened[i].size(); j++){
       cout << IFunit_instructions_tokened[i].at(j) << " ";
     }
     cout << "  |||  ";
   }
   cout << "\n";

   cout << "PREISSUE_QUEUE at end of " << cycle << "\t" << "size: " << preissue_instructions_tokened.size();
   for(int i = 0; i < preissueSize; i++){
     cout << "size: " << preissue_instructions_tokened[i].size() << ", ";
     for(int j = 0; j < preissue_instructions_tokened[i].size(); j++){
       cout << preissue_instructions_tokened[i].at(j) << " ";
     }
     cout << "  |||  ";
   }
   cout << "\n";

   cout << "PRE_ALU1_QUEUE at end of " << cycle << "\t";
   for(int i = 0; i < pre_alu1_instructions_tokened.size(); i++){
     cout << "size: " << pre_alu1_instructions_tokened[i].size() << ", ";
     for(int j = 0; j < pre_alu1_instructions_tokened[i].size(); j++){
       cout << pre_alu1_instructions_tokened[i].at(j) << " ";
     }
     cout << "\n";
   }
   cout << "\n";

   cout << "PRE_MEM_QUEUE at end of " << cycle << "\t";
   for(int i = 0; i < pre_mem_queue_tokened.size(); i++){
     cout << "size: " << pre_mem_queue_tokened[i].size() << ", ";
     for(int j = 0; j < pre_mem_queue_tokened[i].size(); j++){
       cout << pre_mem_queue_tokened[i].at(j) << " ";
     }
     cout << "\n";
   }
   cout << "\n";

   cout << "POST_MEM_QUEUE at end of " << cycle << "\t";
   for(int i = 0; i < post_mem_queue_tokened.size(); i++){
     cout << "size: " << post_mem_queue_tokened[i].size() << ", ";
     for(int j = 0; j < post_mem_queue_tokened[i].size(); j++){
       cout << post_mem_queue_tokened[i].at(j) << " ";
     }
     cout << "\n";
   }
   cout << "\n";

   cout << "PRE_ALU2_QUEUE at end of " << cycle << "\t";
   for(int i = 0; i < pre_alu2_instructions_tokened.size(); i++){
     cout << "size: " << pre_alu2_instructions_tokened[i].size() << ", ";
     for(int j = 0; j < pre_alu2_instructions_tokened[i].size(); j++){
       cout << pre_alu2_instructions_tokened[i].at(j) << " ";
     }
     cout << "  |||  ";
   }
   cout << "\n";

   cout << "POST_ALU2_QUEUE at end of " << cycle << "\t";
   for(int i = 0; i < post_alu2_queue_tokened.size(); i++){
     cout << "size: " << post_alu2_queue_tokened[i].size() << ", ";
     for(int j = 0; j < post_alu2_queue_tokened[i].size(); j++){
       cout << post_alu2_queue_tokened[i].at(j) << " ";
     }
     cout << "  |||  ";
   }
   cout << "\n";

   cout << "Instructions being finished (WB) at end of " << cycle << "\t";
   for(int i = 0; i < write_back_tokened.size(); i++){
     cout << "size: " << write_back_tokened[i].size() << ", ";
     for(int j = 0; j < write_back_tokened[i].size(); j++){
       cout << write_back_tokened[i].at(j) << " ";
     }
     cout << "  |||  ";
   }
   cout << "\n\n\n";
}

int main(int args, char **argv){

  // #argv[1] should contain the input file
  fstream file;
  // open up output channel
  ofstream simulation("simulation.txt");
  // Open file and set direction to be input
  file.open(argv[1], ios::in);
  int linenum = 0;
  map<int, string> mem_instruction; // instruction#, instruction (bits)
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
  map<int, vector<string>> memory_registers_waited; // should be <0-1, registers>, with 0 being SW and 1 being LW
  map<int, vector<string>> registers_waited_on; // these are for branch instructions
  map<int, vector<string>> destination_registers_inpipeline;
  map<int, vector<string>> source_registers_inpipeline; // format is <cycle time entered, registers>
  map<int, string> post_alu2_queue;
  map<int, string> pre_mem_queue;
  map<int, string> post_mem_queue;
  // These are tokenized mappings of functional units
  map<int, vector<string>> preissue_instructions_tokened; // This should be a <0->3, cycle entered into preissue, instruction> mapping to tell what is in the preissue queue
  map<int, vector<string>> IFunit_instructions_tokened; // This should be at <0/1, cycle time entered | instruction> mapping to tell what is in waiting/executing
  //map<int, vector<string>> instructions_inpipeline; // This will be in the form of < instruction # , cycle time entered | instruction>
  map<int, vector<string>> pre_alu1_instructions_tokened;
  map<int, vector<string>> pre_alu2_instructions_tokened;
  map<int, vector<string>> post_alu2_queue_tokened;
  map<int, vector<string>> pre_mem_queue_tokened;
  map<int, vector<string>> post_mem_queue_tokened;
  map<int, vector<string>> write_back_tokened;
  map<int, map<int,string>::iterator> jumpspots; // map from <initial memory address spot, iterator spot at that address> use this to jump??

  // ------------------------------------------------- //
  // --------- Variables involved in pipeline -------- //
  string memSource = "";
  int possible_memory_instructions = 0;
  int possible_instructions = 0;
  map<int,string>::iterator go_back_to_execute;
  int preissueSize = 0;
  int preissueSpot = 0;
  int ifunitSize = 0;
  int ifNum = 0;
  bool taken = false;
  bool full_stalled = false;
  bool branch_stalled = false;
  bool prealu1_signal = true;  // Used to tell if the preAlu can load another instruction (1 instruction per cycle)
  bool prealu2_signal = true;
  bool notWB = true;
  bool alu2stall = false;
  bool branchInstruction = false;
  bool movetoExecute = false;
  bool timeToDoBranch = false;
  int wbs_completed = 0;
  bool doneExecuting = false;
  bool noHazards = true;
  bool noRaw = true;
  bool noWaw = true;
  bool memoryReady = true;
  bool jumpImm = false;
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
      // On the first iteration, map out all the memory spots to where they are in terms of program execution?
      if(iteration == 1){
        go_back_to_execute = it;
        jumpspots.insert(pair<int, map<int,string>::iterator>(it->first, go_back_to_execute));
      }
      notWB = true;
      cout << "rn im: " << it->first;
      cout << "\n---------------------------------- RESETTING: -----------------------------------------------";
      // If the instruction disassembly has already finished. Start the simulation before doing the memory instruction access?????
      if(iteration == 2){
        wbs_completed++;
        while(notWB == true){
          cycle++;
          //--------- Reset these variables every cycle; they should be loaded in a new one or removed every cycle. ------------//
          memSource = "";
          alu2stall = false;
          possible_memory_instructions = 0;
          branchInstruction = false;
          jumpImm = false;
          noRaw = true;
          noWaw = true;
          noHazards = true;
          cout << "--------------------------------------\n";
          cout << "Beginning of Cycle:" << cycle << "\n";
          cout << "Instructions: " << instructionsAmt << "\n";

          // Beginning of adding instructions to the preissue
          string instruction1 = "";
          string instruction2 = "";
          timeToDoBranch = false;
          //---------------------------------------------------------------------------------------------------------------------//

          // If the pipeline is unstalled, the previous waiting to execute instruction will now be moved to execute.
          if(branch_stalled == false && movetoExecute == true){
            cout << "Time to jump\n";
            timeToDoBranch = true;
            if(IFunit_instructions_tokened[1].size() == 0){
              IFunit_instructions_tokened.insert(pair<int, vector<string>>(1, vector<string>()));
              for(int i = 1; i < IFunit_instructions_tokened[0].size(); i++){
                IFunit_instructions_tokened[1].push_back(IFunit_instructions_tokened[0].at(i));
              }
              IFunit.insert(pair<int, string>(1, IFunit[0]));
              IFunit.erase(0);
              IFunit_instructions_tokened.erase(0);
            }
            it = go_back_to_execute;
            notWB = false;
          }

          // If it is not time to branch, pipeline steps begin?
          if(timeToDoBranch == false){

            //-------------------------------------------- PREISSUE LOADING --------------------------------------------------//
            cout << "preishh Size: " << preissue_instruction.size() << "\n";
            // if there is no space in the preissue, cause a stall for next cycle so it doesnt put more into the pipeline
            if(preissueSize == 4){
              // If preissue size is 4 at the end of this stage, stall it?
              full_stalled = true;
            }
            // If there is space in the preissue and it is not stalled
            if(preissueSize <= 2 && branch_stalled != true && full_stalled != true){
              instruction1 = instructions_list[ifNum];
              instruction2 = instructions_list[ifNum+1];
              cout << "instruction" << ifNum << ": " << instruction1 << "\n";
              cout << "instruction" << ifNum+1 << ": " << instruction2 << "\n\n";
              ifNum = ifNum + 2;
              // Variables needed to tokenize the string
              stringstream ss(instruction1);
              stringstream ss2(instruction2);
              vector<string> tokens;
              string temp_str = "";
              int pos = 0;
              bool breakInstr = false;
              bool ready = true;
              // Takes instruction1 and turns it into tokens, storing the first one as map key and the rest as a vector of strings to the key.
              while(getline(ss, temp_str, ' ')){
                if(pos == 0){
                  if(temp_str == "J"){
                    branchInstruction = true;
                    ready = true;
                  }
                  // These commands do not go to the preissue Queue
                  else if(temp_str == "BREAK" || temp_str == "NOP" || temp_str == "JR" || temp_str == "BEQ" || temp_str == "BLTZ" || temp_str == "BGTZ"){
                    branchInstruction = true;
                    branch_stalled = true;
                    ready = false;
                    // Save the iteration of detecting a branch
                    go_back_to_execute = it;
                    if(temp_str == "BREAK"){
                      // Assuming BREAK is not the first instruction of the list
                      IFunit_instructions_tokened[1].push_back(temp_str);
                      print_Simulation(mem_value, cycle, register_values, preissue_instruction, IFunit_instructions_tokened, IFunit, pre_alu1, pre_alu2, post_alu2_queue, pre_mem_queue, post_mem_queue);
                      return 0;
                    }
                  }
                  // If the instruction is not any of the not allowed ones then add it to the prequeue map
                  if(branchInstruction == false){
                    preissue_instructions_tokened.insert(pair<int, vector<string>>(preissueSpot, vector<string>()));
                    preissue_instructions_tokened[preissueSpot].push_back(to_string(cycle));
                    preissue_instructions_tokened[preissueSpot].push_back(temp_str);
                    pos++;
                  }
                  // Else, if it is a branch operation, insert it into one of two spots (0 for waiting / stlaled, and 1 for executing )
                  else if(branchInstruction == true){
                    // pos being the position in the string stream (from beginning to end)
                    pos++;
                    // If the operation is waiting on operands or something, it is not ready (add to waiting)
                    if(ready == false){
                      IFunit.insert(pair<int, string>(0, instruction1));
                      IFunit_instructions_tokened.insert(pair<int, vector<string>>(0, vector<string>()));
                      IFunit_instructions_tokened[0].push_back(to_string(cycle));
                      IFunit_instructions_tokened[0].push_back(temp_str);
                    }
                    // If the operation is ready and has an immediate or does not need to wait for anytthing (executing)
                    else if(ready == true){
                      IFunit.insert(pair<int, string>(1, instruction1));
                      IFunit_instructions_tokened.insert(pair<int, vector<string>>(1, vector<string>()));
                      IFunit_instructions_tokened[1].push_back(to_string(cycle));
                      IFunit_instructions_tokened[1].push_back(temp_str);
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
                    if(ready == false){
                      IFunit_instructions_tokened[0].push_back(temp_str);
                      registers_waited_on[0].push_back(temp_str);
                    }
                    else if(ready == true){
                      IFunit_instructions_tokened[1].push_back(temp_str);
                    }
                  }
                }// End of adding the components of the instruction

              }
              if(branchInstruction == false){
                addto_preissue(instruction1, preissue_instruction, preissueSpot);
                preissueSize++;
                preissueSpot++;
              }
              // If the first instruction is a branch type instruction, delete the second instruction
              if(branchInstruction == true){
                instruction2 = "";
                ifNum = ifNum - 1;
                if(ready == false){
                  // Take out the offset of the list of registers being waited on
                  registers_waited_on[0].pop_back();
                }
              }

              temp_str = "";
              pos = 0;
              // If the first instruction was not a branch type and not a break instruction.
              if(breakInstr == false && instruction2 != ""){
                while(getline(ss2, temp_str, ' ')){
                  if(pos == 0){
                    if(temp_str == "J"){
                      branchInstruction = true;
                      ready = true;
                    }
                    // These commands do not go to the preissue Queue
                    else if(temp_str == "BREAK" || temp_str == "NOP" || temp_str == "JR" || temp_str == "BEQ" || temp_str == "BLTZ" || temp_str == "BGTZ"){
                      branchInstruction = true;
                      branch_stalled = true;
                      ready = false;
                      // Save the iteration of detecting a branch
                      go_back_to_execute = it;
                      if(temp_str == "BREAK"){
                        // Assuming BREAK is not the first instruction of the list
                        IFunit_instructions_tokened[1].push_back(temp_str);
                        print_Simulation(mem_value, cycle, register_values, preissue_instruction, IFunit_instructions_tokened, IFunit, pre_alu1, pre_alu2, post_alu2_queue, pre_mem_queue, post_mem_queue);
                        return 0;
                      }
                    }
                    // If the instruction is not any of the not allowed ones then add it to the prequeue map
                    if(branchInstruction == false){
                      preissue_instructions_tokened.insert(pair<int, vector<string>>(preissueSpot, vector<string>()));
                      preissue_instructions_tokened[preissueSpot].push_back(to_string(cycle));
                      preissue_instructions_tokened[preissueSpot].push_back(temp_str);
                      pos++;
                    }
                    // Else, if it is a branch operation, insert it into one of two spots (0 for waiting / stlaled, and 1 for executing )
                    else if(branchInstruction == true){
                      // pos being the position in the string stream (from beginning to end)
                      pos++;
                      // If the operation is waiting on operands or something, it is not ready (add to waiting)
                      if(ready == false){
                        IFunit.insert(pair<int, string>(0, instruction2));
                        IFunit_instructions_tokened.insert(pair<int, vector<string>>(0, vector<string>()));
                        IFunit_instructions_tokened[0].push_back(to_string(cycle));
                        IFunit_instructions_tokened[0].push_back(temp_str);
                      }
                      // If the operation is ready and has an immediate or does not need to wait for anytthing (executing)
                      else if(ready == true){
                        IFunit.insert(pair<int, string>(1, instruction2));
                        IFunit_instructions_tokened.insert(pair<int, vector<string>>(1, vector<string>()));
                        IFunit_instructions_tokened[1].push_back(to_string(cycle));
                        IFunit_instructions_tokened[1].push_back(temp_str);
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
                      if(ready == false){
                        IFunit_instructions_tokened[0].push_back(temp_str);
                        registers_waited_on[0].push_back(temp_str);
                      }
                      else if(ready == true){
                        IFunit_instructions_tokened[1].push_back(temp_str);
                      }
                    }
                  }
                }
                if(branchInstruction == false){
                  addto_preissue(instruction2, preissue_instruction, preissueSpot);
                  preissueSize++;
                  preissueSpot++;
                }
              }
            }
            // Else if there is only space for 1 more in the preissue. Add case for IF for waiting / executing instructions
            else if(preissueSize == 3 && branch_stalled != true && full_stalled != true){
              instruction1 = instructions_list[ifNum];
              cout << "adding fourth instruction: " << instruction1 << "\n";
              ifNum++;
              preissueSpot = 3;
              // Variables needed to tokenize the string
              stringstream ss(instruction1);
              vector<string> tokens;
              string temp_str = "";
              int pos = 0;
              bool ready = true;
              cout << "preissueSpot" << preissueSpot << "\t ";
              // Takes instruction1 and turns it into tokens, storing the first one as map key and the rest as a vector of strings to the key.
              while(getline(ss, temp_str, ' ')){
                if(pos == 0){
                  if(temp_str == "J"){
                    branchInstruction = true;
                    IFunit_instructions_tokened[1].clear();
                    IFunit[1] = instruction1;
                    IFunit_instructions_tokened.insert(pair<int, vector<string>>(1, vector<string>()));
                    IFunit_instructions_tokened[1].push_back(to_string(cycle));
                    IFunit_instructions_tokened[1].push_back(temp_str);
                    ready = true;
                    jumpImm = true;
                    notWB = false;
                  }
                  // These commands do not go to the preissue Queue
                  else if(temp_str == "BREAK" || temp_str == "NOP" || temp_str == "JR" || temp_str == "BEQ" || temp_str == "BLTZ" || temp_str == "BGTZ"){
                    branchInstruction = true;
                    branch_stalled = true;
                    ready = false;
                    // Save the iteration of detecting a branch
                    go_back_to_execute = it;
                    if(temp_str == "BREAK"){
                      // Assuming BREAK is not the first instruction of the list
                      IFunit_instructions_tokened[1].push_back(temp_str);
                      print_Simulation(mem_value, cycle, register_values, preissue_instruction, IFunit_instructions_tokened, IFunit, pre_alu1, pre_alu2, post_alu2_queue, pre_mem_queue, post_mem_queue);
                      return 0;
                    }
                  }else{
                    preissue_instructions_tokened.insert(pair<int, vector<string>>(3, vector<string>()));
                    preissue_instructions_tokened[3].push_back(to_string(cycle));
                    preissue_instructions_tokened[3].push_back(temp_str);
                    pos++;
                  }
                }
                else{
                  // If there is an ',' in the token string, remove it so it can be placed in map without it.
                  for(int i = 0; i < temp_str.length(); i++){
                    if(temp_str.at(i) == ','){
                      temp_str = temp_str.substr(0, i);
                    }
                  }
                  if(branchInstruction == false){
                    preissue_instructions_tokened[3].push_back(temp_str);
                  }
                  else if(branchInstruction == true){
                    if(ready == false){
                      IFunit_instructions_tokened[0].push_back(temp_str);
                      registers_waited_on[0].push_back(temp_str);
                    }
                    else if(ready == true){
                      IFunit_instructions_tokened[1].push_back(temp_str);
                    }
                  }
                }
              }
              // idk
              if(jumpImm == true){
                for(map<int, map<int, string>::iterator>::iterator what = jumpspots.begin(); what != jumpspots.end(); what++){
                  if(what->first == stoi(temp_str.substr(1,3))){
                    go_back_to_execute = what->second;
                  }
                }
                cout << temp_str.substr(1, 3);
                cout << it->first;
                cout << "\t" << go_back_to_execute->first;
                cout << "\nhehehe\n";
                //it = go_back_to_execute;
              }
              if(branchInstruction == false){
                addto_preissue(instruction1, preissue_instruction, preissueSpot);
                preissueSize++;
                preissueSpot++;
              }
            } // End of adding instructions to the preissue
            //----------------------------------------------------------------------------------------------------------------//

            //--------------------------------------- PREISSUE -> ALU1/ALU2 LOADING ------------------------------------------//
            // Moving instructions from the preissue to respective ALUs, while there is still an open prealu and no hazards in the instructions
            // while(noHazards == true && (prealu1_signal == true || prealu2_signal == true)){
            // Goes through every instruction in the preissue
            cout << "Current cycle: " << cycle << "\n";
            for(int i = 0; i < preissueSize; i++){
              // For instructions in the preissue, move to pre alu if it was added in a cycle before current one
              if(stoi(preissue_instructions_tokened[i].at(0)) < cycle){
                string source = "";
                string destination = "";
                cout << "\n\nPreissueSize variable: " << preissueSize << "\n";
                cout << "PreissueSize Actually: " << preissue_instructions_tokened.size() << "\n";
                cout << "Instruction: " << preissue_instruction.at(i) << " entered PreISSUE at: " << preissue_instructions_tokened[i].at(0) << "\n";
                possible_instructions++;
                noWaw = true;
                noRaw = true;
                memoryReady = true; // might be obsolete variable

                //----------------------------------------------------- RAW Checking ---------------------------------------------------------------//
                //-------------------------(Makes sure no sources are dependent on a previous destination) -----------------------------------------//

                // If LW, if it's source register is any of the destination regisers that came before it.
                if(preissue_instructions_tokened[i].at(1) == "LW"){
                  possible_memory_instructions++;
                  source = memory_sourceString(preissue_instructions_tokened, 0, source);
                  cout << "LW Source: " << source << "\n";
                  // If for a LW, the source register is one of the destination registers in the pipeline.
                  for(map<int, vector<string>>::iterator it = destination_registers_inpipeline.begin(); it != destination_registers_inpipeline.end(); it++){
                    if(it->first <= cycle){
                      for(int k = 0; k < it->second.size(); k++){
                        if(source == it->second.at(k)){
                          noRaw = false;
                        }
                      }
                    }
                  }
                }
                // If SW, check it's source registers with any registers in pipeline
                else if(preissue_instructions_tokened[i].at(1) == "SW"){
                  source = preissue_instructions_tokened[i].at(2);
                  cout << "SW Source: " << source << "\n";
                  for(map<int, vector<string>>::iterator it = destination_registers_inpipeline.begin(); it != destination_registers_inpipeline.end(); it++){
                    if(it->first <= cycle){
                      for(int k = 0; k < it->second.size(); k++){
                        if(source == it->second.at(k)){
                          noRaw = false;
                        }
                      }
                    }
                  }
                }
                // Check the source registers of a not memory instruction to make sure none of them are a destination register of a previous instruction
                else if(preissue_instructions_tokened[i].size() == 5){
                  for(map<int, vector<string>>::iterator it = destination_registers_inpipeline.begin(); it != destination_registers_inpipeline.end(); it++){
                    if(it->first <= cycle){
                      for(int k = 0; k < it->second.size(); k++){
                        if(it->second.at(k) == preissue_instructions_tokened[i].at(3) || it->second.at(k) == preissue_instructions_tokened[i].at(4)){
                          noRaw = false;
                          cout << preissue_instructions_tokened[i].at(1) << " ----this shit got raw ---\n";
                        }
                      }
                    }
                  }
                }
                // janky version of making sure a non memory type instruction has no sources that rely on destinations of current instructions in preissue
                if(i > 0){
                  for(int w = i-1; w >0; w--){
                    if(preissue_instructions_tokened[i].size() == 5){
                      if(preissue_instructions_tokened[i].at(3) == preissue_instructions_tokened[w].at(2) || preissue_instructions_tokened[i].at(4) == preissue_instructions_tokened[w].at(2)){
                        noRaw = false;
                      }
                    }
                  }
                }
                //----------------------------------------------- END OF RAW ------------------------------------------------//

                //----------------------------------------------- WAW CHECKING ----------------------------------------------//
                //---------------------------(Make sure no destination registres are same as ones before) -------------------//

                // If not the most ready instruction to move, if the destination registers are the same;
                // signal a WAW hazard and add the register being waited on to the map.
                if(i > 0){
                  for(int w = i-1; w > 0; w--){
                    // For all the instructions above an instruction, i.e: if this is i = 1, or 2nd instruction, checks i=0 or 1st instruction for if destination registers are the same
                    // Check the first argument. If it is the same as an SW, store it in the source(0) memory waited on.
                    // and if it is a LW, store it in the destination(1) memory waited on.
                    // If the registers match, there is a WAW hazard and the instruction at i cannot execute


                    // If not SW instruction, check its destination register with any registers in pipeline
                    if(preissue_instructions_tokened[i].at(1) != "SW"){
                      // If the instruction before it is not a SW instruction
                      if(preissue_instructions_tokened[w].at(1) != "SW" && preissue_instructions_tokened[w].at(2) == preissue_instructions_tokened[i].at(2)){
                        noWaw = false;
                      }
                      // If the instruction before it IS a store word instruction
                      else if(preissue_instructions_tokened[w].at(1) == "SW"){
                        // For store words, the destination is what's in the 300(register), but the sourceString function works the same
                        destination = memory_sourceString(preissue_instructions_tokened, w, destination);
                        cout << "Destination from this SW: " << destination;
                        if(destination == preissue_instructions_tokened[i].at(2)){
                          noWaw = false;
                        }
                      }
                    }
                    // If SW, check its destination registers with any registers in pipeline
                    else if(preissue_instructions_tokened[i].at(1) == "SW"){
                      destination = memory_sourceString(preissue_instructions_tokened, i, destination);
                      if(preissue_instructions_tokened[w].at(1) != "SW" && preissue_instructions_tokened[w].at(2) == preissue_instructions_tokened[i].at(2)){
                        noWaw = false;
                      }
                      // If the instruction before it IS a store word instruction
                      else if(preissue_instructions_tokened[w].at(1) == "SW"){
                        // For store words, the destination is what's in the 300(register), but the sourceString function works the same
                        string prev_instr_destination = memory_sourceString(preissue_instructions_tokened, w, destination);
                        cout << "Destination from this SW: " << destination;
                        if(destination == prev_instr_destination){
                          noWaw = false;
                        }
                      }
                    }
                  }
                }
                //---------------------------------------------------- END OF WAW -------------------------------------------------//


                cout << "memsource-> " << memSource;
                cout << "possible mem:" << possible_memory_instructions << "\n";
                //---------------------------------------------------- ADD TO PRE_ALU1 ---------------------------------------------//
                // Move the instruction from the preissue to prealu1 if the preALU1 is open, there is at least one possible memory instruction able to move, prealu1 size is less than 2, and there are no hazards
                if((preissue_instructions_tokened[i].at(1) == "SW" || preissue_instructions_tokened[i].at(1) == "LW") && possible_memory_instructions > 0 && prealu1_signal == true && pre_alu1.size() < 2 && noHazards == true && noWaw == true && noRaw == true){
                  if(i == 0){
                    if(preissue_instructions_tokened[i].at(1) == "SW"){
                      memory_registers_waited.insert(pair<int, vector<string>>(0, vector<string>()));
                      memory_registers_waited[0].push_back(preissue_instructions_tokened[i].at(2));
                      memoryReady = false;
                    }
                    else if(preissue_instructions_tokened[i].at(1) == "LW"){
                      memory_registers_waited.insert(pair<int, vector<string>>(1, vector<string>()));
                      memory_registers_waited[1].push_back(preissue_instructions_tokened[i].at(2));
                      memoryReady = false;
                    }
                  }
                  int pre1size = pre_alu1_instructions_tokened.size();
                  pre_alu1_instructions_tokened.insert(pair<int, vector<string>>(pre1size, vector<string>()));
                  pre_alu1_instructions_tokened[pre1size].push_back(to_string(cycle));

                  destination_registers_inpipeline.insert(pair<int, vector<string>>(cycle, vector<string>()));
                  destination_registers_inpipeline[cycle].push_back(preissue_instructions_tokened[i].at(2));
                  source_registers_inpipeline.insert(pair<int, vector<string>>(cycle, vector<string>()));
                  source_registers_inpipeline[cycle].push_back(source);

                  for(int j = 1; j < preissue_instructions_tokened[i].size(); j++){
                    pre_alu1_instructions_tokened[pre1size].push_back(preissue_instructions_tokened[i].at(j));
                  }
                  // Insert into the printer of pre_alu1
                  pre_alu1.insert(pair<int, string>(pre1size, preissue_instruction.at(i)));
                  // After moving the preissue instruction to the ALU, we must remove it and move up the next instruction.
                  int tti = preissue_instruction.size() - 1;
                  if(i == preissueSize - 1){
                    preissue_instruction.erase(i);
                    preissue_instructions_tokened.erase(i);
                    preissueSpot--;
                    preissueSize--;
                  }
                  else if(i == 0 && preissue_instruction.size() == 1){
                    preissue_instruction.erase(0);
                    preissue_instructions_tokened.erase(0);
                    preissueSize--;
                    preissueSpot--;
                  }
                  else{
                    for(int j = i; j < tti; j++){
                      preissue_instruction[j] = preissue_instruction.at(j+1);
                      preissue_instructions_tokened[j] = preissue_instructions_tokened[j+1];
                      preissue_instruction.erase(j+1);
                      preissue_instructions_tokened.erase(j+1);
                    }
                    preissueSize--;
                    preissueSpot--;
                  }
                  cout << preissue_instruction.size() << " ";
                  prealu1_signal = false;
                  cout << "water u doing";
                  // If this was the first instruction, reset and look at next ready instruction
                  if(prealu2_signal == true){
                    i = -1;
                  }
                }

                if(memoryReady == true){
                  cout << "\nMemory ready\n";
                }
                else if(memoryReady == false){
                  cout << "MemorynotReady\n";
                }
                if(noRaw == true){
                  cout <<"noooRawsHere\n";
                }
                else if(noRaw == false){
                  cout << "\nHAZARDDDDDD|\n";
                }
                if(noWaw == true){
                  cout << "sneky";
                }

                cout << "i" << i << "\n";
                //-------------------------------------------------------- ADD TO PRE_ALU2 ---------------------------------------//
                if(memoryReady == true &
                        noRaw == true &&
                       (preissue_instructions_tokened[i].at(1) != "SW" && preissue_instructions_tokened[i].at(1) != "LW") &&
                       (prealu2_signal == true && pre_alu2.size() < 2) && noWaw == true){

                  cout << "prealu2 test" << "\n";
                  int pre2size = pre_alu2_instructions_tokened.size();
                  pre_alu2_instructions_tokened.insert(pair<int, vector<string>>(pre2size, vector<string>()));
                  pre_alu2_instructions_tokened[pre2size].push_back(to_string(cycle));
                  // Starting at index 1 because we do not want to store the previous cycle entered time
                  for(int j = 1; j < preissue_instructions_tokened[i].size(); j++){
                    pre_alu2_instructions_tokened[pre2size].push_back(preissue_instructions_tokened[i].at(j));
                  }
                  // This stores the registres of the instruction into the map of registers in pipeline (to check for hazards);
                  // Does not add immediate values to the map
                  destination_registers_inpipeline.insert(pair<int, vector<string>>(cycle, vector<string>()));
                  destination_registers_inpipeline[cycle].push_back(preissue_instructions_tokened[i].at(2));
                  source_registers_inpipeline.insert(pair<int, vector<string>>(cycle, vector<string>()));
                  for(int k = 3; k < preissue_instructions_tokened[i].size(); k++){
                    if(preissue_instructions_tokened[i].at(k).at(0) == 'R'){
                      source_registers_inpipeline[cycle].push_back(preissue_instructions_tokened[i].at(k));
                    }
                  }
                  pre_alu2.insert(pair<int,string>(pre2size, preissue_instruction.at(i)));

                  int tti = preissueSize - 1; // tti means times to iterate.
                  cout << "PRESZ: " << preissueSize << "\n";

                  if(i == preissueSize - 1){
                    preissue_instruction.erase(i);
                    preissue_instructions_tokened.erase(i);
                    preissueSpot--;
                    preissueSize--;
                  }
                  else if(i == 0 && preissueSize == 1){
                    preissue_instruction.erase(0);
                    preissue_instructions_tokened.erase(0);
                    preissueSize--;
                    preissueSpot--;
                  }
                  else{
                    for(int j = i; j < tti; j++){
                      preissue_instruction[j] = preissue_instruction.at(j+1);
                      preissue_instructions_tokened[j] = preissue_instructions_tokened[j+1];
                      preissue_instruction.erase(j+1);
                      preissue_instructions_tokened.erase(j+1);
                    }
                    preissueSize--;
                    preissueSpot--;
                  }
                  prealu2_signal = false;

                  if(prealu1_signal == true){
                    i = -1;
                  }
                  cout << "huh??";
                }

              } // end of if instruction entry cycle time is less than current cycle
            } // end of preissue search

            //----------------------------------------------------------------------------------------------------------------//

            // Resetting signals for next cycle
            preissueSize = preissue_instruction.size();
            if(preissueSize < 4){
              full_stalled = false;
            }
            prealu1_signal = true;
            prealu2_signal = true;

            //----------------------------------------- PREALU1 -> PRE_MEM LOADING --------------------------------------------//
            // Moving instructions from the prealu1 to pre-mem (create SW functionality)
            for(int i = 0; i < pre_alu1_instructions_tokened.size(); i++){
              cout << "Instruction: " << pre_alu1.at(i) << " entered prealu1 at " << pre_alu1_instructions_tokened[i].at(0) << "\n";
              if(stoi(pre_alu1_instructions_tokened[i].at(0)) < cycle){
                int prememsize = pre_mem_queue_tokened.size();
                pre_mem_queue.insert(pair<int, string>(prememsize, pre_alu1.at(i)));
                pre_mem_queue_tokened.insert(pair<int, vector<string>>(prememsize, vector<string>()));
                pre_mem_queue_tokened[prememsize].push_back(to_string(cycle));
                for(int j = 1; j < pre_alu1_instructions_tokened[i].size(); j++){
                  pre_mem_queue_tokened[prememsize].push_back(pre_alu1_instructions_tokened[i].at(j));
                }
                cout << "Instruction: " << pre_mem_queue[prememsize] << " entered pre-mem at time : " << pre_mem_queue_tokened[prememsize].at(0) << "\n";
                if(i < pre_alu1_instructions_tokened.size() - 1){
                  pre_alu1[i] = pre_alu1[i+1];
                  pre_alu1_instructions_tokened[i] = pre_alu1_instructions_tokened.at(i+1);
                  pre_alu1.erase(i+1);
                  pre_alu1_instructions_tokened.erase(i+1);
                }
                else if(i == pre_alu1.size() - 1){
                  pre_alu1.erase(i);
                  pre_alu1_instructions_tokened.erase(i);
                }
              }
            }
            //-------------------------------------------------------------------------------------------------------------------//

            //------------------------------------------ PRE_MEM -> POST_MEM (LW ONLY) -------------------------------------------//
            // Moving instructions from the pre-mem to the post-mem queue (Load Word only)
            for(int i = 0; i < pre_mem_queue_tokened.size(); i++){
              if(stoi(pre_mem_queue_tokened[i].at(0)) < cycle){
                post_mem_queue_tokened.clear();
                // ONLY MOVE TO POST_MEM IF IT IS A LW INSTRUCTION. SW ONLY NEED TO GO TO PRE_MEM
                if(pre_mem_queue_tokened[i].at(1) == "LW"){
                  int postmemsize = post_mem_queue_tokened.size();
                  post_mem_queue.insert(pair<int, string>(postmemsize, pre_mem_queue.at(i)));
                  post_mem_queue_tokened.insert(pair<int, vector<string>>(postmemsize, vector<string>()));
                  post_mem_queue_tokened[postmemsize].push_back(to_string(cycle));
                  for(int j = 1; j < pre_mem_queue_tokened[i].size(); j++){
                    post_mem_queue_tokened[postmemsize].push_back(pre_mem_queue_tokened[0].at(j));
                  }
                  // Remove from previous pre_mem queue by moving up element after moving respective one to the post_mem queue
                  if(i < pre_mem_queue.size() - 1){
                    pre_mem_queue[i] = pre_mem_queue[i+1];
                    pre_mem_queue_tokened[i] = pre_mem_queue_tokened[i+1];
                    pre_mem_queue_tokened.erase(i+1);
                    pre_mem_queue.erase(i+1);
                  }
                  else if(i == pre_mem_queue.size() - 1){
                    pre_mem_queue.erase(i);
                    pre_mem_queue_tokened.erase(i);
                  }
                }
              }
            }
            //---------------------------------------------------------------------------------------------------------------------//

            //-------------------------------------------- PREALU2 -> POST_ALU ----------------------------------------------------//
            // Moving instructions from the prealu2 to post alu
            for(int i = 0; i < pre_alu2_instructions_tokened.size(); i++){
              if(stoi(pre_alu2_instructions_tokened[i].at(0)) < cycle && alu2stall != true){
                cout << "Instruction: " << pre_alu2.at(i) << " entered PostALU2 at " << cycle << "\n";
                int post2size = post_alu2_queue_tokened.size();
                post_alu2_queue.insert(pair<int, string>(post2size, pre_alu2.at(i)));
                post_alu2_queue_tokened.insert(pair<int, vector<string>>(post2size, vector<string>()));
                post_alu2_queue_tokened[post2size].push_back(to_string(cycle));

                for(int j = 1; j < pre_alu2_instructions_tokened[i].size(); j++){
                  post_alu2_queue_tokened[post2size].push_back(pre_alu2_instructions_tokened[i].at(j));
                }

                if(i < pre_alu2.size() - 1){
                  pre_alu2[i] = pre_alu2[i+1];
                  pre_alu2_instructions_tokened[i] = pre_alu2_instructions_tokened.at(i+1);
                  pre_alu2.erase(i+1);
                  pre_alu2_instructions_tokened.erase(i+1);
                }
                else if(i == pre_alu2.size() - 1){
                  pre_alu2.erase(i);
                  pre_alu2_instructions_tokened.erase(i);
                }
                alu2stall = true;
              }
            }
            //----------------------------------------------------------------------------------------------------------------------//

            //---------------------------------------------- POST_ALU -> WRITEBACK (WB) --------------------------------------------//
            // Moving instructions from the postALU to the WB
            for(int i = 0; i < post_alu2_queue_tokened.size(); i++){
              if(stoi(post_alu2_queue_tokened[i].at(0)) < cycle){
                cout << "cycle enter:: " << post_alu2_queue_tokened[i].at(0) << "\n";
                cout << "Instruction: ";
                int wb_size = write_back_tokened.size();
                write_back_tokened.insert(pair<int, vector<string>>(wb_size, vector<string>()));
                write_back_tokened[wb_size].push_back(to_string(cycle));
                for(int j = 1; j < post_alu2_queue_tokened[i].size(); j++){
                  cout << post_alu2_queue_tokened[i].at(j) << " ";
                  write_back_tokened[wb_size].push_back(post_alu2_queue_tokened[i].at(j));
                }

                if(i < post_alu2_queue_tokened.size() - 1){
                  cout << post_alu2_queue.size() << "\'";
                  post_alu2_queue_tokened[i] = post_alu2_queue_tokened.at(i+1);
                  post_alu2_queue_tokened.erase(i+1);
                }
                else if(i == post_alu2_queue_tokened.size() - 1){
                  post_alu2_queue_tokened.erase(i);
                  post_alu2_queue.clear();
                }
                cout << " entered WB at " << cycle << "\n";

              }
            }
            //-----------------------------------------------------------------------------------------------------------------------//


            print_pipelinedebugging_states(registers_waited_on,
              destination_registers_inpipeline,
              source_registers_inpipeline,
              memory_registers_waited,
              IFunit_instructions_tokened,
              preissue_instructions_tokened,
              pre_alu1_instructions_tokened,
              pre_mem_queue_tokened,
              post_mem_queue_tokened,
              pre_alu2_instructions_tokened,
              post_alu2_queue_tokened,
              write_back_tokened,
              cycle,
              preissueSize);


            // If there is something in the write back queue; signals it is time to write_back
            if(write_back_tokened.size() > 0){
              notWB = false;
            }
            // If the write back queue is empty?
            else{
              cout << "printing not writebacked simulation " << "\n";
              print_Simulation(mem_value, cycle, register_values, preissue_instruction, IFunit_instructions_tokened, IFunit, pre_alu1, pre_alu2, post_alu2_queue, pre_mem_queue, post_mem_queue);
              //write_back_tokened.clear();
            }
          } // End of the if(timeToDoBranch == false)

          // If a branch instruction is now executing, remove it from the executing queue and then allow it to go down
          // Only way (movetoExecute = true) happens is when a pipeline is unstalled and there are no registeres being waited on.
          if(movetoExecute == true){
            cout << "wbs: " << wbs_completed << "\n";
            // Reset the iterator to be the beginning of where it was stalled? Then go to next instruction at minimum incase taken.
            it = go_back_to_execute;
            for(int i = 0; i < wbs_completed -1; i++){
              it++;
            }
            //IFunit.erase(1);
            //IFunit_instructions_tokened.erase(1);
            movetoExecute = false;
            cout << " in here " << "\n";
            doneExecuting = true;
          }
        } // End of the not writeback loop?

      }   // End of if(iteration == 2) will go do stuff below, unless specified otherwise

      // This is the write-back stage???? |  Only gets to this part of the program if its finishing WB
      //                                  V

      //-------------- Related to the write-back portion of the pipeline --------------------//

      // If the pipeline is stalled waiting bc of branch or something, after an instruction reaches
      // the writeback stage, if it's destination register is in the sources for the branch instr, unstall it and move the instruction to the execute.
      if(branch_stalled == true){
        for(int i = 0; i < registers_waited_on[0].size(); i++){
          // If in the registers being waited on list, the write back destination matches, remove it from the waiting list
          if(registers_waited_on[0].at(i) == write_back_tokened[0].at(2)){
            // If the size is greater than 1, swap the that element with the last element and then pop it off.
            if(registers_waited_on[0].size() > 1){
              string temp = "";
              temp = registers_waited_on[0].at(i);
              registers_waited_on[0].at(i) = registers_waited_on[0].at(registers_waited_on[0].size() - 1);
              registers_waited_on[0].at(registers_waited_on[0].size() - 1) = temp;
              registers_waited_on[0].pop_back();
            }
            // If the size of registers being waited on is 1, just pop it off.
            else if(registers_waited_on[0].size() == 1){
              registers_waited_on[0].pop_back();
            }
          }
        }
        if(registers_waited_on[0].size() == 0){
          branch_stalled = false;
          branchInstruction = false;
          movetoExecute = true;
        }

        // If the pipeline is stalled waiting bc of branch or something, after an instruction reaches
        // the writeback stage, if it's destination register is in the sources for the branch instr, unstall it
        if(IFunit_instructions_tokened[0].at(1) == write_back_tokened[0].at(2)){
          branch_stalled = false;
          branchInstruction = false;
        }

      }
      // Clears the write_back_queue because it's being done here.
      for(int i = 0; i < write_back_tokened.size(); i++){
        write_back_tokened.erase(i);
      }

      // Flushing pipeline errors so they can be re-checked after write-back finish.
      destination_registers_inpipeline.clear();
      source_registers_inpipeline.clear();
      memory_registers_waited.clear(); // Clear the LW/SW memory registers waited on
      noHazards = true;
      noRaw = true;
      // // // // // // //

      if(iteration == 2){
        cout << "Writeback simulation : " << cycle << "\n";
      }

      //-------------------------------------------------------------------------------------//

      // Makes a substring out of the line to find out if the instruction is in category 1 or category 2
      // Start at position 0 - 2 length
      categorybits = it->second.substr(0,2);
      // Start at position 2 - 4 length
      opcode = it->second.substr(2,4);
      //cout << it->first << "\n";
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
              cout << "TAKEN";
            }
            else{
              cout << "NOT TAKEN";
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
            // do nothing?
            // cycle++;
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
          }
        }
      }
      // Start of the actual simulation. if(iteration ==1) is only for disassembly purposes.
      if(iteration == 2){
        // If it's a jump, I have to increase the cycle number so that it'll print out its jump state before changing the iterator to the one right before current iterator??
        if(taken == true && (opcode == "0000" || opcode == "0001" || opcode == "0010" || opcode == "0011" || opcode == "0100") && (categorybits == "01") ){
          cycle++;
        }

        print_Simulation(mem_value, cycle, register_values, preissue_instruction, IFunit_instructions_tokened, IFunit, pre_alu1, pre_alu2, post_alu2_queue, pre_mem_queue, post_mem_queue);
        cout << "End of Cycle:" << cycle << "\n\n";
        write_Simulation(simulation, mem_value, cycle, register_values, instruction_disassembly, it, instruction);
        if(jumpImm == true){
          branch_stalled = false;
          int instrnum = 0;
          it = go_back_to_execute;
          for(map<int, string>::iterator itr = mem_instruction.begin(); itr != mem_instruction.end(); itr++){
            if(itr->second == it->second){
              instrnum = itr->first;
              cout << "\n" << itr->first << "|" << itr->second;
            }
          }
          it--;
          instrnum = (instrnum - 256) / 4;
          cout << "\nAFTER" << it->first << "\n";
          cout << "INSTRUCTNUM : " << instrnum;
          ifNum = instrnum;
          IFunit_instructions_tokened[1].clear();
          IFunit[1].clear();
        }


        // Branches will come and print the WB simulation, so remove the IFunit[1]
        // before it wraps around and prints the non-WB simulation
        if(doneExecuting == true){
          IFunit.erase(1);
          IFunit_instructions_tokened.erase(1);
          doneExecuting = false;
        }

        // If jump, set the current iterator to be equal to the instruction right before where we want so that when it loops around it'll be the actual one we want
        // Jumper is set previously in the other if statement, but the iterator is turned into the jump addr here at the end.
        if(taken == true && (opcode == "0000" || opcode == "0001" || opcode == "0010" || opcode == "0011" || opcode == "0100") && (categorybits == "01") ){
          it = jumper;
        }

      } // End of if(iteration == 2)

    } // End of the for loop iterating through the mem_instruction map
    if(iteration == 1){
      write_Disassembly(instruction_disassembly, mem_value, mem_data);
      // Reset the cycle for the beginning of iteration 2 (simulation)
      cycle = 0;
      //cout << "Cycle at end of iteration 1: " << cycle << "\n";
    }

    // Only reaches here once the code is done due to the it = jumper line in iteration == 2 causing the program to shoot back up.

    // The iterations are first iteration for the disassembly to be finished and then second iteration for actual register work
    if(iteration == 1){
      iteration++;
    }
  }

  return 0;
}
