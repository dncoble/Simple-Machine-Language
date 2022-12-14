/*
 * On our honor, we have neither given nor received unuauthorized assistance on this assignment.
 * Author: Wolfgang Buchmaier, Daniel Coble
 * Wolfgang Buchmaier: main & parseCommand & returnvarlocation
 * Daniel Coble: parseExpression
 */
#include <iostream>
#include <string>
#include <iomanip>
#include <array>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <cctype>
#include <algorithm>
#include "SML.h"
using namespace std;

bool isVariable(string element){
  bool out = false;
  if(element == "a" || element == "b" || element == "c" || element == "d" || element == "e" || element == "f" || element == "g" || element == "h" || element == "i" || element == "j" || element == "k" || element == "l" || element == "m" || element == "n" || element == "o" || element == "p" || element == "q" || element == "r" || element == "s" || element == "t" || element == "u" || element == "v" || element == "w" || element == "x" || element == "y" || element == "z"){
    out = true;
  }
  return out;
}
/* isNumber also includes operators and comparators? */
bool isNumber(string element){
  bool out = false;
  if(isdigit(element[0])){
    bool out2 = true;
    if(element.size() > 4){out2= false;}
    for(char ch : element){
      if(!isdigit(ch)){out2 = false;}
    }
    out = out2;
  }
  return out;
}

bool isOperator(string element) {
  if(element == "+" || element == "-" || element == "*" || element == "/" || element == "(" || element == ")") {return true;}
  return false;
}

bool isComparator(string element) {
  if(element == "==" || element == "<" || element == ">") {
    return true;
  }
  return false;
}

bool isCommand(string element){
  bool out = false;
  if(element == "if" || element == "goto" || element == "let" || element == "input" || element == "print" || element == "end" || element == "rem"){out = true;}
  return out;
}

string helpBetterCheckSyntax(vector<string> line) {
  if(!isCommand(line[1])) {return line[0];}
  else if(line[1] == "rem") {
    //pass
  }
  else if(line[1] == "let") {
    if(!isVariable(line[2])) {return line[0];}
    if(line[3] != "=") {return line[0];}
    for(int i = 4; i < line.size(); i ++) {
      if(!isVariable(line[i]) && !isNumber(line[i]) && !isOperator(line[i])) {return line[0];}
    }
  }
  else if(line[1] == "goto") {
    if(line.size() != 3){return line[0];}
    else if(!isNumber(line[2])) {return line[0];}
  }
  else if(line[1] == "print") {
    for(int i = 2; i < line.size(); i ++) {
      if(!isVariable(line[i]) && !isNumber(line[i]) && !isOperator(line[i])) {
        return line[0];
      }
    }
  }
  else if(line[1] == "input") {
    if(line.size() != 3) {return line[0];}
    else if(!isVariable(line[2])) {return line[0];}
  }
  else if(line[1] == "end") {
    if(line.size() != 2) {return line[0];}
  }
  else if(line[1] == "if") {
    if(!isVariable(line[2]) && !isNumber(line[2]) && !isOperator(line[2])) {return line[0];}
    int k = 3;
    while(isVariable(line[k]) || isNumber(line[k]) || isOperator(line[k])) {k ++;}
    if(!isComparator(line[k])) {return line[0];}
    k++;
    if(!isVariable(line[k]) && !isNumber(line[k]) && !isOperator(line[k])) {return line[0];}
    while(isVariable(line[k]) || isNumber(line[k]) || isOperator(line[k])) {k ++;}
    vector<string> split (0);
    split.push_back(line[0]);
    while(k < line.size()) {split.push_back(line[k]); k++;}
    return helpBetterCheckSyntax(split);
  }
  return "-1";
}

string betterCheckSyntax(vector<vector<string>> input) {
  for(vector<string> line : input) {
    string error = helpBetterCheckSyntax(line);
    if (error != "-1") {return error;}
  }
  return "-1";
}

/*this function has two functions. 
 * 1: if given a varname which is not in the variablelist, it creates
 * it, allocating a spot for it in memory.
 * 2: if given a varname which is in variablelist, it returns its
 * location.
 * The first is used on the first run-through, which identifies
 * all variables and literals in the code.
 * The second is used in the second (compiling) run-through, to get
 * variable locations. */
