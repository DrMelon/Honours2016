#include "Stopwatch.h"



Stopwatch::Stopwatch()
{
	startTimeStampMillis = 0;
	endTimeStampMillis = 0;
}


Stopwatch::~Stopwatch()
{
}

void Stopwatch::StartTiming()
{
	startTimeStampMillis = ofGetElapsedTimeMillis();
	endTimeStampMillis = 0;
}

int Stopwatch::StopTiming(std::string eventName)
{
	endTimeStampMillis = ofGetElapsedTimeMillis();
	std::cout << eventName << " Time Taken: " << endTimeStampMillis - startTimeStampMillis << "ms. " << std::endl;
	return endTimeStampMillis - startTimeStampMillis;
}