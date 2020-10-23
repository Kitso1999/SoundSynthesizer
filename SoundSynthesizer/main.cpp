#include <iostream>

#include "olcNoiseMaker.h"

std::atomic<double> dFrequencyOutput = 0.0;

double MakeNoice(double dTime)
{
	double dOutput = sin(dFrequencyOutput * 2.0 * 3.14159 * dTime);
	return dOutput;
}


int main()
{
	std::wcout << "onelonecoder.com - Synthesizer Part 1" << std::endl;

	//Get all sound hardware
	std::vector<std::wstring> devices = olcNoiseMaker<short>::Enumerate();

	//Display findings
	for (auto d : devices)	std::wcout << "Found audio device:" << d << std::endl;
	
	//Create sound machine
	olcNoiseMaker<short> sound(devices[0], 44100, 1, 8, 512);
	sound.SetUserFunction(MakeNoice);

	double dOctaveBaseFrequency = 110.0;
	double d12thRootOf2 = std::pow(2.0, 1.0 / 12.0);


	
	while (true)
	{
		bool bKeyPressed = false;
		//Add a Keyboard
		for (int k = 0; k < 16; ++k)
		{
			if (GetAsyncKeyState((unsigned char)("ZSXDCVGBHNJMK\xbcL\xbe"[k])) & 0x8000)
			{
				
				dFrequencyOutput = dOctaveBaseFrequency * std::pow(d12thRootOf2, k);
				bKeyPressed = true;
			}
		}
		if (!bKeyPressed)
		{
			dFrequencyOutput = 0.0;
		}
	}

	system("pause");
	return 0;
}