string returnvarlocation(string varname, vector<array<string, 2>>&variablelist){
  if(variablelist.size()>0){
    string simpleloc = "";
    for(array<string, 2> pair : variablelist){
      if(pair[1]==varname){
        simpleloc = pair[0];
      }
    }
    if(simpleloc!=""){
      return simpleloc;
    }else{
      int locoflastvariable; istringstream(variablelist[variablelist.size()-1][0]) >> locoflastvariable; 
      if(locoflastvariable<10){
        variablelist.push_back( {"0"+to_string(locoflastvariable-1), varname} ); 
      }else{
        variablelist.push_back( {to_string(locoflastvariable-1), varname} );
      }
      
      return to_string(variablelist.size()-1);
    }
  }else{
    variablelist.push_back({"99",  varname});
    return returnvarlocation(varname, variablelist);
  }
}

/* a helper method for parseExpression */
int precedence(string c) {
  if(c == "^") {return 3;} //^ is never used, but produces no errors
  else if (c == "*" || c == "/") {return 2;}
  else if (c == "+" || c == "-") {return 1;}
  else {return -1;}
}

/* the compiler may be given any complicated expression, such as 
 * (x - 4) / (y - 5 * w) and must parse it. this function first
 * changes that expression to post-order, then inputs the code for
 * evaluating that expression into the linelist, and returns the
 * final location of the parsed expression. along the way it might
 * need to use "temporary variables", which are free spots in memory,
 * but are not added to the variable list, and are allowed to be
 * overwritten after parseExpression is run. the returned value
 * will be a temporary variable location in memory where the parsed
 * expression will be stored.*/
string parseExpression(vector<string> infix, string simpleline, int memstart, vector<array<string, 2>>& variablelist, vector<array<string, 2>>&linelist){
  //if infix is just one term, variable or literal, then we just return the value of that location
  if(infix.size() == 1) {return returnvarlocation(infix[0], variablelist);}
  //change expression to the postfix notation string postfix.
  vector<string> stack;
  int l = infix.size();
  string postfix;
  for(int i = 0; i < l; i++) {
    if(precedence(infix[i]) == -1 && infix[i] != "(" && infix[i] != ")"){
      postfix += infix[i] + " ";
    }
    else if(infix[i] == "(") {
      stack.insert(stack.begin(), "(");    }
    else if(infix[i] == ")") {
      while(stack.size() != 0 && stack[0] != "(") { 
        string c = stack[0]; 
        stack.erase(stack.begin());
        postfix += c + " "; 
      } 
      if(stack[0] == "(") { 
        string c = stack[0];
        stack.erase(stack.begin());
      }
    }
    else{
      while(stack.size() != 0 && precedence(infix[i]) <= precedence(stack[0])) {
        string c = stack[0];
        stack.erase(stack.begin());
        postfix += c + " "; 
      } 
      stack.insert(stack.begin(), infix[i]); 
    } 
  }
  while(stack.size() != 0) { 
    string c = stack[0]; 
    stack.erase(stack.begin());
    postfix += c + " "; 
  }
  //there may be some number of spaces at the end of postfix
  while(postfix.at(postfix.size() - 1) == ' ') {
    postfix = postfix.substr(0, postfix.size() - 1);
  }
  /*breaking the string back into a vector<string>. suboptimal
   * but it gets the job done.*/
  vector<string> postfixed;
  string word = "";
  for(char c : postfix) {
    if(c != ' ') {
      word += c;
    }
    else {
      postfixed.push_back(word);
      word = "";
    }
  }
  postfixed.push_back(word);
  //vector of machine lines that will be added to linelist
  vector<string> machinelines;
  int memcurrent = memstart;
  while(postfixed.size() > 1) {
    int i = 0;
    while(precedence(postfixed[i]) == -1) {i++;}
    string var1 = postfixed[i - 2];
    string var2 = postfixed[i - 1];
    string var1loc, var2loc;
    if(var1.at(0) == 'X') {var1loc = var1.substr(1);}
    else {var1loc = returnvarlocation(var1, variablelist);}
    if(var2.at(0) == 'X') {var2loc = var2.substr(1);}
    else {var2loc = returnvarlocation(var2, variablelist);}
    if(var1loc.size() == 1) {var1loc = "0" + var1loc;}
    if(var2loc.size() == 1) {var2loc = "0" + var2loc;}
    machinelines.push_back("20" + var1loc);
    if(postfixed[i] == "+") {machinelines.push_back("30" + var2loc);}
    else if(postfixed[i] == "-") {machinelines.push_back("31" + var2loc);}
    else if(postfixed[i] == "/") {machinelines.push_back("32" + var2loc);}
    else if(postfixed[i] == "*") {machinelines.push_back("33" + var2loc);}
    string smemcurrent = to_string(memcurrent);
    if(smemcurrent.size() == 1) {smemcurrent = "0" + smemcurrent;}
    machinelines.push_back("21" + to_string(memcurrent));
    postfixed.erase(postfixed.begin() + i - 2);
    postfixed.erase(postfixed.begin() + i - 2);
    postfixed.erase(postfixed.begin() + i - 2);
    postfixed.insert(postfixed.begin() + i - 2, "X" + to_string(memcurrent));
    memcurrent --;
  }
  //append machinelines onto linelist
  for(string i : machinelines) {
    linelist.push_back({simpleline, i});
  }
  return postfixed[0].substr(1);
}

