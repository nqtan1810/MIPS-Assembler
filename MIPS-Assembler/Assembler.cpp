#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <sstream>

using namespace std;

int PC = 4194304 - 4; // base PC address in integer
map<string, int> addr;
map<string, string> opcode;
map<string, string> reg;
map<string, string> funct;
vector<string> ins;

bool isSpace(char x); // ham kiem tra ky tu la khoang trong
bool splitLabelAndInstrruction(string Line, string& Label, string& Ins); // ham tach label va instruction

int convertBinaryStringToDec(string bin);
string convertHexToBinaryNbits(string hex, int N);
string convertIntToHexString(int); // ham chuyen so int sang so hex
string convertIntToBinStringNbits(int, int); // ham chuyen so int sang so bin
string instructionType(string); // ham tra ve dinh dang lenh: R-type, I-type, J-type

void setAddr();
void setOpcode(); // ham khoi tao opcode cho cac lenh
void setReg(); // ham khoi tao gia tri cua register o dang binary
void setFunct(); // ham khoi tao truong function cho cac lenh

void extractInstructionFromInputFile(fstream& file_in); // ham tach cac lenh, label, xoa bo cac cmt, khoang trang thua, dau phay
void deleteComment(string& line); // ham xoa comment
void deleteSpace(string& line); // ham xoa tat ca khoang trong
void deleteExcessSpace(string& line); // ham xoa khoang trong thua 
void deleteSpaceFrontBack(string& line); // ham xoa khoang trang o dau cuoi day
void changeTabAndBracketToSpace(string &line); // ham thay the '\t' tro thanh ' '
void deleteComma(string& line); // ham xoa dau phay
void deleteLabel(string& line); // ham xoa label

void writeExtractedInstructionToOutputFile(fstream& file_out); // ham xuat lenh ra file .txt

void generateToBinaryCodeAndWriteResult(fstream& file_out, fstream& file_bin); // ham xuat binary code tuong ung voi cac lenh ra file .txt
string generateR_typeToBinary(vector<string>); // ham chuyen lenh R-type sang Binary code
string generateI_typeToBinary(vector<string>); // ham chuyen lenh I-type sang Binary code
string generateJ_typeToBinary(vector<string>); // ham chuyen lenh J-type sang Binary code
void twoComplement(string &bin);

void setAddr(){
	addr["syscall"] = 12;
}

void setOpcode(){
	// opcode for R-type instruction
	opcode["add"] = "000000";
	opcode["addu"] = "000000";
	opcode["and"] = "000000";
	opcode["jr"] = "000000";
	opcode["nor"] = "000000";
	opcode["or"] = "000000";
	opcode["slt"] = "000000";
	opcode["sltu"] = "000000";
	opcode["sll"] = "000000";
	opcode["srl"] = "000000";
	opcode["sub"] = "000000";
	opcode["subu"] = "000000";
	
	// opcode for I-type instruction
	opcode["addi"] = "001000";
	opcode["addiu"] = "001001";
	opcode["andi"] = "001100";
	opcode["beq"] = "000100";
	opcode["bne"] = "000101";
	opcode["lbu"] = "100100";
	opcode["lhu"] = "100101";
	opcode["ll"] = "110000";
	opcode["lui"] = "001111";
	opcode["lw"] = "100011";
	opcode["lb"] = "100000";
	opcode["ori"] = "001101";
	opcode["slti"] = "001010";
	opcode["sltiu"] = "001011";
	opcode["sb"] = "101000";
	opcode["sc"] = "111000";
	opcode["sh"] = "101001";
	opcode["sw"] = "101011";
	
	// opcode for J-type instruction
	opcode["j"] = "000010";
	opcode["jal"] = "000011";
}

