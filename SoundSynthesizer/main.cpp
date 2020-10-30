#include <iostream>

#include "olcNoiseMaker.h"


//Convert Frequency to Angular Frequency
double w(double dHertz)
{
	return 2 * PI * dHertz;
}

double osc(double dHertz, double dTime, int nType)
{
	switch (nType)
	{
	case 0: //Sine Wave
		return sin(w(dHertz) * dTime);
	case 1:	//Square Wawe
		return (sin(w(dHertz) * dTime) > 0) ? 1.0 : -1.0;
	case 2:	//Triangle Wawe
		return (2 / PI) * asin(sin(w(dHertz) * dTime));
	case 3:	//Saw Wave Math Method
	{
		double dOutput = 0.0;

		for (double n = 1.0; n < 100.0; ++n)
			dOutput += (sin(n * w(dHertz) * dTime)) / n;
		return dOutput * (2.0 / PI);
	}
	case 4:	//Saw Wave CS Method
		return (2.0 / PI) * (dHertz * PI * fmod(dTime, 1.0 / dHertz) - (PI / 2.0));
	case 5: //Pseudo Random Noice
		return 2.0 * (double)rand() / (double)RAND_MAX - 1.0;
		
	default: return 0.0;
	}
}


struct sEnvelopeADSR
{
	double dAttackTime;
	double dDecayTime;
	double dReleaseTime;

	double dSustainAmplitude;
	double dStartAmplitude;

	double dTriggerOnTime;
	double dTriggerOffTime;

	bool bNoteOn;

	sEnvelopeADSR()
	{
		dAttackTime = 0.01;
		dDecayTime = 0.01;
		dStartAmplitude = 1.0;
		dSustainAmplitude = 0.8;
		dReleaseTime = 0.02;
		dTriggerOnTime = 0.0;
		dTriggerOffTime = 0.0;
		bNoteOn = false;
	}

	double GetAmplitude(double dTime)
	{
		double dAmplitude = 0.0;
		double dLifeTime = dTime - dTriggerOnTime;


		if (bNoteOn)
		{
			//ADS

			//Attack
			if (dLifeTime <= dAttackTime)
				dAmplitude = (dLifeTime / dAttackTime) * dStartAmplitude;

			//Decay
			if (dLifeTime > dAttackTime && dLifeTime <= (dAttackTime + dDecayTime))
			{
				dAmplitude = dStartAmplitude - (dLifeTime - dAttackTime) / dDecayTime * ((dStartAmplitude - dSustainAmplitude));
			}

			//Sustain
			if (dLifeTime > (dAttackTime + dDecayTime))
			{

				dAmplitude = dSustainAmplitude;
			}
		}
		else
		{
			//R
			dAmplitude = dSustainAmplitude * ( 1 - ((dTime - dTriggerOffTime) / dReleaseTime));
		}
		
		if (dAmplitude < 0.0001)
		{
			dAmplitude = 0.0;
		}




		return dAmplitude;
	}

	void NoteOn(double dTimeOn)
	{
		dTriggerOnTime = dTimeOn;
		bNoteOn = true;
	}

	void NoteOff(double dTimeOff)
	{
		dTriggerOffTime = dTimeOff;
		bNoteOn = false;
	}
};


//Global Synthesizer Variables
std::atomic<double> dFrequencyOutput = 0.0;
sEnvelopeADSR envelope;
double dOctaveBaseFrequency = 110.0;
double d12thRootOf2 = std::pow(2.0, 1.0 / 12.0);
double dAmp = 0.0;


double MakeNoice(double dTime)
{
	double dOutput = envelope.GetAmplitude(dTime) * osc(dFrequencyOutput, dTime, 2);
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


	int nCurrentKey = -1;
	bool bKeyPressed;
	
	while (true)
	{
		bKeyPressed = false;
		//Add a Keyboard
		for (int k = 0; k < 16; ++k)
		{
			if (GetAsyncKeyState((unsigned char)("ZSXDCVGBHNJMK\xbcL\xbe"[k])) & 0x8000)
			{
				if (nCurrentKey != k)
				{
					dFrequencyOutput = dOctaveBaseFrequency * std::pow(d12thRootOf2, k);
					envelope.NoteOn(sound.GetTime());
					bKeyPressed = true;
					nCurrentKey = k;
				}
				bKeyPressed = true;
			}
	
		}
		if (!bKeyPressed)
		{
			if (nCurrentKey != -1)
			{
				envelope.NoteOff(sound.GetTime());
				nCurrentKey = -1;
			}
		}
	}

	system("pause");
	return 0;
}