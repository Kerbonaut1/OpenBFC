#include <iostream>
#include <fstream>
#include "GetExePath.h"
#include <string>
#include <vector>

using namespace std;

int main()
{
	// Plan (pseudocode, detailed):
	// 1. Open 'main.bf' for reading and create 'main_release.c' for writing (same as before).
	// 2. Write C prologue to the output file.
	// 3. Read the input BF file character by character.
	// 4. Maintain two running counters:
	//      - dataDelta: net change for '+' and '-'
	//      - ptrDelta: net change for '>' and '<'
	// 5. While scanning:
	//      - If char is '+' or '-', accumulate into dataDelta.
	//          * If ptrDelta is non-zero, flush ptrDelta first (emit optimized p += N or p -= N).
	//      - If char is '>' or '<', accumulate into ptrDelta.
	//          * If dataDelta is non-zero, flush dataDelta first (emit optimized m[p] += N or m[p] -= N).
	//      - If char is any other BF command ('.' ',' '[' ']') or any non-command:
	//          * Flush both dataDelta and ptrDelta (if non-zero).
	//          * Emit the corresponding C for the BF command (e.g., putchar, getchar, while, }).
	//      - Ignore any other characters (comments/whitespace).
	// 6. After the loop flush any remaining deltas.
	// 7. Finish the C file (return 0; }) and compile using system("gcc ...").
	// 8. Notes on emission:
	//      - If delta == 0 -> emit nothing.
	//      - Use 'm[p] += N;' or 'm[p] -= N;' for data changes.
	//      - Use 'p += N;' or 'p -= N;' for pointer moves.
	//      - This collapses repeated chars into a single arithmetic op (e.g., '+++--' -> m[p] += 1).
	// 9. Keep other behavior identical to the original program.

	// open a file in read mode
	std::ifstream inputFile(GetExePath() + "\\main.bf");


	// check if the file is opened successfully
	if (!inputFile.is_open())
	{
		std::cerr << "Error: Could not open the file 'main.bf'." << std::endl;
		return 1;
	}
	else
	{
		std::cout << "File 'main.bf' opened successfully." << std::endl;
	}

	// create a new file in write mode
	std::ofstream outputFile(GetExePath() + "\\main_release.cpp");

	if (!outputFile.is_open())
	{
		std::cerr << "Error: Could not create the file 'main_release.cpp'." << std::endl;
		return 1;
	}
	else
	{
		std::cout << "File 'main_release.cpp' created successfully." << std::endl;
	}

	// write some data to the output file
	outputFile << "// Compiled with OpenBFC Version Beta 1" << std::endl;
	outputFile << "#include <iostream>" << std::endl;
	outputFile << "unsigned char m[1000];" << std::endl;
	outputFile << "int p = 0;" << std::endl;
	outputFile << "char inp = ' ';" << std::endl;
	outputFile << "int main() {" << std::endl;

	// helper lambdas to flush accumulated deltas
	auto flushData = [&](int &dataDelta) {
		if (dataDelta == 0) return;
		if (dataDelta > 0)
		{
			outputFile << "    m[p] += " << dataDelta << ";" << std::endl;
		}
		else
		{
			outputFile << "    m[p] -= " << -dataDelta << ";" << std::endl;
		}
		dataDelta = 0;
	};

	auto flushPtr = [&](int &ptrDelta) {
		if (ptrDelta == 0) return;
		if (ptrDelta > 0)
		{
			outputFile << "    p += " << ptrDelta << ";" << std::endl;
		}
		else
		{
			outputFile << "    p -= " << -ptrDelta << ";" << std::endl;
		}
		ptrDelta = 0;
	};

	// read the input file character by character and optimize runs
	int processedChars = 0;
	int dataDelta = 0;
	int ptrDelta = 0;

	char ch;
	while (inputFile.get(ch))
	{
		switch (ch)
		{
		case '+':
			// accumulate data delta; if we were accumulating pointer moves, flush them first
			if (ptrDelta != 0)
			{
				flushPtr(ptrDelta);
			}
			dataDelta += 1;
			break;
		case '-':
			if (ptrDelta != 0)
			{
				flushPtr(ptrDelta);
			}
			dataDelta -= 1;
			break;
		case '>':
			// accumulate pointer delta; if we were accumulating data changes, flush them first
			if (dataDelta != 0)
			{
				flushData(dataDelta);
			}
			ptrDelta += 1;
			break;
		case '<':
			if (dataDelta != 0)
			{
				flushData(dataDelta);
			}
			ptrDelta -= 1;
			break;
		case '.':
			// flush pending deltas then emit putchar
			flushData(dataDelta);
			flushPtr(ptrDelta);
			outputFile << "    if(m[p] != 10){ std::cout << (char)m[p];} if(m[p] == 10){std::cout << '\\n' << std::flush;}" << std::endl;
			break;
		case ',':
			flushData(dataDelta);
			flushPtr(ptrDelta);
			outputFile << "    std::cin >> inp; m[p] = (int)inp;" << std::endl;
			break;
		case '[':
			flushData(dataDelta);
			flushPtr(ptrDelta);
			outputFile << "    while (m[p]) {" << std::endl;
			break;
		case ']':
			flushData(dataDelta);
			flushPtr(ptrDelta);
			outputFile << "    }" << std::endl;
			break;
		default:
			// Ignore other characters (comments, whitespace, etc.), but do not flush here:
			// We only flush when a different BF command is encountered or at end of file.
			break;
		}

		processedChars++;
		// Optionally, you can print progress to the console
	}

	// flush any remaining accumulated deltas
	flushData(dataDelta);
	flushPtr(ptrDelta);

	outputFile << "    return 0;" << std::endl;
	outputFile << "}" << std::endl;

	// use gcc to compile the generated C code
	system(("g++ " + GetExePath() + "\\main_release.cpp -std=c++17 -o " + GetExePath() + "\\main_release.exe").c_str());

	// cleanup (kept commented out like original)
	//outputFile.close();
	//inputFile.close();
	//remove((GetExePath() + "\\main_release.c").c_str());

	return 0;
}