void setReg(){
	reg["$0"] = reg["$zero"] = "00000";
	reg["$1"] = reg["$at"] = "00001";
	
	reg["$2"] = reg["$v0"] = "00010";
	reg["$3"] = reg["$v1"] = "00011";
	
	reg["$4"] = reg["$a0"] = "00100";
	reg["$5"] = reg["$a1"] = "00101";
	reg["$6"] = reg["$a2"] = "00110";
	reg["$7"] = reg["$a3"] = "00111";
	
	reg["$8"] = reg["$t0"] = "01000";
	reg["$9"] = reg["$t1"] = "01001";
	reg["$10"] = reg["$t2"] = "01010";
	reg["$11"] = reg["$t3"] = "01011";
	reg["$12"] = reg["$t4"] = "01100";
	reg["$13"] = reg["$t5"] = "01101";
	reg["$14"] = reg["$t6"] = "01110";
	reg["$15"] = reg["$t7"] = "01111";
	
	reg["$16"] = reg["$s0"] = "10000";
	reg["$17"] = reg["$s1"] = "10001";
	reg["$18"] = reg["$s2"] = "10010";
	reg["$19"] = reg["$s3"] = "10011";
	reg["$20"] = reg["$s4"] = "10100";
	reg["$21"] = reg["$s5"] = "10101";
	reg["$22"] = reg["$s6"] = "10110";
	reg["$23"] = reg["$s7"] = "10111";
	
	reg["$24"] = reg["$t8"] = "11000";
	reg["$25"] = reg["$t9"] = "11001";
	
	reg["$26"] = reg["$k0"] = "11010";
	reg["$27"] = reg["$k1"] = "11011";
	
	reg["$28"] = reg["$gp"] = "11100";
	reg["$29"] = reg["$sp"] = "11101";
	reg["$30"] = reg["$fp"] = "11110";
	reg["$31"] = reg["$ra"] = "11111";
}

// define funct in case instruction is R-type
void setFunct(){
	funct["add"] = "100000";
	funct["addu"] = "100001";
	funct["and"] = "100100";
	funct["jr"] = "001000";
	funct["nor"] = "100111";
	funct["or"] = "100101";
	funct["slt"] = "101010";
	funct["sltu"] = "101011";
	funct["sll"] = "000000";
	funct["srl"] = "000010";
	funct["sub"] = "100010";
	funct["subu"] = "100011";
}

bool isSpace(char x) {
	return x == ' ' || x == '\t';
}

bool splitLabelAndInstrruction(string Line, string& Label, string& Ins){
	if(Line[Line.length()-1] == ':')
		return false;
	int pos = Line.find(':');
	if(pos != string::npos){
		if(Line[pos+1] == ' '){
			Label = Line.substr(0, pos+1);
			Ins = Line.substr(pos+2);
		} else {
			Label = Line.substr(0, pos+1);
			Ins = Line.substr(pos+1);
		}
		return true;
	}
	return false;
}

string convertIntToHexString(int n){
	string s = "";
	while(n){
		int tmp = n % 16;
		n /= 16;
		if(tmp >= 0 && tmp <= 9) s += to_string(tmp);
		else if(tmp == 10) s += "a";
		else if(tmp == 11) s += "b";
		else if(tmp == 12) s += "c";
		else if(tmp == 13) s += "d";
		else if(tmp == 14) s += "e";
		else if(tmp == 15) s += "f";
 	}
 	reverse(s.begin(), s.end());
 	s = "0x" + string(8 - s.length(), '0') + s;
 	return s;
}

string convertIntToBinStringNbits(int n, int N){
	string s = "";
	int  k = abs(n);
	while(k){
		int tmp = k % 2;
		k /= 2;
		s += to_string(tmp);
 	}
 	reverse(s.begin(), s.end());
 	
 	if(n < 0){
		twoComplement(s);
		s = string(N - s.length(), '1') + s;
		return s;	
	}
	s = string(N - s.length(), '0') + s;
	return s;
}

string convertHexToBinaryNbits(string hex, int N){
	string bin = "";
	int i = 2;
	while(hex[i] == '0') i++;			// 0x00abc12
	for( ; i < hex.length(); i++){
		if(hex[i] >= '0' && hex[i] <= '9'){
			string tmp = "";
			tmp += hex[i];
			bin += convertIntToBinStringNbits(stoi(tmp), 4);
		}
		else {
			if(toupper(hex[i]) == 'A') bin += "1010";
			if(toupper(hex[i]) == 'B') bin += "1011";
			if(toupper(hex[i]) == 'C') bin += "1100";
			if(toupper(hex[i]) == 'D') bin += "1101";
			if(toupper(hex[i]) == 'E') bin += "1110";
			if(toupper(hex[i]) == 'F') bin += "1111";
		}
	}
	bin = string(N-bin.length(), '0') + bin;
	return bin;
}