/* takes in a paresed line with simpleline number cut out and spits
 * out corresponding machine code.
 * variable list is {(locationinfinalcode, variable name), (..)
 * literals (numbers) are interpreted as final variables. e.g. 
 * they will be named "5" and store the value of 5 in memory.
 * linelist is {(simplelinenumber, 2215), (...)} */
void parseCommand(string simpleline, int memstart, vector<string> parsedline, vector<array<string, 2>>& variablelist, vector<array<string, 2>>&linelist) {
  //parsedline[0] is the command. parsing each command is different.
  int tempmemstart = memstart;
  if(parsedline[0] == "if"){
    //find operator
    int operatorlocation;
    int iftruelocation;
    int i = 0;
    for(string element : parsedline){
      if(element == "==" || element =="<" || element ==">"){operatorlocation=i;}
      else if(i>0 && (element == "if" || element == "goto" || element == "let" || element == "input" || element == "print" || element == "end"/* || element == "rem"*/)){iftruelocation=i; break;}
      i++;
    }
    //further parse the parsedline vector into if statement components
    vector<string> firsthalf (parsedline.begin()+1, parsedline.begin()+operatorlocation);
    string compare1 = parseExpression(firsthalf, simpleline, tempmemstart, variablelist, linelist);
    if(stoi(compare1)<=memstart){tempmemstart=stoi(compare1)-1;}
    string compareoperator = parsedline[operatorlocation];
    vector<string> lasthalf (parsedline.begin()+operatorlocation+1, parsedline.begin()+iftruelocation);
    string compare2 = parseExpression(lasthalf, simpleline, tempmemstart, variablelist, linelist);
    //check for numbers to add to variable list
    vector<string> parsediftrue (parsedline.begin()+iftruelocation, parsedline.end());
    //if elvaluation
    if (compareoperator == "=="){//3 cases
      linelist.push_back({simpleline, "20"+compare1});//put in first half
      linelist.push_back({simpleline, "31"+compare2});//subtract by second half
      int branchcommandlocation = linelist.size();
      linelist.push_back({simpleline, "41"});//skip code if comepare2>compare1
      linelist.push_back({simpleline, "20"+compare2});//put in first half
      linelist.push_back({simpleline, "31"+compare1});//subtract by second half
      linelist.push_back({simpleline, "41"});//skip code if comepare1>compare2
      /* if may contain another command, such as let, goto, or even
       * another if as the result of the if being evaluated true.
       * handling this requires recursively calling parseCommand
       * again. */
      parseCommand(simpleline, tempmemstart, parsediftrue, variablelist, linelist);
      int endbranchlocation = linelist.size();
      if(endbranchlocation<10){
        linelist[branchcommandlocation][1] += "0"+to_string(endbranchlocation);
        linelist[branchcommandlocation+3][1] += "0"+to_string(endbranchlocation);
      }else{
        linelist[branchcommandlocation][1] += to_string(endbranchlocation);
        linelist[branchcommandlocation+3][1] += to_string(endbranchlocation);
      }
    }else if(compareoperator == "<"){
      linelist.push_back({simpleline, "20"+compare2});//put in second half
      linelist.push_back({simpleline, "31"+compare1});//subtract by first half
      int branchcommandlocation = linelist.size();
      linelist.push_back({simpleline, "41"});//skip code if comepare1>compare2
      linelist.push_back({simpleline, "42"});//skip code if comepare1=compare2
      parseCommand(simpleline, tempmemstart, parsediftrue, variablelist, linelist);
      int endbranchlocation = linelist.size();
      if(endbranchlocation<10){
        linelist[branchcommandlocation][1] += "0"+to_string(endbranchlocation);
        linelist[branchcommandlocation+1][1] += "0"+to_string(endbranchlocation);
      }
      else{
        linelist[branchcommandlocation][1] += to_string(endbranchlocation);
        linelist[branchcommandlocation+1][1] += to_string(endbranchlocation);
      }
    }else if(compareoperator == ">"){
      linelist.push_back({simpleline, "20"+compare1});//put in first half
      linelist.push_back({simpleline, "31"+compare2});//subtract by second half
      int branchcommandlocation = linelist.size();
      linelist.push_back({simpleline, "41"});//skip code if compare1<comepare2
      linelist.push_back({simpleline, "42"});//skip code if comepare1=compare2
      parseCommand(simpleline, tempmemstart, parsediftrue, variablelist, linelist);
      int endbranchlocation = linelist.size();
      if(endbranchlocation<10){
        linelist[branchcommandlocation][1] += "0"+to_string(endbranchlocation);
        linelist[branchcommandlocation+1][1] += "0"+to_string(endbranchlocation);
      }
      else{
        linelist[branchcommandlocation][1] += to_string(endbranchlocation);
        linelist[branchcommandlocation+1][1] += to_string(endbranchlocation);
      }
    }
  }else if(parsedline[0] == "let"){
    string location;
    for(array<string, 2> pair : variablelist){
      if(pair[1] == parsedline[1]){location = pair[0]; break;}
    }
    vector<string> expression (parsedline.begin()+3, parsedline.end());
    linelist.push_back({simpleline, "20"+parseExpression(expression, simpleline, tempmemstart, variablelist, linelist)});
    linelist.push_back({simpleline, "21"+location});
  }
  /* goto may link to a place in the code which has not been
   * compiled yet. to handle this, on this second pass, we add the 
   * "40" branch command, but don't yet add where to branch to. 
   * this is done later in the third pass. */
  else if(parsedline[0] == "goto"){
    /*string location;
    int i = 0;
    for(array<string, 2> pair : linelist){
      //cout<<parsedline[1]<<" "<<pair[0]<<endl;
      if(parsedline[1] == pair[0]){
        if(i<10){location = "0"+to_string(i); break;}
        else{location = to_string(i); break;}
      }
      i++;
    }
    linelist.push_back({simpleline, "40"+location});*/
    linelist.push_back({simpleline, "40"+parsedline[1]});
  }else if(parsedline[0] == "rem"){//primary
    //do nothing
  }else if(parsedline[0] == "input"){//primary
    linelist.push_back( {simpleline, "10"+ returnvarlocation(parsedline[1], variablelist)} );
  }else if(parsedline[0] == "print"){//primary
    linelist.push_back( {simpleline, "11"+ returnvarlocation(parsedline[1], variablelist)} );
  }else if(parsedline[0] == "end"){//primary
    linelist.push_back( {simpleline, "4300"} );
  }
}

