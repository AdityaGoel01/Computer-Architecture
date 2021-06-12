#include <iostream>
#include <cmath>
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>
#include <map>
#include <iomanip>
using namespace std;
typedef char byte;
int ROW_ACCESS_DELAY, COL_ACCESS_DELAY, ROW_BUFFER_UPDATES = 0;
vector<string> InCode;
// New global variables RegisterCount and MemoryLocationCount have been declared to help identify safe instructions in case on non-blocking memory access and Queuing DRAM requests for efficient Total Execution Time in the number of cycles 
multimap<string, int> RegisterCount; 
multimap<int, int> MemoryLocationCount;
vector<string> insertOrder = {"zero","at","v0","v1","a0","a1","a2","a3","t0","t1","t2","t3","t4","t5","t6","t7","s0","s1","s2","s3","s4","s5","s6","s7","t8","t9","k0","k1","gp","sp","s8","ra"};
    	map<string, int> RegisterMap={
		{"zero",0},
		{"at",0},
		{"v0",0},
		{"v1",0},
		{"a0",0},
		{"a1",0},
		{"a2",0},
		{"a3",0},
		{"t0",0},
		{"t1",0},
		{"t2",0},
		{"t3",0},
		{"t4",0},
		{"t5",0},
		{"t6",0},
		{"t7",0},
		{"s0",0},
		{"s1",0},
		{"s2",0},
		{"s3",0},
		{"s4",0},
		{"s5",0},
		{"s6",0},
		{"s7",0},
		{"t8",0},
		{"t9",0},
		{"k0",0},
		{"k1",0},
		{"gp",0},
		{"sp",0},
		{"s8",0},
		{"ra",0},
	};
int LineNumberHelper;
int CurrentLineNo = 1, TotalInstructions = 0, TotalLines = 0, clk_cycle = 1;
map<string, int> InstructionMap={
		{"add",0},
		{"sub",0},
		{"mul",0},
		{"beq",0},
		{"bne",0},
		{"slt",0},
		{"j",0},
		{"lw",0},
		{"sw",0},
		{"addi",0}
	};
vector<string> insertOrder2 = {"add","sub","mul","beq","bne","slt","j","lw","sw","addi"};
byte DRAM[1024][1024];
byte ROW_BUFFER [1024];
int ROW_CURRENT = -1;
// New global variables util and tmp have been declared to help identify the completion of delay period while accessing DRAM and to store the clock cycle number at which DRAM request was initiated respectively 
int util, tmp;
map<string, int> LabelToLine;
void rm_space(string &str)
{
	int32_t j=0;
	while(j<str.size() && (str[j]==' ' || str[j]=='\t'))
	{
		j++;
	}
	str=str.substr(j);
}

void RemoveComma(string &current_instruction)
{
	if(current_instruction.size()<2 || current_instruction[0]!=',') //check that first element exists and is a comma
	{
		cout<<"Error: Comma expected: "<<current_instruction<<endl;
		exit(1);
	}
	current_instruction=current_instruction.substr(1); //remove it
}