int convertBinaryStringToDec(string bin){
	int kq = 0, tmp = 0;
	for(int i = bin.length()-1; i >= 0; i--){
		if(bin[i] == '1')
			kq += pow(2, tmp);
		tmp++;
	}
	return kq;
}

// ham tim so bu hai
void twoComplement(string &bin){
	for(int i = 0; i < bin.length(); i++){ // dao bit
		if(bin[i] == '0') bin[i] = '1';
		else bin[i] = '0';
	}
	int pos = bin.length() - 1;	// +1
	while(pos >= 0 && bin[pos] == '1'){
		bin[pos] = '0';
		pos--;
	}
	if(pos == -1) return;
	bin[pos] = '1';
}

string instructionType(string s){
	if(s == "add" || s == "addu" || s == "and" || s == "jr" || s == "nor" || s == "or"
		|| s == "slt" || s == "sltu" || s == "sll" || s == "srl" || s == "sub" || s == "subu"){
		return "R-type";
	} else if(s == "addi" || s == "addiu" || s == "andi" || s == "beq" || s == "bne" || s == "lbu" || s == "lhu"
		|| s == "ll" || s == "lui" || s == "lw" || s == "ori" || s == "slti" || s == "sltiu"
		|| s == "sb" || s == "sc" || s == "sh" || s == "sw" || s == "lb"){
		return "I-type";	
	} else if(s == "j" || s == "jal"){
		return "J-type";
	}
}

void deleteSpace(string& line) {
	for (int i = 0; i < line.length(); i++) {
		if (isSpace(line[i])) {
			line.erase(line.begin() + i);
			i--;
		}
	}
}

void deleteSpaceFrontBack(string& line) {
	int left = 0;
	int right = line.length() - 1;
	while (isSpace(line[left]) && left < line.length()) {
		left++;
	}
	while (isSpace(line[right]) && right >= 0) {
		right--;
	}
	if (left >= right) line = "";
	else line = line.substr(left, right - left + 1);
}

void deleteExcessSpace(string& line){
	for (int i = 0; i < line.length(); i++) {
		if (isSpace(line[i]) && isSpace(line[i+1])) {
			line.erase(line.begin() + i);
			i--;
		}
	}
}

void changeTabAndBracketToSpace(string &s){
	for(int i = 0; i < s.length(); i++){
		if(s[i] == '\t' || s[i] == '(' || s[i] == ')') s[i] = ' ';
	}
}

void deleteComma(string& line) {
	for (int i = 0; i < line.length(); i++) {
		if (line[i] == ',' && !isSpace(line[i+1])) {
			line[i] = ' ';
		} else if(line[i] == ',' && isSpace(line[i+1])){
			line.erase(line.begin() + i);
		}
	}
}

void deleteComment(string& line){
	int pos = 0;
	while (line[pos] != '#' && pos < line.length()) pos++;
	if(pos >= line.length()) return;
	line.erase(line.begin() + pos, line.end());
}

void extractInstructionFromInputFile(ifstream& file_in){
	while(file_in.eof() == false){
		string tmp;
		getline(file_in, tmp);
		deleteComment(tmp);
		deleteSpaceFrontBack(tmp);
		deleteExcessSpace(tmp);
		if(tmp == ".text"){
			while(file_in.eof() == false){
				string line;
				getline(file_in, line);
				deleteComment(line);
				deleteSpaceFrontBack(line);
				deleteComma(line);
				changeTabAndBracketToSpace(line);
				deleteExcessSpace(line);
				string Label = "";
				string Ins = "";
				bool isIncludeLabel = splitLabelAndInstrruction(line, Label, Ins);
				if(isIncludeLabel){
					ins.push_back(Label);
					ins.push_back(Ins);
				} else{
					if (!line.empty()) ins.push_back(line);
				}
			}
			return;
		}
	}
}

void writeExtractedInstructionToOutputFile(ofstream& file_out){
	for(int i = 0; i < ins.size(); i++){
		size_t pos = ins[i].find(':');
		if(pos != string::npos){
			addr[ins[i]] = PC+4;
			continue;	// loai bo label
		} else{
			PC += 4;
		}
		if (!ins[i].empty()){
			if(i == ins.size() - 1) file_out << ins[i];	// xuat lenh ra file
			else file_out << ins[i] << endl;
		}
		
	}
}