void compile() {
    cout<<"File to be compiled: ";
  string simplefilename; cin>>simplefilename;
  cout<<"Name compiled file: ";
  string outputfilename; cin>>outputfilename;
  //pulls in lines and turns them into an array
  vector<string> lines (0);
  ifstream infile(simplefilename, ios::in);
	string next;
	while(getline(infile, next)) {
		lines.push_back(next);
	}
	infile.close();
  //splitting the lines into a 2D vector around ' '
  vector<vector<string>> parsedlines;
  for(string line : lines){
    vector<string> parsedline;
    string lastword = "";
    for(char c : line){
      if(c != ' '){lastword+=c;}
      else{
        parsedline.push_back(lastword);
        lastword = "";
      }
    }
    parsedline.push_back(lastword);
    
    parsedlines.push_back(parsedline);
  }//should now have { {40, let, i, =, 0}, {...}, ...}
  //first pass: check syntax.
  string errorline = betterCheckSyntax(parsedlines);
  if(errorline != "-1"){
    cout<<"Problem on line "<<errorline<<"!";
    return;
  }

  /* second pass: preloading variables and literals into variable list
   * expressions often need temp variables, which store evaluted sub
   * expressions like "x + 5". temp variables will be stored
   * right after real variables. because of this we need to make the
   * list of all variables before evaluating temp variables.
   * literals must be put into memory before code can even run, not
   * in machine code though*/
  vector<array<string, 2>> variablelist; //variable list will be {(memorylocation, variable name), (..)}
  vector<array<string, 2>> linelist; //linelist will be {(simplelinenumber, 2215), (...)}
  
  for(vector<string> line : parsedlines){
    line.erase(line.begin());
    for(string element : line){
      if(element == "goto" || element == "rem"){break;}
      else if(element == "a" || element == "b" || element == "c" || element == "d" || element == "e" || element == "f" || element == "g" || element == "h" || element == "i" || element == "j" || element == "k" || element == "l" || element == "m" || element == "n" || element == "o" || element == "p" || element == "q" || element == "r" || element == "s" || element == "t" || element == "u" || element == "v" || element == "w" || element == "x" || element == "y" || element == "z" || isdigit(element[0]) ){
        returnvarlocation(element, variablelist);
      }
    }
  }
  int freememorystart; istringstream(variablelist[variablelist.size()-1][0]) >> freememorystart; freememorystart-=1;
  /*third pass: this makes everything into machine code except 
   * goto statements, which require everything else in order to get
   * the machine line number to go to.*/
  for(vector<string> parsedline : parsedlines){
    string simpleline = parsedline[0];
    parsedline.erase(parsedline.begin());
    parseCommand(simpleline, freememorystart, parsedline, variablelist, linelist);
  }
  /*fourth pass: linking goto. */
  //find 40 line pair, get goto simple line, find corresponding machine line number with that goto simple line in linelist;
  int k = 0;
  for(array<string, 2> pair : linelist) {
    
    if(pair[1].substr(0, 2) == "40") {
      string gotoline=pair[1].substr(2, 2);
      int j=0;
      for(array<string, 2> pair2 : linelist) {
        if(gotoline==pair2[0]){
          if(j<10){linelist[k][1]="400"+to_string(j);}
          else{linelist[k][1]="40"+to_string(j);}
          break;
        }
        j++;
      }
    }
    k++;
  }
  //pushes out lines into text document
  ofstream outfile(outputfilename, ios::out);
  int i = 0;
	for(array<string, 2> line : linelist){
    outfile << line[1] << endl;
    i++;
  }
  int firstvar; istringstream(variablelist[variablelist.size()-1][0]) >> firstvar;
  while(i<firstvar){outfile << "0000" << endl; i++;}
  reverse(variablelist.begin(), variablelist.end());
  for(array<string, 2> pair : variablelist){
    if(isdigit(pair[1][0]) || (pair[1][0] == '-' && isdigit(pair[1][0]) )){
      string out;
      string literal = pair[1];
      if(literal.size()==1){out = "000"+literal;}
      else if(literal.size()==2){out = "00"+literal;}
      else if(literal.size()==3){out = "0"+literal;}
      else if(literal.size()==4){out = literal;}
      outfile << out << endl;
    }
    else{outfile << "0000" << endl;}
  }
  reverse(variablelist.begin(), variablelist.end());
	outfile.close();
  cout<<"Compiled!";
}

void run() {
  cout<<"File to be run: ";
  string runfilename; cin>>runfilename;
  SML program = SML(runfilename);
  program.run();
}
int main() {
  cout<<"Run (r) or compile (c)?: ";
  string ans; cin>>ans;
  if(ans == "r") {
    run();
  }
  else {
    compile();
  }
  return 0;
}