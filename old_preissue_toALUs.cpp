// // End of adding instructions to the preissue
// cout << "preissue_size: " << preissue_instructions_tokened.size() << " " << preissueSize << "\n";
// // For instructions in the preissue, if it was entered at a cycle before current cycle, move it to PREALUs if able
// for(int preissSize = 0; preissSize < preissueSize; preissSize++){
//   cout << preissSize << "<";
//   if(stoi(preissue_instructions_tokened[preissSize].at(0)) < cycle){
//     cout << "Instruction: " << preissue_instruction.at(preissSize) << " entered PreALU at: " << cycle << "\n";
//     prealu1_signal = true;
//     for(int i = 0; i < preissue_instructions_tokened[preissSize].size(); i++){
//       cout << "iiii: " << i << "\t";
//       cout <<  ">> " << preissue_instructions_tokened[preissSize].size() ;
//       cout << "size: " << preissueSize << "\n";
//       // Scan through the preissue. If able to load and the instruction is a load or store, take it out of the preissue and move it to the preALU1
//       if(preissue_instructions_tokened[i].at(1) == "SW" || preissue_instructions_tokened[i].at(1) == "LW" && prealu1_signal == true && pre_alu1.size() < 2){
//         cout << "buggin here?? ";
//         int pre1size = pre_alu1_instructions_tokened.size();
//         pre_alu1_instructions_tokened.insert(pair<int, vector<string>>(pre1size, vector<string>()));
//         // This makes the first vector element of the pre_alu1_instructions_tokened the cycle at which it is entered in.
//         pre_alu1_instructions_tokened[pre1size].push_back(to_string(cycle));
//         // For the instruction, add it to the respective prealu1 slot
//         for(int j = 1; j < preissue_instructions_tokened[i].size(); j++){
//           pre_alu1_instructions_tokened[pre1size].push_back(preissue_instructions_tokened[i].at(j));
//         }
//
//
//         // Insert into the printer of pre_alu1
//         pre_alu1.insert(pair<int, string>(pre1size, preissue_instruction.at(i)));
//         //cout << preissue_instruction.size() << " ";
//
//         // After moving the preissue instruction to the ALU, we must remove it and move up the next instruction.
//         if(i < preissue_instruction.size()){
//           preissue_instruction[i] = preissue_instruction.at(i+1);
//           preissue_instructions_tokened[i] = preissue_instructions_tokened[i+1];
//           preissue_instruction.erase(i+1);
//           preissue_instructions_tokened.erase(i+1);
//           preissueSize--;
//           preissueSpot--;
//         }
//         // If the moved preissue instruction is the last one, just remove it from the preissue list
//         else if(i == preissue_instruction.size() - 1){
//           preissue_instruction.erase(i);
//         }
//
//         cout << preissue_instruction.size() << " ";
//         prealu1_signal = false;
//       }
//       if(prealu2_signal == true && pre_alu2.size() < 2){
//         cout << "prealu2 test" << "\n";
//         int pre2size = pre_alu2_instructions_tokened.size();
//         pre_alu2_instructions_tokened.insert(pair<int, vector<string>>(pre2size, vector<string>()));
//         pre_alu2_instructions_tokened[pre2size].push_back(to_string(cycle));
//         // Starting at index 1 because we do not want to store the previous cycle entered time
//         for(int j = 1; j < preissue_instructions_tokened[i].size(); j++){
//           pre_alu2_instructions_tokened[pre2size].push_back(preissue_instructions_tokened[i].at(j));
//         }
//         pre_alu2.insert(pair<int,string>(pre2size, preissue_instruction.at(i)));
//         if(i < preissue_instruction.size() - 1){
//           //cout << preissue_instruction.size();
//           preissue_instruction[i] = preissue_instruction.at(i+1);
//           preissue_instructions_tokened[i] = preissue_instructions_tokened[i+1];
//           preissue_instruction.erase(i+1);
//           preissue_instructions_tokened.erase(i+1);
//           preissueSize--;
//           preissueSpot--;
//         }
//         // If the moved preissue instruction is the last one, just remove it from the preissue list
//         else if(i == preissue_instruction.size() - 1){
//           preissue_instruction.erase(i);
//           preissue_instructions_tokened.erase(i);
//           preissueSize--;
//           preissueSpot--;
//         }
//         prealu2_signal = false;
//       }
//
//
//     }
//   }
// } // End of the Preissue to PreAlu
