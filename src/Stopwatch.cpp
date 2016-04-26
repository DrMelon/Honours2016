#include "Stopwatch.h"



Stopwatch::Stopwatch()
{
	Stopwatch("out.log");
}

Stopwatch::Stopwatch(std::string newfilename)
{
	startTimeStampMillis = 0;
	endTimeStampMillis = 0;
	filename = newfilename;
}


Stopwatch::~Stopwatch()
{
	// Write log contents out to file.
	GetAverage();
	std::ofstream fileOut;
	fileOut.open(filename, std::ios::app);
	
	for (auto iter = log.begin(); iter != log.end(); iter++)
	{
		fileOut << (*iter) << std::endl;
	}


	fileOut.close();
}

void Stopwatch::StartTiming()
{
	startTimeStampMillis = ofGetElapsedTimeMillis();
	endTimeStampMillis = 0;
}

void Stopwatch::ClearLogs()
{
	timelog.clear();
	log.clear();
}

int Stopwatch::StopTiming(std::string eventName)
{
	endTimeStampMillis = ofGetElapsedTimeMillis();
	std::stringstream stringBuild;
	stringBuild << eventName << " Time Taken: " << endTimeStampMillis - startTimeStampMillis << "ms. ";
	log.push_back(stringBuild.str());
	timelog.push_back(endTimeStampMillis - startTimeStampMillis);
	return endTimeStampMillis - startTimeStampMillis;
}

float Stopwatch::GetAverage()
{ 
	float average = 0;
	for (auto iter = timelog.begin(); iter != timelog.end(); iter++)
	{
		average += (*iter);
	}
	average = average / timelog.size();

	log.push_back(std::to_string(average));

	return average;
}