string findRegister(string &str, map<string, int> registers)
{

	if(str[0]!='$' || str.size()<2) //find '$' sign
	{
		cout<<"Error: Register expected here: "<<InCode[CurrentLineNo-1]<<endl;
		exit(1);
	}
	bool flag = true;
	str = str.substr(1); //remove '$' sign
	string registerID; //next two characters to match

	if(str.size()>=4){
		if(str.substr(0,4) == "zero"){
			registerID = "zero";
			str = str.substr(4);
			flag = false;}
	}
	if(flag){
		registerID = str.substr(0,2);
		auto itr = registers.find(registerID);
		if(itr == registers.end()){
			cout<<"Error: Invalid register: "<<InCode[CurrentLineNo-1]<<endl;
			exit(1);
		}
		str = str.substr(2);

	}
	return registerID;
	
}
string findLabel(string z)
{
	rm_space(z);
	bool f= false, d= false;
	string tmpStr="";
	for(int i=0;i<z.size();i++)
	{
		if(f==true && d==false && (z[i]==' '||z[i]=='\t')){
				d = true;
		}
		else if(f==true && d==true && !(z[i]==' '||z[i]=='\t')){
				cout<<"Error: Unexpected text after value: "<<InCode[CurrentLineNo-1]<<endl;
				exit(1);
		}
		else if(f==false && !(z[i]==' '||z[i]=='\t')){
				f=true;
				tmpStr+=z[i];	
		}
		else if(f==true && !(z[i]==' '||z[i]=='\t')){
			tmpStr+=z[i];
		}
	}
	return tmpStr;
}
void number_verify(string z)
{
	int start=0;
	char character = z[0];
	if (character=='-') start++;
	for(int i=start;i<z.size();i++)
	{
		if(z[i]<48 || z[i]>57)
		{
			cout<<"Error: Given value is not a valid number: "<<InCode[CurrentLineNo-1]<<endl;
			exit(1);
		}
	}
	if(character!='-'){ 
		if(z.size()>10 || (z.size()==10 && z>"2147483647")){
			cout<<"Error: Out of Range Integer: "<<InCode[CurrentLineNo-1]<<endl;
			exit(1);
		}
	}
	else if(z.size()>11 || (z.size()==11 && z>"-2147483648")){
		cout<<"Error: Out of Range Integer:"<<InCode[CurrentLineNo-1]<<endl;
		exit(0);
	}
}
void add(map<string, int> &RegisterMap, string str1, string str2, string str3)
{
	if(!RegisterCount.empty()){
		cout<< "Cycle Number: " << to_string(clk_cycle+1) <<endl<< "Instruction " << LineNumberHelper << " :" << InCode[LineNumberHelper-1] << "\n";
	}
	else{
		cout<< "Cycle Number: " << to_string(clk_cycle) <<endl<< "Instruction " << CurrentLineNo << " :" << InCode[CurrentLineNo-1] << "\n";
	}
	long long int answer = long(RegisterMap.find(str2)->second)+long(RegisterMap.find(str3)->second);
	if(answer<-2147483648 || answer>2147483647){
		cout<<"Addition Overflow"<<InCode[CurrentLineNo-1]<<endl;
		exit(1);
	}
	InstructionMap.find("add")->second++;
	RegisterMap[str1]=RegisterMap.find(str2)->second+RegisterMap.find(str3)->second;
	cout<< "Modified Register And New Value: $" <<str1<<" = "<<RegisterMap.find(str1)->second<<endl<<endl;
	CurrentLineNo++;
	clk_cycle++;
}
void sub(map<string, int> &RegisterMap, string str1, string str2, string str3)
{
	if(!RegisterCount.empty()){
		cout<< "Cycle Number: " << to_string(clk_cycle+1) <<endl<< "Instruction " << LineNumberHelper << " :" << InCode[LineNumberHelper-1] << "\n";
	}
	else{
		cout<< "Cycle Number: " << to_string(clk_cycle) <<endl<< "Instruction " << CurrentLineNo << " :" << InCode[CurrentLineNo-1] << "\n";
	}
	long long int answer = long(RegisterMap.find(str2)->second)-long(RegisterMap.find(str3)->second);
	if(answer<-2147483648 || answer>2147483647){
		cout<<"Subtraction Overflow"<<endl;
		exit(1);
	}
	InstructionMap.find("sub")->second++;
	RegisterMap[str1]=RegisterMap.find(str2)->second-RegisterMap.find(str3)->second;
	cout<< "Modified Register And New Value: $" <<str1<<" = "<<RegisterMap.find(str1)->second<<endl<<endl;
	CurrentLineNo++;
	clk_cycle++;
}
void mul(map<string, int> &RegisterMap, string str1, string str2, string str3)
{
	if(!RegisterCount.empty()){
		cout<< "Cycle Number: " << to_string(clk_cycle+1) <<endl<< "Instruction " << LineNumberHelper << " :" << InCode[LineNumberHelper-1] << "\n";
	}
	else{
		cout<< "Cycle Number: " << to_string(clk_cycle) <<endl<< "Instruction " << CurrentLineNo << " :" << InCode[CurrentLineNo-1] << "\n";
	}
	InstructionMap.find("mul")->second++;
	RegisterMap[str1]=RegisterMap.find(str2)->second*RegisterMap.find(str3)->second;
	cout<< "Modified Register And New Value: $" <<str1<<" = "<<RegisterMap.find(str1)->second<<endl<<endl;
	CurrentLineNo++;
	clk_cycle++;
}
void slt(map<string, int> &RegisterMap, string str1, string str2, string str3)
{
	if(!RegisterCount.empty()){
		cout<< "Cycle Number: " << to_string(clk_cycle+1) <<endl<< "Instruction " << LineNumberHelper << " :" << InCode[LineNumberHelper-1] << "\n";
	}
	else{
		cout<< "Cycle Number: " << to_string(clk_cycle) <<endl<< "Instruction " << CurrentLineNo << " :" << InCode[CurrentLineNo-1] << "\n";
	}
	InstructionMap.find("slt")->second++;
	if(RegisterMap.find(str2)->second<RegisterMap.find(str3)->second){
		RegisterMap[str1]=1;
		cout<< "Modified Register And New Value: $" <<str1<<" = "<<RegisterMap.find(str1)->second<<endl<<endl;
		CurrentLineNo++;
		clk_cycle++;
	}
	else{
		RegisterMap[str1]=0;
		cout<< "Modified Register And New Value: $" <<str1<<" = "<<RegisterMap.find(str1)->second<<endl<<endl;
		CurrentLineNo++;
clk_cycle++;
	}
}
void beq(map<string, int> &RegisterMap, string str1, string str2, string str3)
{	
	cout<< "Cycle Number: " << to_string(clk_cycle) <<endl<< "Instruction " << CurrentLineNo << " :" << InCode[CurrentLineNo-1] << "\n\n";
	map<string, int>::iterator it = LabelToLine.find(str3);
	if(it == LabelToLine.end()){
		cout<<"Label Not Found: "<< InCode[CurrentLineNo-1]<<endl;
		exit(1);
	}
	InstructionMap.find("beq")->second++;
	if(RegisterMap.find(str1)->second==RegisterMap.find(str2)->second){
		CurrentLineNo = LabelToLine.find(str3)->second;
		clk_cycle++;
	}
	else{
		CurrentLineNo++;
		clk_cycle++;
	}
}
void bne(map<string, int> &RegisterMap, string str1, string str2, string str3)
{
	cout<< "Cycle Number: " << to_string(clk_cycle) <<endl<< "Instruction " << CurrentLineNo << " :" << InCode[CurrentLineNo-1] << "\n\n";
	map<string, int>::iterator it = LabelToLine.find(str3);
	if(it == LabelToLine.end()){
		cout<<"Label Not Found: "<< InCode[CurrentLineNo-1]<<endl;
		exit(1);
	}
	InstructionMap.find("bne")->second++;
	if(RegisterMap.find(str1)->second!=RegisterMap.find(str2)->second){
		CurrentLineNo = LabelToLine.find(str3)->second;
		clk_cycle++;
	}
	else{
		CurrentLineNo++;
		clk_cycle++;
	}
}
void j(map<string, int> &RegisterMap, string str1)
{
	cout<< "Cycle Number: " << to_string(clk_cycle) <<endl<< "Instruction " << CurrentLineNo << " :" << InCode[CurrentLineNo-1] << "\n\n";
	map<string, int>::iterator it = LabelToLine.find(str1);
	if(it == LabelToLine.end()){
		cout<<"Label Not Found: "<< InCode[CurrentLineNo-1]<<endl;
		exit(1);
	}
	InstructionMap.find("j")->second++;
		CurrentLineNo = LabelToLine.find(str1)->second;
		clk_cycle++;
}
void addi(map<string, int> &RegisterMap, string str1, string str2, int str3)
{
	if(!RegisterCount.empty()){
		cout<< "Cycle Number: " << to_string(clk_cycle+1) <<endl<< "Instruction " << LineNumberHelper << " :" << InCode[LineNumberHelper-1] << "\n";
	}
	else{
		cout<< "Cycle Number: " << to_string(clk_cycle) <<endl<< "Instruction " << CurrentLineNo << " :" << InCode[CurrentLineNo-1] << "\n";
	}
	long long int answer = long(RegisterMap.find(str2)->second)+long(str3);
	if(answer<-2147483648 || answer>2147483647){
		cout<<"Addition Overflow: "<< InCode[CurrentLineNo-1]<<endl;
		exit(1);
	}
	InstructionMap.find("addi")->second++;
	RegisterMap[str1]=RegisterMap.find(str2)->second+str3;
	cout<< "Modified Register And New Value: $" <<str1<<" = "<<RegisterMap.find(str1)->second<<endl<<endl;
	CurrentLineNo++;
	clk_cycle++;
}
bool instruction_executer(map<string, int> &registers, int line_number, vector<string> lines, int num,string str);
void displayRegisters (map<string, int> &RegisterMap,vector<string> insertOrder);
// lw function has been changed with respect to code for Minor
void lw(map<string, int> &RegisterMap,string str1, int str2,string str3){
	string tmpStr = InCode[CurrentLineNo-1];
	if(str2 > 1048576 || str2 < 0 || str2%4!=0){
		cout<<"Invalid Memory address: "<< InCode[CurrentLineNo-1]<<"\n";
		exit(1);
	}
	int temp = str2 + 1;
	int k = -1;
	while(temp>0){
		temp= temp/1024;
		k++;
	}
	int ROW_NUMBER = k;
	int COL_NUMBER = str2 - (k*1024) + 1;
	tmp = clk_cycle;
	if(!MemoryLocationCount.empty() && ROW_CURRENT != ROW_NUMBER){
			RegisterCount.erase(RegisterCount.lower_bound(str3));
			RegisterCount.erase(RegisterCount.lower_bound(str1));
			cout<< "Cycle Number: " << to_string(clk_cycle) <<endl<< "Instruction "<< CurrentLineNo << " :" << InCode[CurrentLineNo-1] << "\n";
			cout<< "DRAM Request Issued" <<endl<<endl;
			if (ROW_CURRENT != -1){
				for(int i=0; i<1024; i++){
					DRAM[ROW_CURRENT][i] = ROW_BUFFER[i];
				}
			}
			int temp = ROW_CURRENT;
			ROW_CURRENT = ROW_NUMBER;
			if(temp != ROW_NUMBER){
				for(int i=0; i<1024; i++){
					ROW_BUFFER[i] = DRAM[ROW_NUMBER][i];
				}
				ROW_BUFFER_UPDATES++;
			}
			InstructionMap.find("lw")->second++;
			RegisterMap[str1] = ROW_BUFFER[COL_NUMBER];
			int ans = ROW_BUFFER[COL_NUMBER];
			CurrentLineNo++;
			int str_temp = CurrentLineNo;
			if(temp==-1){
				util = ROW_ACCESS_DELAY+COL_ACCESS_DELAY -1;
				int util2= clk_cycle+ROW_ACCESS_DELAY+COL_ACCESS_DELAY;
				while(LineNumberHelper<=TotalInstructions && util-- && instruction_executer(RegisterMap, LineNumberHelper-1, InCode, -2, "")) {
					tmp++;
					LineNumberHelper++;
				}
				clk_cycle=util2;
				}
			else if(temp == ROW_NUMBER){
				util = COL_ACCESS_DELAY -1;
				int util2= clk_cycle+COL_ACCESS_DELAY;
				while(LineNumberHelper<=TotalInstructions && util-- && instruction_executer(RegisterMap, LineNumberHelper-1, InCode, -2, "")) {
					tmp++;
					LineNumberHelper++;
				}
				clk_cycle=util2;
			}
			else{
				util = (2*ROW_ACCESS_DELAY)+COL_ACCESS_DELAY -1;
				int util2= clk_cycle+(2*ROW_ACCESS_DELAY)+COL_ACCESS_DELAY;
				while(LineNumberHelper<=TotalInstructions && util-- && instruction_executer(RegisterMap, LineNumberHelper-1, InCode, -2, "")) {
					tmp++;
					LineNumberHelper++;
				}
				clk_cycle=util2;
				}
			cout<< "Cycle Number: " <<to_string(tmp+1)<<"-" << to_string(clk_cycle) <<endl<< "Instruction " << str_temp -1 << " :" << tmpStr << "\n";
			cout<< "Modified Register And New Value: $" <<str1<<" = "<<ans<<endl<<endl;
			clk_cycle++;		
			}
	else if(!MemoryLocationCount.empty()){
			RegisterCount.erase(RegisterCount.lower_bound(str3));
			RegisterCount.erase(RegisterCount.lower_bound(str1));
			cout<< "Cycle Number: " << to_string(clk_cycle) <<endl<< "Instruction "<< CurrentLineNo  << " :" << InCode[CurrentLineNo-1] << "\n";
			cout<< "DRAM Request Issued" <<endl<<endl;
			InstructionMap.find("lw")->second++;
		RegisterMap[str1] = ROW_BUFFER[COL_NUMBER];
		int ans = ROW_BUFFER[COL_NUMBER];
		CurrentLineNo++;
		int str_temp = CurrentLineNo;
		util = COL_ACCESS_DELAY -1;
		int util2= clk_cycle+COL_ACCESS_DELAY;
		while(LineNumberHelper<=TotalInstructions && instruction_executer(RegisterMap, LineNumberHelper-1, InCode, -2, "") && util--) {
					tmp++;
					LineNumberHelper++;
				}
		clk_cycle=util2;
		cout<< "Cycle Number: " <<to_string(tmp+1)<<"-" << to_string(clk_cycle) <<endl<< "Instruction " << str_temp -1 << " :" << tmpStr << "\n";
		cout<< "Modified Register And New Value: $" <<str1<<" = "<<ans<<endl<<endl;
		clk_cycle++;
		}
	else{
		RegisterCount.insert({str1,CurrentLineNo});
		MemoryLocationCount.insert({str2,CurrentLineNo});
		RegisterCount.insert({str3,LineNumberHelper});
		int temp = ROW_CURRENT;
		clk_cycle++;
		CurrentLineNo++;
		while(true){
			if(CurrentLineNo>TotalInstructions||!instruction_executer(RegisterMap, CurrentLineNo-1, InCode, str2, str1)) break;
		}
		LineNumberHelper = CurrentLineNo--;
		clk_cycle = tmp;
		auto it = MemoryLocationCount.begin();
		
		it = MemoryLocationCount.begin();
		while (!MemoryLocationCount.empty()) {
			it = MemoryLocationCount.begin();
		while ( it != MemoryLocationCount.end()) {
			int temp = it->first + 1;
			int k = -1;
			while(temp>0){
				temp= temp/1024;
				k++;
			}
			int ROW_NUMBER = k;
			if(ROW_NUMBER == ROW_CURRENT){
				CurrentLineNo = it->second;
				instruction_executer(RegisterMap,it->second -1, InCode,-1,"");
				if(it->second == MemoryLocationCount.rbegin()->second){
					MemoryLocationCount.erase(it->first);
					break;
				}
				MemoryLocationCount.erase(it++);
			}	
			else{
				it++;
			}
		}
			if(!MemoryLocationCount.empty()){
				it = MemoryLocationCount.begin();
				CurrentLineNo = it->second;
				instruction_executer(RegisterMap,it->second -1, InCode,-1,"");
				MemoryLocationCount.erase(it);
			}
		}
		CurrentLineNo = LineNumberHelper;
		RegisterCount.clear();
		MemoryLocationCount.clear();
		}
}
// sw function has been changed with respect to code for Minor
void sw(map<string, int> &RegisterMap,string str1, int str2,string str3){
	string tmpStr = InCode[CurrentLineNo-1];
	if(str2 > 1048576 || str2 < 0 || str2%4!=0){
		cout<<"Invalid Memory address: "<< InCode[CurrentLineNo-1]<<"\n";
		exit(1);
	}
	int temp = str2 + 1;
	int k = -1;
	while(temp>0){
		temp= temp/1024;
		k++;
	}
	int ROW_NUMBER = k;
	int COL_NUMBER = str2 - (k*1024) + 1;
	tmp = clk_cycle;
	if(!MemoryLocationCount.empty() && ROW_CURRENT != ROW_NUMBER){
			RegisterCount.erase(RegisterCount.lower_bound(str3));
			RegisterCount.erase(RegisterCount.lower_bound(str1));
			cout<< "Cycle Number: " << to_string(clk_cycle) <<endl<< "Instruction "<< CurrentLineNo  << " :" << InCode[CurrentLineNo-1] << "\n";
			cout<< "DRAM Request Issued" <<endl<<endl;
			if (ROW_CURRENT != -1){
				for(int i=0; i<1024; i++){
					DRAM[ROW_CURRENT][i] = ROW_BUFFER[i];
				}
			}
			int temp = ROW_CURRENT;
			ROW_CURRENT = ROW_NUMBER;
			if (temp != ROW_NUMBER){
				for(int i=0; i<1024; i++){
					ROW_BUFFER[i] = DRAM[ROW_NUMBER][i];
				}
				ROW_BUFFER_UPDATES++;
			}
			InstructionMap.find("sw")->second++;
			ROW_BUFFER[COL_NUMBER]= RegisterMap.find(str1)->second;
			int ans = ROW_BUFFER[COL_NUMBER];
			ROW_BUFFER_UPDATES++;
			CurrentLineNo++;
			int str_temp = CurrentLineNo;
			if(temp==-1){
				util = ROW_ACCESS_DELAY+COL_ACCESS_DELAY -1;
				int util2= clk_cycle+ROW_ACCESS_DELAY+COL_ACCESS_DELAY;
				while(LineNumberHelper<=TotalInstructions && util-- && instruction_executer(RegisterMap, LineNumberHelper-1, InCode, -2, "")) {
					tmp++;
					LineNumberHelper++;
				}
				clk_cycle=util2;
				}
			else if (temp == ROW_NUMBER){
				util = COL_ACCESS_DELAY -1;
				int util2= clk_cycle+COL_ACCESS_DELAY;
				while(LineNumberHelper<=TotalInstructions && util-- && instruction_executer(RegisterMap, LineNumberHelper-1, InCode, -2, "")) {
					tmp++;
					LineNumberHelper++;
				}
				clk_cycle=util2;
			}
			else{
				util = (2*ROW_ACCESS_DELAY)+COL_ACCESS_DELAY -1;
				int util2= clk_cycle+(2*ROW_ACCESS_DELAY)+COL_ACCESS_DELAY;
				while(LineNumberHelper<=TotalInstructions && util-- && instruction_executer(RegisterMap, LineNumberHelper-1, InCode, -2, "")) {
					tmp++;
					LineNumberHelper++;
				}
				clk_cycle=util2;
				}
			cout<< "Cycle Number: " <<to_string(tmp+1)<<"-"<<to_string(clk_cycle) <<endl<< "Instruction " << str_temp -1 << " :" << tmpStr << "\n";
			cout<< "Modified Memory Location And New Value: " <<to_string(str2)<<"-"<<to_string(str2+3)<<": "<<ans<<endl<<endl;
			clk_cycle++;		
			}
	else if (!MemoryLocationCount.empty()){
			RegisterCount.erase(RegisterCount.lower_bound(str3));
			RegisterCount.erase(RegisterCount.lower_bound(str1));
			cout<< "Cycle Number: " << to_string(clk_cycle) <<endl<< "Instruction "<< CurrentLineNo  << " :" << InCode[CurrentLineNo-1] << "\n";
			cout<< "DRAM Request Issued" <<endl<<endl;
			InstructionMap.find("sw")->second++;
			ROW_BUFFER[COL_NUMBER]= RegisterMap.find(str1)->second;
			int ans = ROW_BUFFER[COL_NUMBER];
			ROW_BUFFER_UPDATES++;
			CurrentLineNo++;
			int str_temp = CurrentLineNo;
			util = COL_ACCESS_DELAY -1;
			int util2= clk_cycle+COL_ACCESS_DELAY;
			while(LineNumberHelper<=TotalInstructions && instruction_executer(RegisterMap, LineNumberHelper-1, InCode, -2, "")&& util-- ) {
					tmp++;
					LineNumberHelper++;
				}
			clk_cycle=util2;
			cout<< "Cycle Number: " <<to_string(tmp+1)<<"-"<<to_string(clk_cycle) <<endl<< "Instruction " << str_temp -1 << " :" << tmpStr << "\n";
			cout<< "Modified Memory Location And New Value: " <<to_string(str2)<<"-"<<to_string(str2+3)<<": "<<ans<<endl<<endl;
			clk_cycle++;
	}
	else {
		RegisterCount.insert({str1,CurrentLineNo});
		MemoryLocationCount.insert({str2,CurrentLineNo});
		RegisterCount.insert({str3,LineNumberHelper});
		int temp = ROW_CURRENT;
		clk_cycle++;
		CurrentLineNo++;
		while(true){
			if(CurrentLineNo>TotalInstructions||!instruction_executer(RegisterMap, CurrentLineNo-1, InCode, str2, str1)) break;
		}
		LineNumberHelper = CurrentLineNo--;
		clk_cycle = tmp;
		auto it = MemoryLocationCount.begin();
		
		it = MemoryLocationCount.begin();
		while (!MemoryLocationCount.empty()) {
			it = MemoryLocationCount.begin();
		while ( it != MemoryLocationCount.end()) {
			int temp = it->first + 1;
			int k = -1;
			while(temp>0){
				temp= temp/1024;
				k++;
			}
			int ROW_NUMBER = k;
			if(ROW_NUMBER == ROW_CURRENT){
				CurrentLineNo = it->second;
				instruction_executer(RegisterMap,it->second -1, InCode,-1,"");
				if(it->second == MemoryLocationCount.rbegin()->second){
					MemoryLocationCount.erase(it->first);
					break;
				}
				MemoryLocationCount.erase(it++);
			}	
			else{
				it++;
			}
		}
			if(!MemoryLocationCount.empty()){
				it = MemoryLocationCount.begin();
				CurrentLineNo = it->second;
				instruction_executer(RegisterMap,it->second -1, InCode,-1,"");
				MemoryLocationCount.erase(it);
			}
		}
		CurrentLineNo = LineNumberHelper;
		RegisterCount.clear();
		MemoryLocationCount.clear();
		}
}
void displayInstructions (map<string, int> &InstructionMap,vector<string> insertOrder){
	// Iterate over the map using iterator
	cout<<"Instruction Name\tInstruction Count"<<endl;
	// Print the final results.
	for (int i = 0; i < insertOrder.size(); ++i)
	{
    		const string &s = insertOrder[i];
    		cout<<s<<"\t\t\t"<<to_string(InstructionMap[s])<<'\n';
	}
	cout<<'\n';
}
void displayRegisters (map<string, int> &RegisterMap,vector<string> insertOrder){
	// Iterate over the map using iterator
	cout<<"Register Name\tRegister Value"<<endl;
	// Print the final results.
	for (int i = 0; i < insertOrder.size(); ++i)
	{
    		const string &s = insertOrder[i];
    		cout<<s<<"\t\t"<<"0x"<<setfill('0')<<setw(sizeof(int)*2)<<hex<<RegisterMap[s]<<'\n';
	}
	cout<<'\n';
}
bool instruction_executer(map<string, int> &registers, int line_number, vector<string> lines, int num=-1,string str=""){
	string l = lines.at(line_number);
	
	//remove comments
	if(l.find("#")!=-1) //remove comments
	{
		l = l.substr(0,l.find("#"));
	}
	
	//remove spaces
	rm_space(l);
	if(l=="") //if there was only comment
	{
		return true;
	}

	int32_t no_of_register=0,k=0;

	if(l.find(":")!=-1) //if label encountered
	{
		//remove comments
		if(l.find("#")!=-1) //remove comments
		{
			l = l.substr(0,l.find("#"));
		}
		//remove spaces
		rm_space(l);
		l = l.substr(0,l.find(":"));
		//remove spaces
		rm_space(l);
		if(str=="" && num==-1){
			cout<< "Label Encountered: " <<InCode[CurrentLineNo-1] << "\n\n";
		}
		CurrentLineNo++;
		return false;
	}
	if((l.size()<4&&l[0]!='j') || (l[0] == 'j' && l.size()<3)) //no valid instruction is this small
	{
		if(num!=-1 || str!=""){
			return false;
		}
		cout<<"Error: Invalid operation: "<< InCode[CurrentLineNo-1]<<endl;
		exit(1);
	}
	for(k=0;k<4;k++) //find length of operation
	{
		if(l[k]==' ' || l[k]=='\t')
		{
			break;
		}
	}
	string operation = l.substr(0,k); //cut the operation out

	if(l.size()>0 && k<l.size()-1)
	{
		l = l.substr(k+1);
	}

	if(operation != "add" && operation != "sub" && operation != "mul" && operation != "slt" && operation != "addi" && operation != "lw" && operation != "sw" && operation != "beq" && operation != "bne" && operation != "j") //if not valid
	{
		if(num!=-1 || str!=""){
			return false;
		}
		cout<<"Error: Invalid operation: "<< InCode[CurrentLineNo-1]<<endl;
		exit(1);
	}

	int32_t foundOp=0; //whether valid operation or not
	string register1, register2, register3;
	int imme_value;
	string address;
	
	if(operation == "add" || operation == "sub" || operation == "mul" || operation == "slt"){

		rm_space(l);
		register1 = findRegister(l, registers);
		rm_space(l);
		RemoveComma(l);

		rm_space(l);
		register2 = findRegister(l, registers);
		rm_space(l);
		RemoveComma(l);

		rm_space(l);
		register3 = findRegister(l, registers);
		rm_space(l);
		
		no_of_register = 3;

		if(l!="") //if something more found
		{
			cout<<"Error: More arguments provided than required: "<< InCode[CurrentLineNo-1]<<endl;
			exit(1);
		}
		// Changes for execution of safe instructions in case of non-blocking memory access alongside row access and column access delay involved in reordered DRAM requests 
		if((num== -2) && !(RegisterCount.find(register1)!=RegisterCount.end() || RegisterCount.find(register2)!=RegisterCount.end()) || RegisterCount.find(register3)!=RegisterCount.end()){
			if(operation == "add"){
				add(registers,register1,register2,register3);
				return true;
			}else if(operation == "sub"){
				sub(registers,register1,register2,register3);
				return true;
			}else if(operation == "mul"){
				mul(registers,register1,register2,register3);
				return true;
			}else{
				slt(registers,register1,register2,register3);
				return true;
			}
		}
		if((num!=-1 || str!="")){
			return false;
		}
		else if(operation == "add"){
			add(registers,register1,register2,register3);
		}else if(operation == "sub"){
			sub(registers,register1,register2,register3);
		}else if(operation == "mul"){
			mul(registers,register1,register2,register3);
		}else{
			slt(registers,register1,register2,register3);
		}
	}else if(operation == "addi"){

		rm_space(l);
		register1 = findRegister(l, registers);
		rm_space(l);
		RemoveComma(l);

		rm_space(l);
		register2 = findRegister(l, registers);
		rm_space(l);
		RemoveComma(l);

		rm_space(l);
		// Changes for execution of safe instructions in case of non-blocking memory access alongside row access and column access delay involved in reordered DRAM requests 
		if((num== -2) && !(RegisterCount.find(register1)!=RegisterCount.end() || RegisterCount.find(register2)!=RegisterCount.end())){
			string tempString = findLabel(l); //find third argument, a number
			number_verify(tempString); //check validity
			imme_value=stoi(tempString); //convert and store
			no_of_register = 2;
			addi(registers,register1,register2,imme_value);
			return true;
		}
		if((num!=-1 || str!="")){
			return false;
		}
		string tempString = findLabel(l); //find third argument, a number
		number_verify(tempString); //check validity
		imme_value=stoi(tempString); //convert and store
		no_of_register = 2;
		addi(registers,register1,register2,imme_value);

	}else if(operation == "lw" || operation == "sw"){

		string tempString="";
		rm_space(l);
		register1 = findRegister(l, registers); //find source/destination register
		rm_space(l);
		RemoveComma(l); //find comma, ignoring extra spaces
		rm_space(l);
		int i, num1;
		string tmpStr1;
		for(i=0; i<l.size(); i++){
			if(l[i]=='('){
				break;
			}
		}
		if(i>=l.size()){
			cout<<"Invalid Memory Address: "<< InCode[CurrentLineNo-1]<<endl;
			exit(1);
		}
		tmpStr1= l.substr(0,i);
		rm_space(tmpStr1);
		number_verify(tmpStr1);
		num1 = stoi(tmpStr1);
		while(l[l.size()-1]=='\t'||l[l.size()-1]==' '){
		 	l = l.substr(0,l.size()-1);
		}
		if(l[l.size()-1]!=')'){
			cout<<"Invalid Memory Address: "<< InCode[CurrentLineNo-1]<<endl;
			exit(1);
		}
		l = l.substr(i+1,l.size()-2);
		string registerFind = findRegister(l, registers);
		int address = registers.find(registerFind)->second + num1;
		
		tempString = findLabel(l);
		no_of_register = 1;
		// Changes for execution of safe instructions in case of non-blocking memory access alongside row access and column access delay involved in reordered DRAM requests 
		if(str!=""||num!=-1){
			if((RegisterCount.find(register1)!=RegisterCount.end()) ){
				return false;
			}
			else if(operation == "lw"){
				if(num==-2){
					RegisterCount.insert({register1,LineNumberHelper});
					RegisterCount.insert({registerFind,LineNumberHelper});
					MemoryLocationCount.insert({address,LineNumberHelper});
					CurrentLineNo++;
					util++;
					tmp--;
					return true;
				}
				else{
					RegisterCount.insert({register1,CurrentLineNo});
					RegisterCount.insert({registerFind,LineNumberHelper});
					MemoryLocationCount.insert({address,CurrentLineNo});
					CurrentLineNo++;
					return true;
				}
			}else{
				if(num==-2){
					RegisterCount.insert({register1,LineNumberHelper});
					RegisterCount.insert({registerFind,LineNumberHelper});
					MemoryLocationCount.insert({address,LineNumberHelper});
					CurrentLineNo++;
					util++;
					tmp--;
					return true;
				}
				else{
					RegisterCount.insert({register1,CurrentLineNo});
					RegisterCount.insert({registerFind,LineNumberHelper});
					MemoryLocationCount.insert({address,CurrentLineNo});
					CurrentLineNo++;
					LineNumberHelper = CurrentLineNo;
					return true;
				}
			}
		}
		if(operation == "lw"){
			lw(registers,register1,address,registerFind);
		}else{
			sw(registers,register1,address,registerFind);
		}
	}else if(operation == "beq" || operation == "bne"){
		rm_space(l);
		register1 = findRegister(l, registers);
		rm_space(l);
		RemoveComma(l);

		rm_space(l);
		register2 = findRegister(l, registers);
		rm_space(l);
		RemoveComma(l);

		rm_space(l);

		string tempString="";
		tempString = findLabel(l);
		address = tempString;
		no_of_register = 2;
		if(str!=""||num!=-1){
			return false;
		}
		if(operation == "beq"){
			beq(registers,register1,register2,address);
		}else{
			bne(registers,register1,register2,address);
		}
	}else if(operation == "j"){
		
		string tempString="";
		rm_space(l);
		tempString = findLabel(l);
		address = tempString;
		no_of_register = 1;
		if(str!=""||num!=-1){
			return false;
		}
		j(registers, address);
	}
	return true;

}
bool check_comment(string x){
	string y;
	if(x.find("#")!=-1)
	{
		y = x.substr(0,x.find("#"));
	}else{
		y =x;
	}
	
	//remove spaces
	rm_space(y);
	if(y=="") //if there was only comment
	{
		return true;
	}
	return false;

}
int main(int argc, char* argv[]){
	string temp1= argv[1], temp2= argv[2];
	for(int i=0; i<temp1.size(); i++){
		if(!isdigit(temp1[i])){
			cout<<"Error: ROW_ACCESS_DELAY not specified correctly"<<endl;
			exit(1);
		}
	}
	for(int i=0; i<temp2.size(); i++){
		if(!isdigit(temp2[i])){
			cout<<"Error: COL_ACCESS_DELAY not specified correctly"<<endl;
			exit(1);
		}
	}
	ROW_ACCESS_DELAY=stoi(argv[1]);
	COL_ACCESS_DELAY=stoi(argv[2]);
	if(ROW_ACCESS_DELAY<0){
		cout<<"Error: ROW_ACCESS_DELAY not specified correctly"<<endl;
		exit(1);
	}
	if(COL_ACCESS_DELAY<0){
		cout<<"Error: COL_ACCESS_DELAY not specified correctly"<<endl;
		exit(1);
	}
	ifstream InputStream;
	InputStream.open(argv[3],ios::in);
	if(!InputStream)
	{
		cout<<"Error: File does not exist or could not be opened"<<endl;
		exit(1);
	}
	string tmpStr;
	while(getline(InputStream,tmpStr))
	{
		TotalLines++;
		if(!check_comment(tmpStr)){
			if(tmpStr.find(":")!=-1 ){
			string tmpStr1 = tmpStr;
			//remove comments
			if(tmpStr1.find("#")!=-1) //remove comments
			{
				tmpStr1 = tmpStr1.substr(0,tmpStr1.find("#"));
			}
			//remove spaces
			rm_space(tmpStr1);
			tmpStr1 = tmpStr1.substr(0,tmpStr1.find(":"));
			//remove spaces
			rm_space(tmpStr1);
			while(tmpStr1[tmpStr1.size()-1]=='\t'||tmpStr1[tmpStr1.size()-1]==' '){
		 			tmpStr1 = tmpStr1.substr(0,tmpStr1.size()-1);
				}
			LabelToLine[tmpStr1] = TotalInstructions+1;}
			else{
				InCode.push_back(tmpStr);
				TotalInstructions++;
				}
			}
		if(TotalInstructions>131072){
				cout<<"Number of Instructions exceeded the maximum allowed values\n";
				exit(1);
			}
	}
	InputStream.close();
	while(CurrentLineNo<=TotalInstructions){
		if(!check_comment(InCode[CurrentLineNo-1])){
			if(InCode[CurrentLineNo-1].find(":")!=-1 ){
				string tmpStr1 = InCode[CurrentLineNo-1];
				//remove comments
				if(tmpStr1.find("#")!=-1) //remove comments
				{
					tmpStr1 = tmpStr1.substr(0,tmpStr1.find("#"));
				}
				//remove spaces
				rm_space(tmpStr1);
				tmpStr1 = tmpStr1.substr(0,tmpStr1.find(":"));
				//remove spaces
				rm_space(tmpStr1);
				while(tmpStr1[tmpStr1.size()-1]=='\t'||tmpStr1[tmpStr1.size()-1]==' '){
		 			tmpStr1 = tmpStr1.substr(0,tmpStr1.size()-1);
				}
				cout<< "Label Encountered: " <<InCode[CurrentLineNo-1] << "\n\n";
				CurrentLineNo++;
			}
			else{
			instruction_executer(RegisterMap, CurrentLineNo-1, InCode);
			}
		}else{
			CurrentLineNo++;
		}
	}
	cout << "Execution Successful! " << "\n";
	cout << "Total Execution Time In Clock Cycles: " << to_string(clk_cycle-1) << "\n";
	cout << "Number Of Row Buffer Updates: " << to_string(ROW_BUFFER_UPDATES) << "\n";
}