void generateToBinaryCodeAndWriteResult(ifstream& file_out, ofstream& file_bin){
	PC = 4194304 - 4; // reset base PC address in integer
	while(file_out.eof() == false){
		PC += 4;
		string line, tmp;
		getline(file_out, line);
		if(line == "syscall") file_bin << convertIntToBinStringNbits(addr["syscall"], 32) << endl;
		else{
			stringstream ss(line);
			vector<string> token;
			while(ss >> tmp){
				token.push_back(tmp);
			}
			if(!token.empty()){
				if(instructionType(token[0]) == "R-type"){
					file_bin << generateR_typeToBinary(token) << endl;
				}
				 else if(instructionType(token[0]) == "I-type"){
					file_bin << generateI_typeToBinary(token) << endl;
				} 
				else if(instructionType(token[0]) == "J-type"){
					file_bin << generateJ_typeToBinary(token) << endl;
				} 
			}
		}
	}
	
}

string generateR_typeToBinary(vector<string> token){
	string bin = "";
	bin += opcode[token[0]]; // opcode
	if(token[0] == "sll" || token[0] == "srl"){	// exception handling ( sll $s1, $t0, 2 or srl $s2, $t2, 2 )
		bin += "00000";			// rs
		bin += reg[token[2]];	// rt
		bin += reg[token[1]];	// rd
		bin += convertIntToBinStringNbits(stoi(token[3]), 5);	// shamt
		bin += funct[token[0]];			// function						
	} else if(token[0] == "jr"){ // exception handling ( jr rs )
		bin += reg[token[1]];	// rs
		bin += string(15, '0');	// 15 bits "0"
		bin += funct[token[0]];	// function
	} else {
		bin += reg[token[2]];	// rs
		bin += reg[token[3]];	// rt
		bin += reg[token[1]];	// rd
		bin += "00000";			// shamt
		bin += funct[token[0]];	// function
	}
	return bin;											
}

string generateI_typeToBinary(vector<string> token){
	string bin = "";
	bin += opcode[token[0]];
	if(token[0] == "beq" || token[0] == "bne"){ // beq $t0, $t1, exit
		bin += reg[token[1]];
		bin += reg[token[2]];
		bin += convertIntToBinStringNbits((addr[token[3]+":"] - PC - 4) / 4, 16);
	} 
	else if(token[0] == "ll" || token[0] == "lui"){ 
		bin += "00000";
		bin += reg[token[1]];
		if(token[2][1] == 'x'){
			string tmp = token[2];
			bin += convertHexToBinaryNbits(tmp, 16);
		} else {
			bin += convertIntToBinStringNbits(stoi(token[2]), 16);
		}
	} 
	else if(token[0] == "addi" || token[0] == "addiu" || token[0] == "andi"
		|| token[0] == "ori"|| token[0] == "slti"|| token[0] == "sltiu"){
		bin += reg[token[2]];
		bin += reg[token[1]];
		bin += convertIntToBinStringNbits(stoi(token[3]), 16);
	} else {
		if(token.size() == 3){			// lw $t0 $s
			bin += reg[token[2]];		// sw $t0 $s0
			bin += reg[token[1]];
			bin += "0000000000000000";
		} else {						// lw $t0 8 $s2
			bin += reg[token[3]];		// sw $t0 4 $s0
			bin += reg[token[1]];
			bin += convertIntToBinStringNbits(stoi(token[2]), 16);
		}
	}
	return bin;
}

string generateJ_typeToBinary(vector<string> token){
	string bin = "";
	bin += opcode[token[0]];
	bin += convertIntToBinStringNbits(addr[token[1]+":"] / 4, 26);
	return bin;
}

int main(){
	
	setAddr();
	setOpcode();
	setFunct();
	setReg();
	
	ifstream file_in("assembly_program.asm");
	ofstream file_out_ins("cleaned_program.txt");
	ifstream file_in_ins("cleaned_program.txt");
	ofstream file_out_bin("machine_code.bin");
	
	extractInstructionFromInputFile(file_in);
	writeExtractedInstructionToOutputFile(file_out_ins); 
	
	generateToBinaryCodeAndWriteResult(file_in_ins, file_out_bin);
	
	return 0;
}
