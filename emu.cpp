/*****************************************************************************
TITLE: EMULATOR																														
AUTHOR: CHIRANJIBI PRADHAN
ROLLNO: 2101CS21
Declaration of Authorship
This cpp file emu.cpp, is part of the miniproject of CS209/CS210 at the 
department of Computer Science and Engg, IIT Patna . 
*****************************************************************************/

#include <bits/stdc++.h>
using namespace std;

int A=0,B=0,pc=0,sp=0x100000; // The 4 registers. SP initialized to a random address far from 0.

string Instructions_name[19]={"ldc","adc","ldl","stl","ldnl","stnl","add","sub","shl","shr","adj","a2sp","sp2a","call","return","brz","brlz","br","HALT"};

map<string,pair<int,bool>>mnemonic{ // Table for mnemonics in the form {name, {opcode, num of operands}}
    {"ldc",{0,1}},
    {"adc",{1,1}},
    {"ldl",{2,1}},
    {"stl",{3,1}},
    {"ldnl",{4,1}},
    {"stnl",{5,1}},
    {"add",{6,0}},
    {"sub",{7,0}},
    {"shl",{8,0}},
    {"shr",{9,0}},
    {"adj",{10,1}},
    {"a2sp",{11,0}},
    {"sp2a",{12,0}},
    {"call",{13,1}},
    {"return",{14,0}},
    {"brz",{15,1}},
    {"brlz",{16,1}},
    {"br",{17,1}},
    {"HALT",{18,0}}
};

long long binpow(long long a, long long b) {
    long long res = 1;
    while (b > 0) {
        if (b & 1)
            res = res * a;
        a = a * a;
        b >>= 1;
    }
    return res;
}

string toHex(int num, int s){ // Converts num into a hex string of s characters.

    stringstream ss;
    ss<<hex<<num;

    string ans(ss.str());

    ans=string(max(0,s-(int)ans.size()),'0')+ans;

    if((int)ans.size()>s)ans=ans.substr(ans.size()-s,s);

    return ans;
}
 // For storing instructions read as binary in numeric {opcode , operand} format. Indexed by PC.
vector<pair<int,int>>  instructions ;


map<int,int>mem{}; // Implementing memory.
map<int,vector<int>>memtable{}; // For printing upto 4 memory values in a single line. 

ofstream tracefile;

void printTrace(int opd,int opc){ // Prints values of the 4 registers and the executed mnemonic and operand.

    tracefile<<"A: "<<toHex(A,8)<<" B: "<<toHex(B,8)<<" PC: "<<toHex(pc,8)<<" SP: "<<toHex(sp,8)<<" "<<Instructions_name[opc];

    if(opc<=18&&opc>=0&&mnemonic[Instructions_name[opc]].second) tracefile<<" "<<opd;

    tracefile<<endl;
}





int main(int argc,char* argv[]){

    streampos filesize;

    if(argc!=3){
        cout<<"Usage: ./emu [option] file.o\nWhere [option] is one of the following:" <<endl ;
        cout <<"-trace: generates trace file "<<endl ;
        cout <<"-memdump: shows memdump" ;
        return 0;
    }
    string filename=string(argv[2]);

    if(filename.find('.')!=string::npos)filename=filename.substr(0,filename.find('.'));

    ifstream file(argv[2],ios::binary);

    string flag=argv[1];

    if(flag=="-trace"){
        tracefile.open(filename+".trace");
        cout<<"Writing trace file.\n";
    }
    
    file.seekg(0,std::ios::end);
    filesize=file.tellg();
    file.seekg(0,std::ios::beg);

    vector<unsigned char>fdata(filesize); 

    file.read(reinterpret_cast<char*>(&fdata[0]),filesize); // Reads the entire file into this vector.

    for(int i=0;i<(int)fdata.size();i+=4){ // Converting binary data to integer format. Reads 4 bytes from vector in each iteration.
       
        int of= binpow(2,8) ;
        pair<int,int> next ={fdata[i], 0} ;
        next.second=fdata[i+1]|((int)fdata[i+2]<<8)|((int)fdata[i+3]<<16);
        if(next.second&0x800000)next.second-=0x1000000;
        mem[i/4]=(int)next.first|(next.second<<8); // Instructions stored in memory addresses starting from 0x0
        instructions.push_back(next);
    }

    int executions=0; // Number of instructions executed.

 
    bool halted=false;
    while(!halted){
        if(pc>=(int)instructions.size()){
            cout<<"Warning: Reached end of file without recieving HALT instruction. Program terminated.\n";
            break;
        }
        int of= binpow(2,4) ;

        int opcode=instructions[pc].first;// opcode
        int operand=instructions[pc].second; //operand
        if(flag=="-trace")printTrace(operand,opcode); // if flag is trace keep on writing in trace file
        //functions of opcode in if else
        if(opcode==0){
            B=A;
            A=operand;
        }else if(opcode==1){
            A+=operand;
        }else if(opcode==2){
            B=A;
            A=mem[sp+operand];
        }else if(opcode==3){
            mem[sp+operand]=A;
            A=B;
        }else if(opcode==4){
            A=mem[A+operand];
        }else if(opcode==5){
            mem[A+operand]=B;
        }else if(opcode==6){
            A=B+A;
        }else if(opcode==7){
            A=B-A;
        }else if(opcode==8){
            A=B<<A;
        }else if(opcode==9){
            A=B>>A;
        }else if(opcode==10){
            sp+=operand;
        }else if(opcode==11){
            sp=A;
            A=B;
        }else if(opcode==12){
            B=A;
            A=sp;
        }else if(opcode==13){
            B=A;
            A=pc;
            pc+=operand;
        }else if(opcode==14){
            pc=A;
            A=B;
        }else if(opcode==15){
            if(A==0)pc+=operand;
        }else if(opcode==16){
            if(A<0)pc+=operand;
        }else if(opcode==17){
            pc+=operand;
        }else if(opcode==18){
            halted=true;
        }else{
            cout<<"ERROR: Unknown opcode "<<opcode<<" at PC: "<<pc;
            return 0;
        }
        pc++;
        executions++;
    }

    
    if(flag=="-memdump"){
        cout<<"Memory Dump generated in file :" <<endl ;
     
        memtable.clear();
        ofstream memdump(filename +".mem") ;
         for(auto p:mem)
        {
            auto addr=p.first;

            addr/=4; 

            addr*=4;

            if(memtable.find(addr)!=memtable.end())memtable[addr].push_back(p.second);

            else if(memtable.find(addr+1)!=memtable.end()) memtable[addr+1].push_back(p.second);

            else if(memtable.find(addr+2)!=memtable.end()) memtable[addr+2].push_back(p.second);

            else memtable[p.first].push_back(p.second);
        }

        for(auto p:memtable)
        {
            memdump<<toHex(p.first,8)<<" ";
            for(auto v:p.second)memdump<<toHex(v,8)<<" ";
            memdump<<'\n';
        }
    }


    cout<<executions<<" instructions executed.\n";
    return 0;
}