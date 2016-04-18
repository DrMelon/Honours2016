#pragma once

//Filename: Stopwatch.h
//Version: 1.0
//Author: J. Brown (1201717)
//Date: 12/04/2016
//
//Purpose: This is the header file for a simple performance instrumentation class.
// This will allow me to time how long functions take to execute.

#include "ofMain.h"

class Stopwatch
{
	private:
		int startTimeStampMillis;
		int endTimeStampMillis;
	public:
		void StartTiming();
		int StopTiming(std::string eventName);
		Stopwatch();
		~Stopwatch();
};

