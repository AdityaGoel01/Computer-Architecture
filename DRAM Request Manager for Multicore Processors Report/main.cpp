#include <bits/stdc++.h>
using namespace std;
typedef char byte;
byte DRAM[1024][1024];
byte ROW_BUFFER [1024];
int ROW_CURRENT = -1;
int ROW_BUFFER_UPDATES_HELPER = 0;
int instruction_executer(int num, string str);
vector<multimap<string, int> > RegisterCount;
int ROW_BUFFER_UPDATES = 0;
int indexH = 0;
class core{
	public:
	vector<string> Instructions;
	map<string, int> LabelToLine;
	int TotalInstructions;
	int NoMemoryRequests = 0 ;
	int clkCycle = 0;
	int NoInstructions = 0 ;
	string FileName;
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
	int CurrentLineNo;
	int Delay;
	string Statistics;
	core(){
		TotalInstructions=0;
		CurrentLineNo=1;
		Delay=0;
	}	
};
class DRAM_Handler{
	public:
	string DRAM_Processed, DRAM_PreProcessed;
	int MemAddress = -1;
	int ROW_NUMBER = -2;
	int delay, delay2;
	int Instruction_No;
	int File_No;
};
class MemoryManagerClass{
	public:
	vector<DRAM_Handler> MemoryManager;
	int LastRow = -1;
}MemoryManagerObj;
vector<core> AllFiles;
int ROW_ACCESS_DELAY, COL_ACCESS_DELAY;
int clkCycle = 1;
int index1=0;
void rm_space(string &str)
{
	int32_t j=0;
	while(j<str.size() && (str[j]==' ' || str[j]=='\t'))
	{
		j++;
	}
	str=str.substr(j);
}
int RemoveComma(string &current_instruction)
{
	if(current_instruction.size()<2 || current_instruction[0]!=',') //check that first element exists and is a comma
	{
		cout<<"Error: Comma expected: "<< AllFiles[index1].Instructions.at(AllFiles[index1].CurrentLineNo-1)<< " in file \""<<AllFiles[index1].FileName<<"\""<<endl<<endl;
		AllFiles[index1].CurrentLineNo=AllFiles[index1].TotalInstructions+1;
		return 0;
	}
	current_instruction=current_instruction.substr(1); //remove it
	return 1;
}

