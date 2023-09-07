
/*****************************************************************************
TITLE: ASSEMBLER																														
AUTHOR: CHIRANJIBI PRADHAN
ROLLNO: 2101CS21
Declaration of Authorship
This cpp file asm.cpp, is part of the miniproject of CS209/CS210 at the 
department of Computer Science and Engg, IIT Patna . 
*****************************************************************************/

#include <bits/stdc++.h>
using namespace std ; 

string filename ;
string name ;




vector<string> warning   ; 
vector<string> errors  ;   //stores errors
map<string,int> label_address ;         //stores PC where labels were declared
vector<string> oginputlines ;     //stores original input line
vector<vector<string>> first_pass_lines ;  // separate the space separated words 
vector<pair<int,long long>> second_pass_lines  ; // {opcode,operand} after 2nd pass


map<string,pair<int,bool>>Mnemonics{ // Table for Mnemonics in the form {name, {opcode, num of operands}}
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
    {"HALT",{18,0}},
    {"data",{0,1}},
    
};


long no_opc=-1e5  ;

bool valid_label(string s){            //checks if a label name is proper according to convention or not 
    
    if(!(s[0]<='z'&&s[0]>='a')&&!(s[0]<='Z'&&s[0]>='A')&&s[0]!='_') return false;

    for(auto c:s){
        if(!(c<='z'&&c>='a')&&!(c<='Z'&&c>='A')&&c!='_'&&!(c<='9'&&c>='0'))return false;
    }
   
    if(s.size()==0)return false;


    return true;
}


int check_operand(int line,string s)  // Checks if an operand is number in proper format or number
{                  
   
    if(label_address.find(s)!=label_address.end())return label_address[s]; // if label already exists

    const char* s1=s.c_str();
    char* end;
    long n;

    if(s[0]=='0'&&s.size()>1){ // For parsing hex / oct / bin numbers
      
        if(s[1]=='x')n=strtol(s1+2,&end,16); //hex
       
        else if(s[1]=='b')n=strtol(s1+2,&end,2); //bin
       
        else n=strtol(s1,&end,8); //octal
    }

    else n=strtol(s1,&end,10);

    if(end-s1!=(int)s.size()){ // If the entire string did not get converted to a decimal number.
        
      
        
    if(s[0]<='9'&&s[0]>='0')errors.push_back("Invalid number format : "+ s +" at line number "+to_string(line));
    
      else if(valid_label(s)){
        errors.push_back("No such label "+s +" at line number" + to_string(line));
    }

        else errors.push_back("Invalid expression at line number "+ to_string(line));
    }
    return n;

}


string toHex(int num, int s){ // Converts num into a hex string of s characters.
    
    stringstream ss;
    
    ss<<hex<<num;
    
    string ans(ss.str());
    
    ans=string(max(0,s-(int)ans.size()),'0')+ans;
    
    if((int)ans.size()>s)
    {
    ans=ans.substr((int)ans.size()-s,s);
    }
    return ans;
}
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
void first_pass()
{
  ifstream infile(filename) ;   
  string file_line ; //stores file line in orginal file

  int file_line_no =0 ; //stores input line number 
  int instr_line =0 ;  //stores program line number

  while (getline(infile,file_line)) //read each line into file_line
  {
    file_line_no++ ;
    oginputlines.push_back(file_line) ;
   
    
    
    // Now we need to separate each line using the delimiter of white space ; 
     istringstream wordstream(file_line);

     string seprated  ;
     vector<string> this_line ;
     long long semi =-1 ;
     long long separator =0 ;
   

     string label_this_line ="" ;
     long long label_done=0 ;

     while (wordstream>>seprated) // separate each line by words
     {   
         separator+=1 ;
         
         
         for (int i =0 ; i < (int)seprated.size() ;i++)
         {
            if (seprated[i] ==';') //if semicolon  is found we wont look after any word after that
            {
            
              seprated=seprated.substr(0,i) ;
            
              semi=1 ;
              break ;
            }
         }

         for (int i =0 ; i<(int)seprated.size();i++)
         {
            if (seprated[i]==':' ) // is colon is found it must be label 
            {
          
              if (separator==1) // label: is the fist seprator
              {
              
                label_this_line = seprated.substr(0,i) ;
                seprated =seprated.substr(i+1,seprated.size()-i) ;
              }
              
              else {
                errors.push_back("Incorrect way of  label declaration at line " + to_string(file_line_no)) ;
              }


            }
         }
       
         
         if (label_this_line!="" && label_done==0) //if label is found
         {
             if (!valid_label(label_this_line))
             {
              
              errors.push_back("Invalid label name declared at line "+ to_string(file_line_no)) ;
             }
             else{
                 if (label_address.find(label_this_line)==label_address.end())
                 {
              
                  label_address[label_this_line]=instr_line;
                 }
                 else {
                 
                  errors.push_back("Duplicate label found at "+ to_string(file_line_no)) ;
                 }
             }
             label_done=1 ;
         }
           
         if (seprated.size()!=0) this_line.push_back(seprated) ;

         if (semi==1) {break ;}

     }

    
     first_pass_lines.push_back(this_line) ;// put the proccessed line
     
     if (this_line.size()!=0) instr_line ++ ;


  }
   
   return ;
}


