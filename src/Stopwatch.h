#pragma once
#include <vector>
#include <string.h>
#include <sstream>
#include <iostream>
#include <fstream>

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
		std::vector<std::string> log;
		std::vector<int> timelog;
	public:
		void StartTiming();
		int StopTiming(std::string eventName);
		float GetAverage();
		void ClearLogs();
		std::string filename;
		Stopwatch();
		Stopwatch(std::string newfilename);
		~Stopwatch();
};

