#include<bits/stdc++.h>
#define STR str##ing
#define CMDNUM 19
#define MAXMEMORY 20000
using namespace std;

struct instruction
{
    STR cmd;
    int line,curPC;
    vector<STR>arg;
};
vector<instruction>code;
STR command[CMDNUM]= {"LD", "LDi", "SD","SDi", "ADD", "ADDi", "SUB", "SUBi", "MUL", "MULi", "DIV", "DIVi", "JMP", "JZ", "JNZ", "JGZ", "JGEZ", "JLZ", "JLEZ"};//19
int numCommand[CMDNUM]= {   2,     2,    2,    2,     3,      3,     3,      3,     3,      3,     3,      3,     1,    2,     2,     2,      2,     2,      2};
map<STR,int>numArg;
map<STR,int>jmpInd;
vector<STR>fileIn;
vector<int>lineNum;
int PC,_PC,MEM;
/***************************************************************/
//Different type checker
//@param char c
bool isSpace(char c)
{
    if(c==' '||c=='\t')
        return true;
    return false;
}
bool isNumber(char c)
{
    if(c>'9'||c<'0')return false;
    return true;
}
//@param STR str
bool isNumber(STR str)
{
    if(str[0]!='-'&&str[0]>'9'&&str[0]<'0')
        return false;
    for(int i=1; i<str.size(); i++)
        if(!isNumber(str[i]))return false;
    return true;
}
//@param STR str
bool isMemory(STR str)
{
    return isNumber(str);
}
bool isRegister(STR str)
{
    if(str[0]!='R')
        return false;
    for(int i=1; i<str.size(); i++)
        if(str[i]>'9'||str[i]<'0')
            return false;
    return true;
}
bool inInd(STR str)
{
    if(str[0]!='['||str[str.size()-1]!=']')
        return false;
    str.erase(str.begin());
    str.erase(str.end()-1);
    cout<<str<<" "<<jmpInd.count(str)<<endl;
    if(jmpInd.count(str)>0)
        return true;
    return false;
}
/*********************************************************/
//Initialize and Debug
void buildMap()
{
    for(int i=0; i<18; i++)
    {
        numArg[command[i]]=numCommand[i];
    }
}
void printFileIn(STR s) //debug
{
    cout<<s<<endl;
    for(int i=0; i<fileIn.size(); i++)
        cout<<lineNum[i]<<" "<<fileIn[i]<<endl;
    cout<<"********************************************"<<endl;
}
/**********************************************************/
//Get input and remove unecessary stuff.
void removeComment()
{
    for(int i=0; i<fileIn.size(); i++)
    {
        int j;
        for(j=0; j<fileIn[i].size(); j++) if(fileIn[i][j]=='#') break;
        fileIn[i]=fileIn[i].substr(0,j);
    }//remove comments
    for(int i=0; i<fileIn.size(); i++)
    {
        bool spaceOnly = true;
        for(int j=0; j<fileIn[i].size(); j++)
        {
            if(!isSpace(fileIn[i][j]))
            {
                spaceOnly=false;
                break;
            }
        }
        if(spaceOnly)
        {
            fileIn.erase(fileIn.begin()+i);
            lineNum.erase(lineNum.begin()+i--);
        }
    }//check for empty line after comments have been deleted
}
int getNum(STR str)
{
    int st=0,ed;
    while(str[st]>'9'||str[st]<'0') st++;
    ed=st;
    while(str[ed]<='9'&&str[ed]>='0') ed++;
    int num=0;
    for(int i=st; i<ed; i++)
    {
        int a=num*10;
        int b=(int)str[i]-'0';
        num=a+b;

    }
    return num;
}
void getCodeData()
{
    for(int i=0; i<fileIn.size(); i++)
    {
        //cout<<i<<endl;
        if(fileIn[i][fileIn[i].size()-2]==':')
        {
            fileIn[i].erase(fileIn[i].begin()+fileIn[i].size()-1);
        }
        int  c = fileIn[i].find("Code:");
        if(c!=-1)
        {
            //cout<<"IN C"<<endl;
            int edAt=c+5;
            PC = getNum(fileIn[i].substr(edAt,fileIn[i].size()-edAt));
            //cout<<"GOTPC"<<endl;
            fileIn.erase(fileIn.begin()+i);
            lineNum.erase(lineNum.begin()+i--);
            //cout<<"FINISHED"<<endl;
            _PC=PC;
        }
        else{
        int m = fileIn[i].find("Data:");
        //cout<<"Found"<<endl;
        if(m!=-1)
        {
            //cout<<"IN M"<<endl;
            int edAt=c+5;
            MEM = getNum(fileIn[i].substr(edAt,fileIn[i].size()-edAt));
            fileIn.erase(fileIn.begin()+i);
            lineNum.erase(lineNum.begin()+i--);
        }
        }
    }
    //cout<<PC<<" "<<MEM<<endl;
}
void removeSpace()
{
    for(int i=0; i<fileIn.size(); i++)
    {
        bool after=false;
        for(int j=0; j<fileIn[i].size(); j++)
        {
            if(fileIn[i][j]!=' '&&fileIn[i][j]!='\t')after=true;
            if((after&&fileIn[i][j]==' ')||(after&&fileIn[i][j]=='\t'))
            {
                fileIn[i][j]=',';
                break;
            }
        }
    }
    for(int i=0; i<fileIn.size(); i++)
        for(int j=0; j<fileIn[i].size(); j++)
            if(fileIn[i][j]==' '||fileIn[i][j]=='\t')
            {
                //cout<<"Removed: "<<i<<" "<<j<<endl;
                fileIn[i].erase(fileIn[i].begin()+j--);
                //fileIn[i].erase(j--,1);
            }
}
/*************************************************/
STR LE(int num)
{
    int nSize=0;
    while(num/pow(10,nSize)>0)nSize++;
    if(num<0)nSize++;
    char temp[nSize]="";
    //cout<<"In LE function"<<endl;
    itoa(num,temp,10);
    //cout<<"AFTER ITOA"<<endl;
    STR result=temp;
    //cout<<result<<endl;
    return ("Error on line "+result+": ");
}
bool sizeError(instruction cur)
{
    //cout<<cur.arg.size()<<" "<<cur.cmd<<" "<<numArg[cur.cmd]<<endl;
    if(cur.arg.size()>numArg[cur.cmd])
    {
        cout<<LE(cur.line)<<"Extra data after instruction"<<endl;
        return 1;
    }
    if(cur.arg.size()<numArg[cur.cmd])
    {
        cout<<LE(cur.line)<<"Missing OPERAND"<<endl;
        return 1;
    }
    return 0;
}
/********************************************************/
//get***Error functions
bool getLDError(instruction cur)
{
    if(sizeError(cur))return 1;
    if(!isRegister(cur.arg[0])&&!isMemory(cur.arg[0]))
    {
        cout<<LE(cur.line)<<"Expected Memory Location or Register for first OPERAND; saw \""<<cur.arg[0]<<"\""<<endl;
        return 1;
    }
    if(!isRegister(cur.arg[1]))
    {
        cout<<LE(cur.line)<<"Expected Register for second OPERAND; saw \""<<cur.arg[1]<<"\""<<endl;
        return 1;
    }
    return 0;
}
bool getLDiError(instruction cur)
{
    if(sizeError(cur))return 1;
    if(!isNumber(cur.arg[0]))
    {
        cout<<LE(cur.line)<<"Expected Number for first OPERAND; saw \""<<cur.arg[0]<<"\""<<endl;
        return 1;
    }
    if(!isRegister(cur.arg[1]))
    {
        cout<<LE(cur.line)<<"Expected Register for second OPERAND; saw \""<<cur.arg[1]<<"\""<<endl;
        return 1;
    }
    return 0;
}
bool getSDError (instruction cur)
{
    if(sizeError(cur))return 1;
    if(!isRegister(cur.arg[0]))
    {
        cout<<LE(cur.line)<<"Expected Register for first OPERAND; saw \""<<cur.arg[0]<<"\""<<endl;
        return 1;
    }
    if(!isMemory(cur.arg[1])&&!isRegister(cur.arg[1]))
    {
        cout<<LE(cur.line)<<"Expected Memory or Register for second OPERAND; saw \""<<cur.arg[1]<<"\""<<endl;
        return 1;
    }
    return 0;
}
bool getSDiError (instruction cur)
{
    if(sizeError(cur))return 1;
    if(!isNumber(cur.arg[0]))
    {
        cout<<LE(cur.line)<<"Expected Number for first OPERAND; saw \""<<cur.arg[0]<<"\""<<endl;
        return 1;
    }
    if(!isMemory(cur.arg[1])&&!isRegister(cur.arg[1]))
    {
        cout<<LE(cur.line)<<"Expected Memory or Register for second OPERAND; saw \""<<cur.arg[1]<<"\""<<endl;
        return 1;
    }
    return 0;
}
bool getSignError (instruction cur)
{
    if(sizeError(cur))return 1;
    //cout<<"After size error"<<endl;
    if(!isRegister(cur.arg[0]))
    {
        cout<<LE(cur.line)<<"Expected Register for first OPERAND; saw \""<<cur.arg[0]<<"\""<<endl;
        return 1;
    }
    if(!isRegister(cur.arg[1]))
    {
        cout<<LE(cur.line)<<"Expected Register for second OPERAND; saw \""<<cur.arg[1]<<"\""<<endl;
        return 1;
    }
    if(!isRegister(cur.arg[2]))
    {
        cout<<LE(cur.line)<<"Expected Register for first OPERAND; saw \""<<cur.arg[2]<<"\""<<endl;
        return 1;
    }
    return 0;
}
bool getSigniError (instruction cur)
{
    if(sizeError(cur))return 1;
    if(!isRegister(cur.arg[0]))
    {
        cout<<LE(cur.line)<<"Expected Register for first OPERAND; saw \""<<cur.arg[0]<<"\""<<endl;
        return 1;
    }
    if(!isNumber(cur.arg[1]))
    {
        cout<<LE(cur.line)<<"Expected Number for second OPERAND; saw \""<<cur.arg[1]<<"\""<<endl;
        return 1;
    }
    if(!isRegister(cur.arg[2]))
    {
        cout<<LE(cur.line)<<"Expectee Register for third OPERAND; saw \""<<cur.arg[2]<<"\""<<endl;
        return 1;
    }
    return 0;
}
bool getJMPError (instruction cur)
{
    if(sizeError(cur))return 1;
    if(!isMemory(cur.arg[0])&&!inInd(cur.arg[0]))
    {
        cout<<LE(cur.line)<<"Expected Memory for first OPERAND; saw \""<<cur.arg[0]<<"\""<<endl;
        return 1;
    }
    return 0;
}
bool getJXXError (instruction cur)
{
    if(sizeError(cur))return 1;
    if(!isRegister(cur.arg[0]))
    {
        cout<<cur.arg[0]<<"|"<<endl;
        cout<<LE(cur.line)<<"Expected Register for first OPERAND; saw \""<<cur.arg[0]<<"\""<<endl;
        return 1;
    }
    if(!isMemory(cur.arg[1])&&!inInd(cur.arg[1]))
    {
        cout<<LE(cur.line)<<"Expected Memory for second OPERAND; saw \""<<cur.arg[1]<<"\""<<endl;
        return 1;
    }
    return 0;
}
bool getError(instruction cur)
{

    if(cur.cmd=="LD") return getLDError(cur);
    if(cur.cmd=="LDi") return getLDiError(cur);
    if(cur.cmd=="SD") return getSDError(cur);
    if(cur.cmd=="SDi") return getSDiError(cur);
    if(cur.cmd=="ADD"||cur.cmd=="SUB"||cur.cmd=="MUL"||cur.cmd=="DIV")return getSignError(cur);
    if(cur.cmd=="ADDi"||cur.cmd=="SUBi"||cur.cmd=="MULi"||cur.cmd=="DIVi")return getSigniError(cur);
    if(cur.cmd=="JMP")return getJMPError(cur);
    if(cur.cmd=="JZ"||cur.cmd=="JNZ"||cur.cmd=="JGZ"||cur.cmd=="JGEZ"||cur.cmd=="JLZ"||cur.cmd=="JLEZ")return getJXXError(cur);
    if(numArg.find(cur.cmd)==numArg.end())
    {
        cout<<"Error on line "<<cur.line<<": Instruction not found"<<endl;
        return 1;
    }
    return 0;
}
//general getContent function
bool getContent()
{
    instruction temp;
    for(int i=0; i<fileIn.size(); i++)
    {
        temp.line=lineNum[i];
        temp.curPC=_PC++;
        int comma=0;
        for(int j=0;j<fileIn[i].size();j++) if(fileIn[i][j]==',')comma++;
        if(fileIn[i][fileIn[i].size()-1]==':'&&comma<=1)
        {
            fileIn[i].erase(fileIn[i].begin()+fileIn[i].size()-1);
            cout<<"                                        JMPIND: "<<fileIn[i]<<endl;
            jmpInd[fileIn[i]]=temp.curPC;
            fileIn[i]+=':';
            temp.cmd=fileIn[i];
            continue;
        }
        for(int st=0; st<fileIn[i].size(); st++)
            for(int ed=st; ed<fileIn[i].size(); ed++)
            {
                if(fileIn[i][ed]==','||ed==fileIn[i].size()-1)
                {
                    if(st==0)
                        temp.cmd=fileIn[i].substr(0,ed);
                    else if(fileIn[i][ed]==',')
                        temp.arg.push_back(fileIn[i].substr(st,ed-st));
                    else temp.arg.push_back(fileIn[i].substr(st,ed-st+1));
                    st=ed;
                    ed=st;
                    break;
                }
            }
        code.push_back(temp);
        //cout<<"before: "<<i<<endl;
        if(getError(temp))return 1;
        //cout<<i<<endl;
        temp.arg.clear();
    }
    return false;
}
bool getFile()
{
    //TODO need to change this back to allow user input file name;
    STR fileName="loopAddition.asm";
    //getline(cin,fileName);
    ifstream fin;
    fin.open(fileName.c_str());
    STR curLine;
    getline(fin,curLine);
    while(fin)
    {
        fileIn.push_back(curLine);
        lineNum.push_back(fileIn.size());
        getline(fin,curLine);
    }
    //printFileIn("Right After Input");
    removeComment();
    //printFileIn("After removeComment");
    getCodeData();
    //printFileIn("After getCodeData");
    removeSpace();
    //printFileIn("After removeSpace");
    for(int i=0; i<fileIn.size(); i++)
        cout<<fileIn[i]<<endl;
    return getContent();
    //printInput();
    fin.close();
}
//test input
void printInput()
{
    cout<<"pinrtInput:"<<code.size()<<endl;
    for(int i=0; i<code.size(); i++)
    {
        //cout<<code[i].arg.size()<<" ";
        cout<<code[i].line<<" ";
        cout<<code[i].cmd;
        for(int j=0; j<code[i].arg.size(); j++)
        {
            cout<<" "<<code[i].arg[j];
        }
        cout<<endl;
    }
}
//main
int main()
{
    buildMap();
    if(getFile())   return 0;//end if an error is found
    printInput();
    int LS=0,ALU=0,CJ=0;
    for(int i=0; i<code.size(); i++)
    {
        if(code[i].cmd=="LD"||code[i].cmd=="LDi"||code[i].cmd=="SD"||code[i].cmd=="SDi")
            LS++;
        if(code[i].cmd=="ADD"||code[i].cmd=="ADDi"||code[i].cmd=="SUB"||code[i].cmd=="SUBi"||code[i].cmd=="MUL"||code[i].cmd=="MULi"||code[i].cmd=="DIV"||code[i].cmd=="DIVi")
            ALU++;
        if(code[i].cmd[0]=='J')
            CJ++;
    }
    cout<<"Total number of assembly instructions: "<<LS+ALU+CJ<<endl;
    cout<<"Number of Load/Store: "<<LS<<endl;
    cout<<"Number of ALU: "<<ALU<<endl;
    cout<<"Number of Compare/Jump: "<<CJ<<endl;
}
