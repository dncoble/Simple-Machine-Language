/* Header file of the SML class */
#include <iostream>
#include <string>
#include <iomanip>
#include <vector>
#include <fstream>
using namespace std;

class SML {
	public:
		SML(string filename);
		void run();
	private:
		int memory[100];
		int pointer;
};