string findRegister(string &str, map<string, int> RegisterMap)
{

	if(str[0]!='$' || str.size()<2) //find '$' sign
	{
		cout<<"Error: Register expected here: "<< AllFiles[index1].Instructions.at(AllFiles[index1].CurrentLineNo-1)<< " in file \""<<AllFiles[index1].FileName<<"\""<<endl<<endl;
		AllFiles[index1].CurrentLineNo=AllFiles[index1].TotalInstructions+1;
		return "";
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
		auto itr = RegisterMap.find(registerID);
		if(itr == RegisterMap.end()){
			cout<<"Error: Invalid register: "<< AllFiles[index1].Instructions.at(AllFiles[index1].CurrentLineNo-1)<< " in file \""<<AllFiles[index1].FileName<<"\""<<endl<<endl;
			AllFiles[index1].CurrentLineNo=AllFiles[index1].TotalInstructions+1;
			return "";
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
				cout<<"Error: Unexpected text after value: "<<AllFiles[index1].Instructions.at(AllFiles[index1].CurrentLineNo-1)<<endl<<endl;
				AllFiles[index1].CurrentLineNo=AllFiles[index1].TotalInstructions+1;
				return "";
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
int number_verify(string z)
{
	int start=0;
	char character = z[0];
	if (character=='-') start++;
	for(int i=start;i<z.size();i++)
	{
		if(z[i]<48 || z[i]>57)
		{
			cout<<"Error: Given value is not a valid number: "<< AllFiles[index1].Instructions.at(AllFiles[index1].CurrentLineNo-1)<< " in file \""<<AllFiles[index1].FileName<<"\""<<endl;
			AllFiles[index1].CurrentLineNo=AllFiles[index1].TotalInstructions+1;
			return 0;
		}
	}
	if(character!='-'){ 
		if(z.size()>10 || (z.size()==10 && z>"2147483647")){
			cout<<"Error: Out of Range Integer: "<< AllFiles[index1].Instructions.at(AllFiles[index1].CurrentLineNo-1)<< " in file \""<<AllFiles[index1].FileName<<"\""<<endl;
			AllFiles[index1].CurrentLineNo=AllFiles[index1].TotalInstructions+1;
			return 0;
		}
	}
	else if(z.size()>11 || (z.size()==11 && z>"-2147483648")){
		cout<<"Error: Out of Range Integer:"<< AllFiles[index1].Instructions.at(AllFiles[index1].CurrentLineNo-1)<< " in file \""<<AllFiles[index1].FileName<<"\""<<endl<<endl;
		AllFiles[index1].CurrentLineNo=AllFiles[index1].TotalInstructions+1;
		return 0;
	}
	return 1;
}
int add(map<string, int> &RegisterMap, string str1, string str2, string str3)
{
	if(RegisterCount[index1].find(str1)!=RegisterCount[index1].end()||RegisterCount[index1].find(str2)!=RegisterCount[index1].end()||RegisterCount[index1].find(str3)!=RegisterCount[index1].end()){
		return 0;
	}
	cout<< "Cycle Number: " << to_string(clkCycle) <<endl<< "Instruction " << AllFiles[index1].CurrentLineNo-1+1 <<" in file \""<<AllFiles[index1].FileName<<"\"" << " :" << AllFiles[index1].Instructions.at(AllFiles[index1].CurrentLineNo-1) << "\n";
	long long int answer = long(RegisterMap.find(str2)->second)+long(RegisterMap.find(str3)->second);
	if(answer<-2147483648 || answer>2147483647){
		cout<<"Addition Overflow"<< AllFiles[index1].Instructions.at(AllFiles[index1].CurrentLineNo-1)<< " in file \""<<AllFiles[index1].FileName<<"\""<<endl<<endl;
		AllFiles[index1].CurrentLineNo=AllFiles[index1].TotalInstructions+1;
		return 0;
	}
	RegisterMap[str1]=RegisterMap.find(str2)->second+RegisterMap.find(str3)->second;
	cout<< "Modified Register And New Value: $" <<str1<<" = "<<RegisterMap.find(str1)->second<<endl<<endl;
	AllFiles[index1].CurrentLineNo++;
	return 1;
}
int sub(map<string, int> &RegisterMap, string str1, string str2, string str3)
{
	if(RegisterCount[index1].find(str1)!=RegisterCount[index1].end()||RegisterCount[index1].find(str2)!=RegisterCount[index1].end()||RegisterCount[index1].find(str3)!=RegisterCount[index1].end()){
		return 0;
	}
	cout<< "Cycle Number: " << to_string(clkCycle) <<endl<< "Instruction " << AllFiles[index1].CurrentLineNo-1+1 << " in file \""<<AllFiles[index1].FileName<<"\"" << " :"<< AllFiles[index1].Instructions.at(AllFiles[index1].CurrentLineNo-1)<< "\n";
	long long int answer = long(RegisterMap.find(str2)->second)-long(RegisterMap.find(str3)->second);
	if(answer<-2147483648 || answer>2147483647){
		cout<<"Subtraction Overflow"<<endl;
		AllFiles[index1].CurrentLineNo=AllFiles[index1].TotalInstructions+1;
		return 0;
	}
	RegisterMap[str1]=RegisterMap.find(str2)->second-RegisterMap.find(str3)->second;
	cout<< "Modified Register And New Value: $" <<str1<<" = "<<RegisterMap.find(str1)->second<<endl<<endl;
	AllFiles[index1].CurrentLineNo++;
	return 1;
}
int mul(map<string, int> &RegisterMap, string str1, string str2, string str3)
{
	if(RegisterCount[index1].find(str1)!=RegisterCount[index1].end()||RegisterCount[index1].find(str2)!=RegisterCount[index1].end()||RegisterCount[index1].find(str3)!=RegisterCount[index1].end()){
		return 0;
	}
	cout<< "Cycle Number: " << to_string(clkCycle) <<endl<< "Instruction " << AllFiles[index1].CurrentLineNo-1+1 <<" in file \""<<AllFiles[index1].FileName<<"\"" << " :" << AllFiles[index1].Instructions.at(AllFiles[index1].CurrentLineNo-1) << "\n";
	RegisterMap[str1]=RegisterMap.find(str2)->second*RegisterMap.find(str3)->second;
	cout<< "Modified Register And New Value: $" <<str1<<" = "<<RegisterMap.find(str1)->second<<endl<<endl;
	AllFiles[index1].CurrentLineNo++;
	return 1;
}
int slt(map<string, int> &RegisterMap, string str1, string str2, string str3)
{
	if(RegisterCount[index1].find(str1)!=RegisterCount[index1].end()||RegisterCount[index1].find(str2)!=RegisterCount[index1].end()||RegisterCount[index1].find(str3)!=RegisterCount[index1].end()){
		return 0;
	}
	cout<< "Cycle Number: " << to_string(clkCycle) <<endl<< "Instruction " << AllFiles[index1].CurrentLineNo-1+1 <<" in file \""<<AllFiles[index1].FileName<<"\"" << " :" << AllFiles[index1].Instructions.at(AllFiles[index1].CurrentLineNo-1) << "\n";
	if(RegisterMap.find(str2)->second<RegisterMap.find(str3)->second){
		RegisterMap[str1]=1;
		cout<< "Modified Register And New Value: $" <<str1<<" = "<<RegisterMap.find(str1)->second<<endl<<endl;
		AllFiles[index1].CurrentLineNo++;
	}
	else{
		RegisterMap[str1]=0;
		cout<< "Modified Register And New Value: $" <<str1<<" = "<<RegisterMap.find(str1)->second<<endl<<endl;
		AllFiles[index1].CurrentLineNo++;
	}
	return 1;
}
int lw(map<string, int> &RegisterMap,string str1, int str2,string str3){
	if(RegisterCount[index1].find(str1)!=RegisterCount[index1].end()){
		return 0;
	}
	string tmpStr = AllFiles[index1].Instructions.at(AllFiles[index1].CurrentLineNo-1);
	if(str2 > 1048576 || str2 < 0 || str2%4!=0){
		cout<<"Invalid Memory address"<< " in file \""<<AllFiles[index1].FileName<<"\"" << " :"<< AllFiles[index1].Instructions.at(AllFiles[index1].CurrentLineNo-1)<< "\n\n";
		AllFiles[index1].CurrentLineNo=AllFiles[index1].TotalInstructions+1;
		return 0;
	}
	int temp = str2 + 1;
	int k = -1;
	while(temp>0){
		temp= temp-1024;
		k++;
	}
	int ROW_NUMBER = k;
	int COL_NUMBER = str2 - (k*1024) ;
	int tmp = clkCycle;
	DRAM_Handler Object;
	Object.Instruction_No = AllFiles[index1].CurrentLineNo-1+1;
	Object.File_No = index1+1;
	Object.ROW_NUMBER=ROW_NUMBER;
	Object.MemAddress=k;
	int it=MemoryManagerObj.MemoryManager.size();
	if(it<64){
		if(it==0){
				int CurrentLineNoTemp = AllFiles[index1].CurrentLineNo;
				AllFiles[index1].CurrentLineNo++;
				while(AllFiles[index1].CurrentLineNo<=AllFiles[index1].TotalInstructions){
					int val = instruction_executer(-3,str1);
					if(val==-1){
						break;
					}
					if(val==1){
						AllFiles[index1].CurrentLineNo = CurrentLineNoTemp;
						AllFiles[index1].CurrentLineNo++;
						return -3;
					}
					AllFiles[index1].CurrentLineNo++;
				}
				AllFiles[index1].CurrentLineNo = CurrentLineNoTemp;
			RegisterCount[index1].insert({str1, AllFiles[index1].CurrentLineNo-1+1});
			AllFiles[index1].NoMemoryRequests++;
			MemoryManagerObj.MemoryManager.push_back(Object);
			MemoryManagerObj.MemoryManager.at(it).delay2 = tmp;
			MemoryManagerObj.MemoryManager.at(it).DRAM_PreProcessed = "Cycle Number: "+to_string(tmp)+"\nInstruction "+to_string(AllFiles[index1].CurrentLineNo-1+1)+" in file \""+AllFiles[index1].FileName+"\""+" :"+AllFiles[index1].Instructions.at(AllFiles[index1].CurrentLineNo-1)+"\n"+"Request Queued\n\n";
			cout<<MemoryManagerObj.MemoryManager.at(it).DRAM_PreProcessed;
			MemoryManagerObj.MemoryManager.at(it).delay2 = -1;
			if(ROW_CURRENT==-1){
				ROW_CURRENT = ROW_NUMBER;
				for(int i=0; i<1024; i++){
					ROW_BUFFER[i] = DRAM[ROW_NUMBER][i];
				}
				AllFiles[index1].RegisterMap[str1] = ROW_BUFFER[COL_NUMBER];
				MemoryManagerObj.MemoryManager.at(it).delay = tmp+ROW_ACCESS_DELAY+COL_ACCESS_DELAY+1;
				MemoryManagerObj.MemoryManager.at(it).DRAM_Processed="Cycle Number: "+to_string(tmp+1)+"-"+to_string(MemoryManagerObj.MemoryManager.at(it).delay)+"\n"+"Instruction "+ to_string(AllFiles[index1].CurrentLineNo-1+1) +" in file \""+AllFiles[index1].FileName+"\"" + " :" +AllFiles[index1].Instructions.at(AllFiles[index1].CurrentLineNo-1)+"\n"+"Modified Register And New Value: $"+str1+" = "+to_string(RegisterMap.find(str1)->second)+"\n"+"\n";
			AllFiles[index1].CurrentLineNo++;
				return 1;  
			}
			else if (ROW_NUMBER==MemoryManagerObj.LastRow){
				for(int i=0; i<1024; i++){
					DRAM[ROW_CURRENT][i] = ROW_BUFFER[i];
				}
				ROW_CURRENT = ROW_NUMBER;
				for(int i=0; i<1024; i++){
					ROW_BUFFER[i] = DRAM[ROW_NUMBER][i];
				}
				AllFiles[index1].RegisterMap[str1] = ROW_BUFFER[COL_NUMBER];
				MemoryManagerObj.MemoryManager.at(it).delay = tmp+COL_ACCESS_DELAY+1;
				MemoryManagerObj.MemoryManager.at(it).DRAM_Processed="Cycle Number: "+to_string(tmp+1)+"-"+to_string(MemoryManagerObj.MemoryManager.at(it).delay)+"\n"+"Instruction "+ to_string(AllFiles[index1].CurrentLineNo-1+1) +" in file \""+AllFiles[index1].FileName+"\"" + " :" +AllFiles[index1].Instructions.at(AllFiles[index1].CurrentLineNo-1)+"\n"+"Modified Register And New Value: $"+str1+" = "+to_string(RegisterMap.find(str1)->second)+"\n"+"\n";
			AllFiles[index1].CurrentLineNo++;
				return 1;  
			}
			else {
				for(int i=0; i<1024; i++){
					DRAM[ROW_CURRENT][i] = ROW_BUFFER[i];
				}
				ROW_CURRENT = ROW_NUMBER;
				for(int i=0; i<1024; i++){
					ROW_BUFFER[i] = DRAM[ROW_NUMBER][i];
				}
				AllFiles[index1].RegisterMap[str1] = ROW_BUFFER[COL_NUMBER];
				MemoryManagerObj.MemoryManager.at(it).delay = tmp+(2*ROW_ACCESS_DELAY)+COL_ACCESS_DELAY+1;
				MemoryManagerObj.MemoryManager.at(it).DRAM_Processed="Cycle Number: "+to_string(tmp+1)+"-"+to_string(MemoryManagerObj.MemoryManager.at(it).delay)+"\n"+"Instruction "+ to_string(AllFiles[index1].CurrentLineNo-1+1) +" in file \""+AllFiles[index1].FileName+"\"" + " :" +AllFiles[index1].Instructions.at(AllFiles[index1].CurrentLineNo-1)+"\n"+"Modified Register And New Value: $"+str1+" = "+to_string(RegisterMap.find(str1)->second)+"\n"+"\n";
			AllFiles[index1].CurrentLineNo++;
				return 1;  
			}
		}
		if(it==1){
			if(MemoryManagerObj.MemoryManager.at(it-1).delay2!=-1){
				return 0;
			}
				int CurrentLineNoTemp = AllFiles[index1].CurrentLineNo;
				AllFiles[index1].CurrentLineNo++;
				while(AllFiles[index1].CurrentLineNo<=AllFiles[index1].TotalInstructions){
					int val = instruction_executer(-3,str1);
					if(val==-1){
						break;
					}
					if(val==1){
						AllFiles[index1].CurrentLineNo = CurrentLineNoTemp;
						AllFiles[index1].CurrentLineNo++;
						return -3;
					}
					AllFiles[index1].CurrentLineNo++;
				}
				AllFiles[index1].CurrentLineNo = CurrentLineNoTemp;
			RegisterCount[index1].insert({str1, AllFiles[index1].CurrentLineNo-1+1});
			AllFiles[index1].NoMemoryRequests++;
			MemoryManagerObj.MemoryManager.push_back(Object);
			MemoryManagerObj.MemoryManager.at(it).delay2 = tmp;
			MemoryManagerObj.MemoryManager.at(it).DRAM_PreProcessed = "Cycle Number: "+to_string(tmp)+"\nInstruction "+to_string(AllFiles[index1].CurrentLineNo-1+1)+" in file \""+AllFiles[index1].FileName+"\""+" :"+AllFiles[index1].Instructions.at(AllFiles[index1].CurrentLineNo-1)+"\n"+"Request Queued\n\n";
			for(int i=0; i<1024; i++){
				DRAM[ROW_CURRENT][i] = ROW_BUFFER[i];
			}
			ROW_CURRENT = ROW_NUMBER;
			for(int i=0; i<1024; i++){
				ROW_BUFFER[i] = DRAM[ROW_NUMBER][i];
			}
			AllFiles[index1].RegisterMap[str1] = ROW_BUFFER[COL_NUMBER];
			cout<<MemoryManagerObj.MemoryManager.at(it).DRAM_PreProcessed;
			MemoryManagerObj.MemoryManager.at(it).delay2 = -1;
			if (ROW_CURRENT==MemoryManagerObj.MemoryManager.at(it-1).ROW_NUMBER){
				MemoryManagerObj.MemoryManager.at(it).delay = MemoryManagerObj.MemoryManager.at(it-1).delay+COL_ACCESS_DELAY+1;
				MemoryManagerObj.MemoryManager.at(it).DRAM_Processed="Cycle Number: "+to_string(MemoryManagerObj.MemoryManager.at(it-1).delay+1)+"-"+to_string(MemoryManagerObj.MemoryManager.at(it).delay)+"\n"+"Instruction "+ to_string(AllFiles[index1].CurrentLineNo-1+1) +" in file \""+AllFiles[index1].FileName+"\"" + " :" +AllFiles[index1].Instructions.at(AllFiles[index1].CurrentLineNo-1)+"\n"+"Modified Register And New Value: $"+str1+" = "+to_string(RegisterMap.find(str1)->second)+"\n"+"\n";
				AllFiles[index1].CurrentLineNo++;
				return 1;  
			}
			else {
				MemoryManagerObj.MemoryManager.at(it).delay = MemoryManagerObj.MemoryManager.at(it-1).delay+(2*ROW_ACCESS_DELAY)+COL_ACCESS_DELAY+1;
				MemoryManagerObj.MemoryManager.at(it).DRAM_Processed="Cycle Number: "+to_string(MemoryManagerObj.MemoryManager.at(it-1).delay+1)+"-"+to_string(MemoryManagerObj.MemoryManager.at(it).delay)+"\n"+"Instruction "+ to_string(AllFiles[index1].CurrentLineNo-1+1) +" in file \""+AllFiles[index1].FileName+"\"" + " :" +AllFiles[index1].Instructions.at(AllFiles[index1].CurrentLineNo-1)+"\n"+"Modified Register And New Value: $"+str1+" = "+to_string(RegisterMap.find(str1)->second)+"\n"+"\n";
				AllFiles[index1].CurrentLineNo++;
				return 1;  
			}
		}
		if(MemoryManagerObj.MemoryManager.at(indexH).delay2!=-1){
			return 0;
		}
		
				int CurrentLineNoTemp = AllFiles[index1].CurrentLineNo;
				AllFiles[index1].CurrentLineNo++;
				while(AllFiles[index1].CurrentLineNo<=AllFiles[index1].TotalInstructions){
					int val = instruction_executer(-3,str1);
					if(val==-1){
						break;
					}
					if(val==1){
						AllFiles[index1].CurrentLineNo = CurrentLineNoTemp;
						AllFiles[index1].CurrentLineNo++;
						return -3;
					}
					AllFiles[index1].CurrentLineNo++;
				}
				AllFiles[index1].CurrentLineNo = CurrentLineNoTemp;
		RegisterCount[index1].insert({str1, AllFiles[index1].CurrentLineNo-1+1});
		AllFiles[index1].NoMemoryRequests++;
		MemoryManagerObj.MemoryManager.push_back(Object);
		for(int i=0; i<1024; i++){
			DRAM[ROW_CURRENT][i] = ROW_BUFFER[i];
		}
		ROW_CURRENT = ROW_NUMBER;
		for(int i=0; i<1024; i++){
			ROW_BUFFER[i] = DRAM[ROW_NUMBER][i];
		}
		AllFiles[index1].RegisterMap[str1] = ROW_BUFFER[COL_NUMBER];
		int current = it-1;
		while(current>0){
			if(MemoryManagerObj.MemoryManager.at(current).MemAddress <= MemoryManagerObj.MemoryManager.at(it).MemAddress){
				break;
			}
			DRAM_Handler temp = MemoryManagerObj.MemoryManager.at(current);
			MemoryManagerObj.MemoryManager.at(current) = MemoryManagerObj.MemoryManager.at(it);
			MemoryManagerObj.MemoryManager.at(it) = temp;
			it--;
			current--;
		}
		MemoryManagerObj.MemoryManager.at(it).delay2 = tmp+MemoryManagerObj.MemoryManager.size()-it-1+2;
		if(tmp == MemoryManagerObj.MemoryManager.at(it).delay2){
			MemoryManagerObj.MemoryManager.at(it).DRAM_PreProcessed = "Cycle Number: "+to_string(tmp)+"\nInstruction "+to_string(AllFiles[index1].CurrentLineNo-1+1)+" in file \""+AllFiles[index1].FileName+"\""+" :"+AllFiles[index1].Instructions.at(AllFiles[index1].CurrentLineNo-1)+"\n"+"Request Queued\n\n";
			cout<<MemoryManagerObj.MemoryManager.at(it).DRAM_PreProcessed;
			MemoryManagerObj.MemoryManager.at(it).delay2 = -1 ;
		} 
		else {
		MemoryManagerObj.MemoryManager.at(it).DRAM_PreProcessed = "Cycle Number: "+to_string(tmp)+"-"+to_string(MemoryManagerObj.MemoryManager.at(it).delay2)+"\nInstruction "+to_string(AllFiles[index1].CurrentLineNo-1+1)+" in file \""+AllFiles[index1].FileName+"\""+" :"+AllFiles[index1].Instructions.at(AllFiles[index1].CurrentLineNo-1)+"\n"+"Request Queued\n\n";
		}
		int temporary;
		if(MemoryManagerObj.MemoryManager.at(current).ROW_NUMBER == ROW_NUMBER){
			temporary = COL_ACCESS_DELAY+1;
			MemoryManagerObj.MemoryManager.at(it).delay = MemoryManagerObj.MemoryManager.at(it-1).delay+temporary;
			MemoryManagerObj.MemoryManager.at(it).DRAM_Processed="Cycle Number: "+to_string(MemoryManagerObj.MemoryManager.at(it-1).delay+1)+"-"+to_string(MemoryManagerObj.MemoryManager.at(it).delay)+"\n"+"Instruction "+ to_string(AllFiles[index1].CurrentLineNo-1+1) +" in file \""+AllFiles[index1].FileName+"\"" + " :" +AllFiles[index1].Instructions.at(AllFiles[index1].CurrentLineNo-1)+"\n"+"Modified Register And New Value: $"+str1+" = "+to_string(RegisterMap.find(str1)->second)+"\n"+"\n";
		}
		else{
			temporary = (2*ROW_ACCESS_DELAY)+COL_ACCESS_DELAY+1;
			MemoryManagerObj.MemoryManager.at(it).delay = MemoryManagerObj.MemoryManager.at(it-1).delay+(2*ROW_ACCESS_DELAY)+COL_ACCESS_DELAY+1;
			MemoryManagerObj.MemoryManager.at(it).DRAM_Processed="Cycle Number: "+to_string(MemoryManagerObj.MemoryManager.at(it).delay2+1)+"-"+to_string(MemoryManagerObj.MemoryManager.at(it).delay)+"\n"+"Instruction "+ to_string(AllFiles[index1].CurrentLineNo-1+1) +" in file \""+AllFiles[index1].FileName+"\"" + " :" +AllFiles[index1].Instructions.at(AllFiles[index1].CurrentLineNo-1)+"\n"+"Modified Register And New Value: $"+str1+" = "+to_string(RegisterMap.find(str1)->second)+"\n"+"\n";
		}
		indexH=it;
		it++;
		while(it<MemoryManagerObj.MemoryManager.size()){
			MemoryManagerObj.MemoryManager.at(it).delay += temporary;
			MemoryManagerObj.MemoryManager.at(it).DRAM_Processed="Cycle Number: "+to_string(MemoryManagerObj.MemoryManager.at(it-1).delay+1)+"-"+to_string(MemoryManagerObj.MemoryManager.at(it).delay)+"\n" +MemoryManagerObj.MemoryManager.at(it).DRAM_Processed.substr(MemoryManagerObj.MemoryManager.at(it).DRAM_Processed.find("I"));
			it++;
		}
		AllFiles[index1].CurrentLineNo++;
		return 1;
	}
	else{
		return 0;
	}
}
int sw(map<string, int> &RegisterMap,string str1, int str2,string str3){
	if(RegisterCount[index1].find(str1)!=RegisterCount[index1].end()){
		return 0;
	}
	string tmpStr = AllFiles[index1].Instructions.at(AllFiles[index1].CurrentLineNo-1);
	if(str2 > 1048576 || str2 < 0 || str2%4!=0){
		cout<<"Invalid Memory address"<< " in file \""<<AllFiles[index1].FileName<<"\"" << " :"<< AllFiles[index1].Instructions.at(AllFiles[index1].CurrentLineNo-1)<< "\n\n";
		AllFiles[index1].CurrentLineNo=AllFiles[index1].TotalInstructions+1;
		return 0;
	}
	int temp = str2 + 1;
	int k = -1;
	while(temp>0){
		temp= temp-1024;
		k++;
	}
	int ROW_NUMBER = k;
	int COL_NUMBER = str2 - (k*1024) ;
	int tmp = clkCycle;
	DRAM_Handler Object;
	Object.Instruction_No = AllFiles[index1].CurrentLineNo-1+1;
	Object.File_No = index1+1;
	Object.ROW_NUMBER=ROW_NUMBER;
	Object.MemAddress=k;
	int it=MemoryManagerObj.MemoryManager.size();
	if(it<64){
		if(it==0){
				int CurrentLineNoTemp = AllFiles[index1].CurrentLineNo;
				AllFiles[index1].CurrentLineNo++;
				while(AllFiles[index1].CurrentLineNo<=AllFiles[index1].TotalInstructions){
					int val = instruction_executer(str2,"");
					if(val==-1){
						break;
					}
					if(val==1){
						AllFiles[index1].CurrentLineNo = CurrentLineNoTemp;
						AllFiles[index1].CurrentLineNo++;
						return -3;
					}
					AllFiles[index1].CurrentLineNo++;
				}
				AllFiles[index1].CurrentLineNo = CurrentLineNoTemp;
				AllFiles[index1].NoInstructions++;
			AllFiles[index1].NoMemoryRequests++;
			MemoryManagerObj.MemoryManager.push_back(Object);
			MemoryManagerObj.MemoryManager.at(it).delay2 = tmp;
			MemoryManagerObj.MemoryManager.at(it).DRAM_PreProcessed = "Cycle Number: "+to_string(tmp)+"\nInstruction "+to_string(AllFiles[index1].CurrentLineNo-1+1)+" in file \""+AllFiles[index1].FileName+"\""+" :"+AllFiles[index1].Instructions.at(AllFiles[index1].CurrentLineNo-1)+"\n"+"Request Queued\n\n";
			cout<<MemoryManagerObj.MemoryManager.at(it).DRAM_PreProcessed;
			MemoryManagerObj.MemoryManager.at(it).delay2 = -1;
			if(ROW_CURRENT==-1){
				ROW_CURRENT = ROW_NUMBER;
				for(int i=0; i<1024; i++){
					ROW_BUFFER[i] = DRAM[ROW_NUMBER][i];
				}
				ROW_BUFFER[COL_NUMBER] = AllFiles[index1].RegisterMap[str1];
				MemoryManagerObj.MemoryManager.at(it).delay = tmp+ROW_ACCESS_DELAY+COL_ACCESS_DELAY+1;
				MemoryManagerObj.MemoryManager.at(it).DRAM_Processed="Cycle Number: "+to_string(tmp+1)+"-"+to_string(MemoryManagerObj.MemoryManager.at(it).delay)+"\n"+"Instruction "+ to_string(AllFiles[index1].CurrentLineNo-1+1) +" in file \""+AllFiles[index1].FileName+"\"" + " :" +AllFiles[index1].Instructions.at(AllFiles[index1].CurrentLineNo-1)+"\n"+"Modified Memory Location And New Value: "+to_string(str2)+"-"+to_string(str2+3)+": "+to_string(RegisterMap.find(str1)->second)+"\n"+"\n";
			AllFiles[index1].CurrentLineNo++;
				return 1;  
			}
			else if (ROW_NUMBER==MemoryManagerObj.LastRow){
				for(int i=0; i<1024; i++){
					DRAM[ROW_CURRENT][i] = ROW_BUFFER[i];
				}
				ROW_CURRENT = ROW_NUMBER;
				for(int i=0; i<1024; i++){
					ROW_BUFFER[i] = DRAM[ROW_NUMBER][i];
				}
				ROW_BUFFER[COL_NUMBER] = AllFiles[index1].RegisterMap[str1];
				MemoryManagerObj.MemoryManager.at(it).delay = tmp+COL_ACCESS_DELAY+1;
				MemoryManagerObj.MemoryManager.at(it).DRAM_Processed="Cycle Number: "+to_string(tmp+1)+"-"+to_string(MemoryManagerObj.MemoryManager.at(it).delay)+"\n"+"Instruction "+ to_string(AllFiles[index1].CurrentLineNo-1+1) +" in file \""+AllFiles[index1].FileName+"\"" + " :" +AllFiles[index1].Instructions.at(AllFiles[index1].CurrentLineNo-1)+"\n"+"Modified Memory Location And New Value: "+to_string(str2)+"-"+to_string(str2+3)+": "+to_string(RegisterMap.find(str1)->second)+"\n"+"\n";
			AllFiles[index1].CurrentLineNo++;
				return 1;  
			}
			else {
				for(int i=0; i<1024; i++){
					DRAM[ROW_CURRENT][i] = ROW_BUFFER[i];
				}
				ROW_CURRENT = ROW_NUMBER;
				for(int i=0; i<1024; i++){
					ROW_BUFFER[i] = DRAM[ROW_NUMBER][i];
				}
				ROW_BUFFER[COL_NUMBER] = AllFiles[index1].RegisterMap[str1];
				MemoryManagerObj.MemoryManager.at(it).delay = tmp+(2*ROW_ACCESS_DELAY)+COL_ACCESS_DELAY+1;
				MemoryManagerObj.MemoryManager.at(it).DRAM_Processed="Cycle Number: "+to_string(tmp+1)+"-"+to_string(MemoryManagerObj.MemoryManager.at(it).delay)+"\n"+"Instruction "+ to_string(AllFiles[index1].CurrentLineNo-1+1) +" in file \""+AllFiles[index1].FileName+"\"" + " :" +AllFiles[index1].Instructions.at(AllFiles[index1].CurrentLineNo-1)+"\n"+"Modified Memory Location And New Value: "+to_string(str2)+"-"+to_string(str2+3)+": "+to_string(RegisterMap.find(str1)->second)+"\n"+"\n";
			AllFiles[index1].CurrentLineNo++;
				return 1;  
			}
		}
		if(it==1){
			if(MemoryManagerObj.MemoryManager.at(it-1).delay2!=-1){
				return 0;
			}
				int CurrentLineNoTemp = AllFiles[index1].CurrentLineNo;
				AllFiles[index1].CurrentLineNo++;
				while(AllFiles[index1].CurrentLineNo<=AllFiles[index1].TotalInstructions){
					int val = instruction_executer(str2,"");
					if(val==-1){
						break;
					}
					if(val==1){
						AllFiles[index1].CurrentLineNo = CurrentLineNoTemp;
						AllFiles[index1].CurrentLineNo++;
						return -3;
					}
					AllFiles[index1].CurrentLineNo++;
				}
				AllFiles[index1].CurrentLineNo = CurrentLineNoTemp;
				AllFiles[index1].NoInstructions++;
			AllFiles[index1].NoMemoryRequests++;
			MemoryManagerObj.MemoryManager.push_back(Object);
			MemoryManagerObj.MemoryManager.at(it).delay2 = tmp;
			MemoryManagerObj.MemoryManager.at(it).DRAM_PreProcessed = "Cycle Number: "+to_string(tmp)+"\nInstruction "+to_string(AllFiles[index1].CurrentLineNo-1+1)+" in file \""+AllFiles[index1].FileName+"\""+" :"+AllFiles[index1].Instructions.at(AllFiles[index1].CurrentLineNo-1)+"\n"+"Request Queued\n\n";
			for(int i=0; i<1024; i++){
				DRAM[ROW_CURRENT][i] = ROW_BUFFER[i];
			}
			ROW_CURRENT = ROW_NUMBER;
			for(int i=0; i<1024; i++){
				ROW_BUFFER[i] = DRAM[ROW_NUMBER][i];
			}
			ROW_BUFFER[COL_NUMBER] = AllFiles[index1].RegisterMap[str1];
			cout<<MemoryManagerObj.MemoryManager.at(it).DRAM_PreProcessed;
			MemoryManagerObj.MemoryManager.at(it).delay2 = -1;
			if (ROW_CURRENT==MemoryManagerObj.MemoryManager.at(it-1).ROW_NUMBER){
				MemoryManagerObj.MemoryManager.at(it).delay = MemoryManagerObj.MemoryManager.at(it-1).delay+COL_ACCESS_DELAY+1;
				MemoryManagerObj.MemoryManager.at(it).DRAM_Processed="Cycle Number: "+to_string(MemoryManagerObj.MemoryManager.at(it-1).delay+1)+"-"+to_string(MemoryManagerObj.MemoryManager.at(it).delay)+"\n"+"Instruction "+ to_string(AllFiles[index1].CurrentLineNo-1+1) +" in file \""+AllFiles[index1].FileName+"\"" + " :" +AllFiles[index1].Instructions.at(AllFiles[index1].CurrentLineNo-1)+"\n"+"Modified Memory Location And New Value: "+to_string(str2)+"-"+to_string(str2+3)+": "+to_string(RegisterMap.find(str1)->second)+"\n"+"\n";
			AllFiles[index1].CurrentLineNo++;
				return 1;  
			}
			else {
				MemoryManagerObj.MemoryManager.at(it).delay = MemoryManagerObj.MemoryManager.at(it-1).delay+(2*ROW_ACCESS_DELAY)+COL_ACCESS_DELAY+1;
				MemoryManagerObj.MemoryManager.at(it).DRAM_Processed="Cycle Number: "+to_string(MemoryManagerObj.MemoryManager.at(it-1).delay+1)+"-"+to_string(MemoryManagerObj.MemoryManager.at(it).delay)+"\n"+"Instruction "+ to_string(AllFiles[index1].CurrentLineNo-1+1) +" in file \""+AllFiles[index1].FileName+"\"" + " :" +AllFiles[index1].Instructions.at(AllFiles[index1].CurrentLineNo-1)+"\n"+"Modified Memory Location And New Value: "+to_string(str2)+"-"+to_string(str2+3)+": "+to_string(RegisterMap.find(str1)->second)+"\n"+"\n";
			AllFiles[index1].CurrentLineNo++;
				return 1;  
			}
		}
		if(MemoryManagerObj.MemoryManager.at(indexH).delay2!=-1){
			return 0;
		}
		
				int CurrentLineNoTemp = AllFiles[index1].CurrentLineNo;
				AllFiles[index1].CurrentLineNo++;
				while(AllFiles[index1].CurrentLineNo<=AllFiles[index1].TotalInstructions){
					int val = instruction_executer(str2,"");
					if(val==-1){
						break;
					}
					if(val==1){
						AllFiles[index1].CurrentLineNo = CurrentLineNoTemp;
						AllFiles[index1].CurrentLineNo++;
						return -3;
					}
					AllFiles[index1].CurrentLineNo++;
				}
				AllFiles[index1].CurrentLineNo = CurrentLineNoTemp;
				AllFiles[index1].NoInstructions++;
		AllFiles[index1].NoMemoryRequests++;
		MemoryManagerObj.MemoryManager.push_back(Object);
		for(int i=0; i<1024; i++){
			DRAM[ROW_CURRENT][i] = ROW_BUFFER[i];
		}
		ROW_CURRENT = ROW_NUMBER;
		for(int i=0; i<1024; i++){
			ROW_BUFFER[i] = DRAM[ROW_NUMBER][i];
		}
		ROW_BUFFER[COL_NUMBER] = AllFiles[index1].RegisterMap[str1];
		int current = it-1;
		while(current>0){
			if(MemoryManagerObj.MemoryManager.at(current).MemAddress <= MemoryManagerObj.MemoryManager.at(it).MemAddress){
				break;
			}
			DRAM_Handler temp = MemoryManagerObj.MemoryManager.at(current);
			MemoryManagerObj.MemoryManager.at(current) = MemoryManagerObj.MemoryManager.at(it);
			MemoryManagerObj.MemoryManager.at(it) = temp;
			it--;
			current--;
		}
		MemoryManagerObj.MemoryManager.at(it).delay2 = tmp+MemoryManagerObj.MemoryManager.size()-it-1+2;
		if(tmp == MemoryManagerObj.MemoryManager.at(it).delay2){
			MemoryManagerObj.MemoryManager.at(it).DRAM_PreProcessed = "Cycle Number: "+to_string(tmp)+"\nInstruction "+to_string(AllFiles[index1].CurrentLineNo-1+1)+" in file \""+AllFiles[index1].FileName+"\""+" :"+AllFiles[index1].Instructions.at(AllFiles[index1].CurrentLineNo-1)+"\n"+"Request Queued\n\n";
			cout<<MemoryManagerObj.MemoryManager.at(it).DRAM_PreProcessed;
			MemoryManagerObj.MemoryManager.at(it).delay2 = -1 ;
		} 
		else {
		MemoryManagerObj.MemoryManager.at(it).DRAM_PreProcessed = "Cycle Number: "+to_string(tmp)+"-"+to_string(MemoryManagerObj.MemoryManager.at(it).delay2)+"\nInstruction "+to_string(AllFiles[index1].CurrentLineNo-1+1)+" in file \""+AllFiles[index1].FileName+"\""+" :"+AllFiles[index1].Instructions.at(AllFiles[index1].CurrentLineNo-1)+"\n"+"Request Queued\n\n";
		}
		int temporary;
		if(MemoryManagerObj.MemoryManager.at(current).ROW_NUMBER == ROW_NUMBER){
			temporary = COL_ACCESS_DELAY+1;
			MemoryManagerObj.MemoryManager.at(it).delay = MemoryManagerObj.MemoryManager.at(it-1).delay+temporary;
			MemoryManagerObj.MemoryManager.at(it).DRAM_Processed="Cycle Number: "+to_string(MemoryManagerObj.MemoryManager.at(it-1).delay+1)+"-"+to_string(MemoryManagerObj.MemoryManager.at(it).delay)+"\n"+"Instruction "+ to_string(AllFiles[index1].CurrentLineNo-1+1) +" in file \""+AllFiles[index1].FileName+"\"" + " :" +AllFiles[index1].Instructions.at(AllFiles[index1].CurrentLineNo-1)+"\n"+"Modified Memory Location And New Value: "+to_string(str2)+"-"+to_string(str2+3)+": "+to_string(RegisterMap.find(str1)->second)+"\n"+"\n";
		}
		else{
			temporary = (2*ROW_ACCESS_DELAY)+COL_ACCESS_DELAY+1;
			MemoryManagerObj.MemoryManager.at(it).delay = MemoryManagerObj.MemoryManager.at(it-1).delay+(2*ROW_ACCESS_DELAY)+COL_ACCESS_DELAY+1;
			MemoryManagerObj.MemoryManager.at(it).DRAM_Processed="Cycle Number: "+to_string(MemoryManagerObj.MemoryManager.at(it).delay2+1)+"-"+to_string(MemoryManagerObj.MemoryManager.at(it).delay)+"\n"+"Instruction "+ to_string(AllFiles[index1].CurrentLineNo-1+1) +" in file \""+AllFiles[index1].FileName+"\"" + " :" +AllFiles[index1].Instructions.at(AllFiles[index1].CurrentLineNo-1)+"\n"+"Modified Memory Location And New Value: "+to_string(str2)+"-"+to_string(str2+3)+": "+to_string(RegisterMap.find(str1)->second)+"\n"+"\n";
		}
		indexH=it;
		it++;
		while(it<MemoryManagerObj.MemoryManager.size()){
			MemoryManagerObj.MemoryManager.at(it).delay += temporary;
			MemoryManagerObj.MemoryManager.at(it).DRAM_Processed="Cycle Number: "+to_string(MemoryManagerObj.MemoryManager.at(it-1).delay+1)+"-"+to_string(MemoryManagerObj.MemoryManager.at(it).delay)+"\n" +MemoryManagerObj.MemoryManager.at(it).DRAM_Processed.substr(MemoryManagerObj.MemoryManager.at(it).DRAM_Processed.find("I"));
			it++;
		}
		AllFiles[index1].CurrentLineNo++;
		return 1;
	}
	else{
		return 0;
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
void beq(map<string, int> &RegisterMap, string str1, string str2, string str3)
{	
	if(!RegisterCount[index1].empty()){
		return;
	}
	cout<< "Cycle Number: " << to_string(clkCycle) <<endl<< "Instruction " << AllFiles[index1].CurrentLineNo-1+1 << " :" << AllFiles[index1].Instructions.at(AllFiles[index1].CurrentLineNo-1) << "\n\n";
	map<string, int>::iterator it = AllFiles[index1].LabelToLine.find(str3);
	if(it == AllFiles[index1].LabelToLine.end()){
		cout<<"Label Not Found: "<< AllFiles[index1].Instructions.at(AllFiles[index1].CurrentLineNo-1)<<endl;
		AllFiles[index1].CurrentLineNo=AllFiles[index1].TotalInstructions+1;
		return;
	}
	if(RegisterMap.find(str1)->second==RegisterMap.find(str2)->second){
		AllFiles[index1].CurrentLineNo = AllFiles[index1].LabelToLine.find(str3)->second;
	}
	else{
		AllFiles[index1].CurrentLineNo++;
	}
}
void bne(map<string, int> &RegisterMap, string str1, string str2, string str3)
{
	if(!RegisterCount[index1].empty()){
		return;
	}
	cout<< "Cycle Number: " << to_string(clkCycle) <<endl<< "Instruction " << AllFiles[index1].CurrentLineNo-1+1 << " :" << AllFiles[index1].Instructions.at(AllFiles[index1].CurrentLineNo-1) << "\n\n";
	map<string, int>::iterator it = AllFiles[index1].LabelToLine.find(str3);
	if(it == AllFiles[index1].LabelToLine.end()){
		cout<<"Label Not Found: "<< AllFiles[index1].Instructions.at(AllFiles[index1].CurrentLineNo-1)<<endl<<endl;
		AllFiles[index1].CurrentLineNo=AllFiles[index1].TotalInstructions+1;
		return;
	}
	if(RegisterMap.find(str1)->second!=RegisterMap.find(str2)->second){
		AllFiles[index1].CurrentLineNo = AllFiles[index1].LabelToLine.find(str3)->second;
	}
	else{
		AllFiles[index1].CurrentLineNo++;
	}
}
void j(map<string, int> &RegisterMap, string str1)
{
	if(!RegisterCount[index1].empty()){
		return;
	}
	cout<< "Cycle Number: " << to_string(clkCycle) <<endl<< "Instruction " << AllFiles[index1].CurrentLineNo-1+1 << " :" << AllFiles[index1].Instructions.at(AllFiles[index1].CurrentLineNo-1) << "\n\n";
	map<string, int>::iterator it = AllFiles[index1].LabelToLine.find(str1);
	if(it == AllFiles[index1].LabelToLine.end()){
		cout<<"Label Not Found: "<< AllFiles[index1].Instructions.at(AllFiles[index1].CurrentLineNo-1)<<endl<<endl;
		AllFiles[index1].CurrentLineNo=AllFiles[index1].TotalInstructions+1;
		return;
	}
	AllFiles[index1].CurrentLineNo = AllFiles[index1].LabelToLine.find(str1)->second;
}
int addi(map<string, int> &RegisterMap, string str1, string str2, int str3)
{
	if(RegisterCount[index1].find(str1)!=RegisterCount[index1].end()||RegisterCount[index1].find(str2)!=RegisterCount[index1].end()){
		return 0;
	}
	cout<< "Cycle Number: " << to_string(clkCycle) <<endl<< "Instruction " << AllFiles[index1].CurrentLineNo-1+1 <<" in file \""<<AllFiles[index1].FileName<<"\"" << " :" << AllFiles[index1].Instructions.at(AllFiles[index1].CurrentLineNo-1) << "\n";
	long long int answer = long(RegisterMap.find(str2)->second)+long(str3);
	if(answer<-2147483648 || answer>2147483647){
		cout<<"Addition Overflow: "<< AllFiles[index1].Instructions.at(AllFiles[index1].CurrentLineNo-1)<<endl<<endl;
		AllFiles[index1].CurrentLineNo=AllFiles[index1].TotalInstructions+1;
		return 0;
	}
	RegisterMap[str1]=RegisterMap.find(str2)->second+str3;
	cout<< "Modified Register And New Value: $" <<str1<<" = "<<RegisterMap.find(str1)->second<<endl<<endl;
	AllFiles[index1].CurrentLineNo++;
	return 1;
}
int instruction_executer(int num=-1, string str=""){
	string l = AllFiles[index1].Instructions.at(AllFiles[index1].CurrentLineNo-1);
	
	//remove comments
	if(l.find("#")!=-1) //remove comments
	{
		l = l.substr(0,l.find("#"));
	}
	
	//remove spaces
	rm_space(l);
	if(l=="") //if there was only comment
	{
		return 1;
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
			cout<< "Label Encountered: " <<AllFiles[index1].Instructions.at(AllFiles[index1].CurrentLineNo-1)<< " in file \""<<AllFiles[index1].FileName<<"\"" << "\n\n";
		}
		AllFiles[index1].CurrentLineNo++;
		return 0;
	}
	if((l.size()<4&&l[0]!='j') || (l[0] == 'j' && l.size()<3)) //no valid instruction is this small
	{
		if(num!=-1 || str!=""){
			return 0;
		}
		cout<<"Error: Invalid operation: "<< AllFiles[index1].Instructions.at(AllFiles[index1].CurrentLineNo-1)<< " in file \""<<AllFiles[index1].FileName<<"\""<<endl<<endl;
		AllFiles[index1].CurrentLineNo=AllFiles[index1].TotalInstructions+1;
		return 0;
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
if(num==-1){
	if(operation != "add" && operation != "sub" && operation != "mul" && operation != "slt" && operation != "addi" && operation != "lw" && operation != "sw" && operation != "beq" && operation != "bne" && operation != "j") //if not valid
	{
		cout<<"Error: Invalid operation: "<< AllFiles[index1].Instructions.at(AllFiles[index1].CurrentLineNo-1)<< " in file \""<<AllFiles[index1].FileName<<"\""<<endl<<endl;
		AllFiles[index1].CurrentLineNo=AllFiles[index1].TotalInstructions+1;
		return 0;
	}
}
	int32_t foundOp=0; //whether valid operation or not
	string register1, register2, register3;
	int imme_value;
	string address;
	
	if(operation == "add" || operation == "sub" || operation == "mul" || operation == "slt"){

		rm_space(l);
		register1 = findRegister(l, AllFiles[index1].RegisterMap);
		if(register1==""){
			return -5;
		}
		rm_space(l);
		if(RemoveComma(l)==0){
			return -5;
		}

		rm_space(l);
		register2 = findRegister(l, AllFiles[index1].RegisterMap);
		if(register2==""){
			return -5;
		}
		rm_space(l);
		if(RemoveComma(l)==0){
			return -5;
		}

		rm_space(l);
		register3 = findRegister(l, AllFiles[index1].RegisterMap);
		if(register3==""){
			return -5;
		}
		rm_space(l);
		no_of_register = 3;
if(num==-1){
		if(l!="") //if something more found
		{
			cout<<"Error: More arguments provided than required: "<< AllFiles[index1].Instructions.at(AllFiles[index1].CurrentLineNo-1)<< " in file \""<<AllFiles[index1].FileName<<"\""<<endl<<endl;
			AllFiles[index1].CurrentLineNo=AllFiles[index1].TotalInstructions+1;
			return 0;
		}
		else if(operation == "add"){
			return add(AllFiles[index1].RegisterMap,register1,register2,register3);
		}else if(operation == "sub"){
			return sub(AllFiles[index1].RegisterMap,register1,register2,register3);
		}else if(operation == "mul"){
			return mul(AllFiles[index1].RegisterMap,register1,register2,register3);
		}else{
			return slt(AllFiles[index1].RegisterMap,register1,register2,register3);
		}
}
else {
		if(register2==str || register3==str){
			return -1;
		}
		else if (register1==str){
			return 1;
		}
		else{
			return -2;
		}
}
	}else if(operation == "addi"){

		rm_space(l);
		register1 = findRegister(l, AllFiles[index1].RegisterMap);
		if(register1==""){
			return -5;
		}
		rm_space(l);
		if(RemoveComma(l)==0){
			return -5;
		}

		rm_space(l);
		register2 = findRegister(l, AllFiles[index1].RegisterMap);
		if(register2==""){
			return -5;
		}
		rm_space(l);
		if(RemoveComma(l)==0){
			return -5;
		}

		rm_space(l);
		string tempString = findLabel(l); //find third argument, a number
		if(tempString==""){
			return -5;
		}
		if(number_verify(tempString)==0){
			return -5;
		} //check validity
		imme_value=stoi(tempString); //convert and store
		no_of_register = 2;
if(num==-1){
		return addi(AllFiles[index1].RegisterMap,register1,register2,imme_value);
}
else {
		if(register2==str){
			return -1;
		}
		else if (register1==str){
			return 1;
		}
		else{
			return -2;
		}
}
	}else if(operation == "lw" || operation == "sw"){

		string tempString="";
		rm_space(l);
		register1 = findRegister(l, AllFiles[index1].RegisterMap); //find source/destination register
		if(register1==""){
			return -5;
		}
		rm_space(l);
		if(RemoveComma(l)==0){
			return -5;
		}
		rm_space(l);
		int i, num1;
		string tmpStr1;
		for(i=0; i<l.size(); i++){
			if(l[i]=='('){
				break;
			}
		}
		if(i>=l.size()){
			cout<<"Invalid Memory Address: "<< AllFiles[index1].Instructions.at(AllFiles[index1].CurrentLineNo-1)<< " in file \""<<AllFiles[index1].FileName<<"\""<<endl<<endl;
			AllFiles[index1].CurrentLineNo=AllFiles[index1].TotalInstructions+1;
			return false;
		}
		tmpStr1= l.substr(0,i);
		rm_space(tmpStr1);
		if(number_verify(tempString)==0){
			return -5;
		} 
		num1 = stoi(tmpStr1);
		while(l[l.size()-1]=='\t'||l[l.size()-1]==' '){
		 	l = l.substr(0,l.size()-1);
		}
		if(l[l.size()-1]!=')'){
			cout<<"Invalid Memory Address: "<< AllFiles[index1].Instructions.at(AllFiles[index1].CurrentLineNo-1)<< " in file \""<<AllFiles[index1].FileName<<"\""<<endl<<endl;
			AllFiles[index1].CurrentLineNo=AllFiles[index1].TotalInstructions+1;
			return false;
		}
		l = l.substr(i+1,l.size()-2);
		string registerFind = findRegister(l, AllFiles[index1].RegisterMap);
		if(registerFind==""){
			return -5;
		}
		int address = AllFiles[index1].RegisterMap.find(registerFind)->second + num1;
		
		tempString = findLabel(l);
		if(tempString==""){
			return -5;
		}
		no_of_register = 1;
		if(operation == "lw"){
		if(num==-1){
			return lw(AllFiles[index1].RegisterMap,register1,address,registerFind);
		}
		else{
			if(address==num){
				return -1;
			}
			if(register1==str){
				return 1;
			}
			else{
				return -2;
			}
		}
		}else{
		if(num==-1){
			return sw(AllFiles[index1].RegisterMap,register1,address,registerFind);
		}
		else{
			if(register1==str){
				return -1;
			}
			if(address==num){
				return 1;
			}
			else{
				return -2;
			}
		}	
		}
	}else if(operation == "beq" || operation == "bne"){
		rm_space(l);
		register1 = findRegister(l, AllFiles[index1].RegisterMap);
		if(register1==""){
			return -5;
		}
		rm_space(l);
		if(RemoveComma(l)==0){
			return -5;
		}

		rm_space(l);
		register2 = findRegister(l, AllFiles[index1].RegisterMap);
		if(register2==""){
			return -5;
		}
		rm_space(l);
		if(RemoveComma(l)==0){
			return -5;
		}

		rm_space(l);

		string tempString="";
		tempString = findLabel(l);
		if(tempString==""){
			return -5;
		}
		address = tempString;
		no_of_register = 2;
if(num==-1){
		if(operation == "beq"){
			beq(AllFiles[index1].RegisterMap,register1,register2,address);
		}else{
			bne(AllFiles[index1].RegisterMap,register1,register2,address);
		}
}
else{
		return -1;
}
	}else if(operation == "j"){
		
		string tempString="";
		rm_space(l);
		tempString = findLabel(l);
		if(tempString==""){
			return -5;
		}
		address = tempString;
		no_of_register = 1;
if(num==-1){
		j(AllFiles[index1].RegisterMap, address);
}
else{
		return -1;
}
	}
	return 1;

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
	int M = stoi(argv[3]);
	int N = stoi(argv[4]);
	if(N>64){
		cout<<"No.Of Cores Exceed Maximum Allowed Value of 64"<<endl;
		exit(1);
	}
	if(argc!=N+5){
		cout<<"Number of Cores not equal to the No. Of Files"<<endl;
		exit(1);
	}
	AllFiles.resize(N);
	RegisterCount.resize(N);
	for(int i=0; i<N; i++){
		ifstream InputStream;
		AllFiles[i].FileName = argv[i+5];
		InputStream.open(argv[i+5],ios::in);
		if(!InputStream)
		{
			cout<<"Error: File \""<<argv[i+5]<<"\" does not exist or could not be opened"<<endl;
			exit(1);
		}
		string tmpStr;
		while(getline(InputStream,tmpStr))
		{
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
					AllFiles[i].LabelToLine[tmpStr1] = AllFiles[i].TotalInstructions+1;
				}
				else{
					AllFiles[i].Instructions.push_back(tmpStr);
					AllFiles[i].TotalInstructions++;
				}
			}
			if(AllFiles[index1].TotalInstructions>131072){
					cout<<"Number of Instructions exceeded the maximum allowed values\n";
					exit(1);
				}
		}
		InputStream.close();
	}
	int temp_clkCycleCount;
	while(clkCycle<=M){
		int index = -1 ; 
		for(int i=0; i<N; i++){
			int done = -1 ; 
			index1 = i;
			if(AllFiles[index1].CurrentLineNo<=AllFiles[index1].TotalInstructions || !MemoryManagerObj.MemoryManager.empty()){
				if(MemoryManagerObj.MemoryManager.size()>0){
					
					if(indexH>=0 && MemoryManagerObj.MemoryManager.at(indexH).delay2==clkCycle){
						cout<<MemoryManagerObj.MemoryManager.at(indexH).DRAM_PreProcessed;
						MemoryManagerObj.MemoryManager.at(indexH).delay2=0;
						index = indexH;
					}
					if(MemoryManagerObj.MemoryManager.at(0).delay==clkCycle && MemoryManagerObj.MemoryManager.at(0).File_No==index1+1){
						
						cout<<MemoryManagerObj.MemoryManager.at(0).DRAM_Processed;
						int temp = MemoryManagerObj.MemoryManager.at(0).Instruction_No;
						done = MemoryManagerObj.MemoryManager.at(0).DRAM_Processed.find("Register");
						if(done!=-1){
							for(auto it = RegisterCount[index1].begin(); it!= RegisterCount[index1].end(); it++){
								if (it->second==temp){
									RegisterCount[index1].erase(it);
									break;
								}
							}
						}
						else {
							AllFiles[index1].NoInstructions--;
						}
						if(MemoryManagerObj.LastRow!=MemoryManagerObj.MemoryManager.at(0).ROW_NUMBER){
							MemoryManagerObj.LastRow=MemoryManagerObj.MemoryManager.at(0).ROW_NUMBER;
							ROW_BUFFER_UPDATES_HELPER++;
						}
						MemoryManagerObj.MemoryManager.erase(MemoryManagerObj.MemoryManager.begin());
						AllFiles[index1].clkCycle = clkCycle;
						indexH--;	
					}
				}
				if(AllFiles[index1].CurrentLineNo<=AllFiles[index1].TotalInstructions && done==-1){
					while(instruction_executer()==-3){
					if(!(AllFiles[index1].CurrentLineNo<=AllFiles[index1].TotalInstructions))break;
					}
					AllFiles[index1].clkCycle = clkCycle;
				}
				temp_clkCycleCount=clkCycle;
			}
		}
		if(MemoryManagerObj.MemoryManager.size()>0 && indexH>=0){
			MemoryManagerObj.MemoryManager.at(indexH).delay2=-1;
		}
		clkCycle++;
	}
	cout<<"Statistics: "<<endl;
	int total = 0;
	int totalCycles = INT_MIN;
	for(int i=0; i<N; i++){
		cout<<"Core "<<i+1<<": \n"<<"No. Of Instructions Completed: "<<AllFiles[i].CurrentLineNo-1-RegisterCount[i].size()-AllFiles[i].NoInstructions<<endl;
		cout<<"No. Of Clock Cycles Taken: "<<AllFiles[i].clkCycle<<endl;
		cout<<"No. Of Requests Made To The Memory Manager: "<<AllFiles[i].NoMemoryRequests<<endl;
		cout<<"No. Of Requests Pending In The Memory Manager: "<<RegisterCount[i].size()+AllFiles[i].NoInstructions<<endl;
		total+=AllFiles[i].CurrentLineNo-1-RegisterCount[i].size()-AllFiles[i].NoInstructions;
		totalCycles=max(totalCycles, AllFiles[i].clkCycle);
	}
	cout<<"\nTotal No. Of Clock Cycles Taken: "<<totalCycles<<endl;
	cout<<"Total No. Of Instructions Completed By The Whole System In This Period: "<<total<<endl;
	cout<<"Total No. Of Row Buffer Updates: "<<ROW_BUFFER_UPDATES_HELPER<<endl;
	cout<<"Total No. Of Requests Pending In The Memory Manager: "<<MemoryManagerObj.MemoryManager.size()<<endl;
}