void second_pass()
{

  for (int line_n =0 ; line_n <(int)first_pass_lines.size()  ;line_n ++ )
  {
    vector<string> line = first_pass_lines[line_n] ;
    int actual_in_separated = line.size() ;
    
    if (actual_in_separated==0){
      second_pass_lines.push_back({no_opc,0}) ;
       continue; 
    } 
    int z=0 ;
    
    
    if (Mnemonics.find(line[0])==Mnemonics.end())
    {
         
         errors.push_back("The mnemonic " +line[0] + " found at line number " +to_string(line_n) +" is not in SIMPLE ISA") ;

        continue;
    }
    int req_separated = 1+ Mnemonics[line[0]].second ; //1 (instr itself) + (no of operands of the type of the instruction)
    
    if (actual_in_separated < req_separated+z) {
      errors.push_back("Operands missing at line " + to_string(line_n+1)) ;
    }


    if (actual_in_separated+z> req_separated)
    {
      errors.push_back("Extra operands found at line "+ to_string(line_n+1))  ;
    }


    pair<int,long long > this_line_op  ; //opcode, operand 

  

    

      this_line_op.first = Mnemonics[line[0]].first ;  //opcode

      if (Mnemonics[line[0]].second==1) // 1 operand
       {    
          if (actual_in_separated==2)this_line_op.second = check_operand(line_n , line[1]) ;
          else this_line_op.second=check_operand(line_n,"0") ;
       }
      else { //0 operand 
          this_line_op.second = z ;

      }
    
   

    if (line[0]=="data")
    {
      
      this_line_op.first =this_line_op.second&0xff ;

      this_line_op.second =this_line_op.second>>8 ;
    }


    second_pass_lines.push_back(this_line_op) ;
  }

}

void print_errors()
{
  ofstream logfile(name + ".log");
  for (auto error :errors)
  {
    logfile <<error <<endl ;
  }
  return ;
}

void create_listing_file()
{
  long pc =0 ;

  ofstream listing(name + ".l");
  

  for  (int input_ln =0 ; input_ln <(long)oginputlines.size() ;input_ln++)
   {
        listing<<toHex(pc,8)<<' ' ;
        if (second_pass_lines[input_ln].first==no_opc) 
        {
           listing<<oginputlines[input_ln]<<endl  ; 
           continue;
        } //if there is no opcode 

        
        int opcode =second_pass_lines[input_ln].first ;

        int operand =second_pass_lines[input_ln].second  ;


        if(opcode==13||opcode==15||opcode==16||opcode==17)
        {
          operand-=pc+1; // for these opcode , operand is the offset not the values ;

        }
        int powe =binpow(2,4) ;
        listing<<toHex(operand,6) ;

        listing<<toHex(opcode,2)<<" " ;
        

        
        pc++ ;
        
        listing<<oginputlines[input_ln]<<" " ;

        listing<<endl ;

   }

  return ;
}

void create_bin_file() // this functin creates the binaryfile ;
{
  long long  pc =0  ;
   ofstream binfile(name +".o",ios::out|ios::binary);
   for  (int input_ln =0 ; input_ln <(long)oginputlines.size() ;input_ln++)
   {

      if (second_pass_lines[input_ln].first!=no_opc) 
      {
        
        long long opcode =second_pass_lines[input_ln].first ;
        long long operand =second_pass_lines[input_ln].second  ;

        int pow8 =binpow(2,8) ;
        
        if(opcode==13||opcode==15||opcode==16||opcode==17)
        {
          operand-=pc+1; // for these opcode , operand is the offset not the values ;

        }
        int pow16=binpow(2,16) ;
        char bin_instr[4]={(char)opcode,(char)(operand&0xff),(char)((operand&0xff00)>>8),(char)((operand&0xff0000)>>16)};
        
         
        
        binfile.write(bin_instr,4);
         
         pc++ ;
        }
  }
  return ;
}



int main ()
{
  
  cout <<"Enter file name (including .asm )" <<endl ;

  cin >> filename  ;
  ifstream infile(filename) ;          

  name = filename.substr(0, filename.find('.')) ;
 
  
  
 
  first_pass() ; // execute first pass

  second_pass() ; //execute second pass

  


  
 
  if (errors.size()!=0)
  {
    cout <<"Compilation unsuccesful . Some Errors and found and mention in the logfile" <<endl ;
    print_errors()  ;

    return 0 ;
  }
   ofstream logfile(name + ".log");
   cout <<"Compilation succesful.Open the listing file "  <<endl ;
    
   create_listing_file() ; // write in the listing file
   create_bin_file() ;    // write in the binary file
   

  return 0 ;     
}