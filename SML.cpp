/* The actual SML class. All it contains is a constructor which loads the file,
 * and the .run() method which runs said file.
 * Author: Daniel Coble
 * 10 21 19
*/
#include <iostream>
#include <string>
#include <iomanip>
#include <vector>
#include <fstream>
using namespace std;
#include "SML.h"

SML::SML(string filename) {
	ifstream infile(filename, ios::in);
	int next;
	int n = 0;
	while(!infile.eof()) {
		infile>>next;
		memory[n] = next;
		n ++;
	}
	infile.close();
	while(n < 100) {
		memory[n] = 0;
		n ++;
	}
}
void SML::run() {
	bool cont = true;
	int operation;
	int location;
	int accumulator = 0;
	while(cont) {
		operation = memory[pointer] / 100;
		location  = memory[pointer] % 100;
		switch(operation){
			case 10: { //Read from in
				cin>>memory[location]; // change that later
				pointer ++;
			} break;
			case 11: { //Print
				cout<<memory[location]<<endl; // change that later
				pointer ++;
			} break;
			case 20: { //Load
				accumulator = memory[location];
				pointer ++;
			} break;
			case 21: { //Store
				memory[location] = accumulator;
				pointer ++;
			} break;
			case 30: { //Add
				accumulator += memory[location];
				pointer ++;
			} break;
			case 31: { //Subtract
				accumulator -= memory[location];
				pointer ++;
			} break;
			case 32: { //Divide
				accumulator /= memory[location];
				pointer ++;
			} break;
			case 33: { //Multiply
				accumulator *= memory[location];
				pointer ++;
			} break;
			case 40: { //Branch
				pointer = location;
			} break;
			case 41: { //BranchNegative
				if(accumulator < 0) {
					pointer = location;
				}
				else {
					pointer ++;
				}
			} break;
			case 42: { //BranchZero
				if(accumulator == 0) {
					pointer = location;
				}
				else {
					pointer ++;
				}
			} break;
			case 43: { //HALT, in the name of the law.
				cont = false;
			} break;
		}
	}
}