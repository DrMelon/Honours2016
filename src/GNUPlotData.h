#pragma once
#include <vector>
#include <iostream>
#include <fstream>
#include <string>

// Date: 20/04/2016
// Author: J. Brown (1201717)
// Purpose: This is intended to be a simple data structure and interface for outputting files compatible with the free graphing software called
//  'gnuplot'. 
//
// This will allow the application to quickly write out analytics data.
// 


// A plot event is added to a graph when the user performs a significant action which may impact performance.
struct GNUPlotEvent
{
	int xPosition;
	int xRange;
	std::string boxColour;
	std::string labelName;
};

template<class T>
struct GNUPlotData
{
	int DataColumns; // Maximum 4 columns
	
	// Generic vectors of data for the columns
	std::vector<T> Column1;
	std::vector<T> Column2;
	std::vector<T> Column3;
	std::vector<T> Column4;

	// Vector Of Plot Events for this graph.
	std::vector<GNUPlotEvent> Events;

	// Column Names
	std::string Column1Name;
	std::string Column2Name;
	std::string Column3Name;
	std::string Column4Name;

	// Graph Axes Names
	std::string XAxisName;
	std::string YAxisName;
	std::string ZAxisName;

	// Graph Style:
	// 0 = Dot Plot
	// 1 = Lines
	// 2 = Lines & Dots
	// 3 = Dots, Errorbars (3 cols only)
	// 4 = Lines, Dots, Errorbars (3 cols only)
	int GraphStyle;

	// Formatting - see gnuplot ps_symbols for info.
	int DotSize;
	int LineThickness;
	std::string HexColour;
	int DotType;

};


class GNUPlotDataManager
{
	public:
		GNUPlotDataManager();
		~GNUPlotDataManager();

		template <class T>
		void WriteGraphDataFile(GNUPlotData<T> data, std::string filename);
};

template <class T>
void GNUPlotDataManager::WriteGraphDataFile(GNUPlotData<T> data, std::string filename)
{
	// Attempt to open a file stream.
	std::ofstream fileStream;
	fileStream.open(filename, std::ios::out);

	if (fileStream.is_open())
	{
		fileStream << "# Generated GNUPlot File." << std::endl;

		// Examine plot data.
		if (data.DataColumns < 2 || data.DataColumns > 4)
		{
			std::cout << "Too little/too many data columns. Aborting." << std::endl;
			fileStream.close();
			return;
		}




		if (data.DataColumns == 2)
		{
			// 2 Columns.

			// If columns of differing lengths, abort.
			if (data.Column1.size() != data.Column2.size())
			{
				std::cout << "Malformed data. Aborting." << std::endl;
				fileStream.close();
				return;
			}

			// Write column names
			fileStream << "# " << data.Column1Name << "\t" << data.Column2Name << std::endl;

			// Write column data recursively.
			for (int i = 0; i < data.Column1.size(); i++)
			{
				fileStream << data.Column1.at(i) << " " << data.Column2.at(i) << std::endl;
			}
		}

		if (data.DataColumns == 3)
		{
			// 2 Columns.

			// If columns of differing lengths, abort.
			if (data.Column1.size() != data.Column2.size() || data.Column1.size() != data.Column3.size())
			{
				std::cout << "Malformed data. Aborting." << std::endl;
				fileStream.close();
				return;
			}

			// Write column names
			fileStream << "# " << data.Column1Name << "\t" << data.Column2Name << "\t" << data.Column3Name << std::endl;

			// Write column data recursively.
			for (int i = 0; i < data.Column1.size(); i++)
			{
				fileStream << data.Column1.at(i) << " " << data.Column2.at(i) << " " << data.Column3.at(i) << std::endl;
			}
		}

		// Generate command strings
		fileStream << "# Command String" << std::endl;
		fileStream << "# set xlabel \"" << data.XAxisName << "\"" << std::endl;
		fileStream << "# set ylabel \"" << data.YAxisName << "\"" << std::endl;
		fileStream << "# set style line 1 lc rgb '#" << data.HexColour << "' lt 1" << " lw " << data.LineThickness << " pt " << data.DotType << " ps " << data.DotSize << std::endl;
		
		// Generate command strings for events
		for (int i = 0; i < data.Events.size(); i++)
		{
			// Coloured Rectangle
			fileStream << "# set object " << i + 1 << " rectangle from \"" << data.Events.at(i).xPosition - data.Events.at(i).xRange << "\", graph 0 to \"" << data.Events.at(i).xPosition + data.Events.at(i).xRange << "\", graph 1 fs solid fc rgb '#" << data.Events.at(i).boxColour << "' behind" << std::endl;

			// Label
			fileStream << "# set label " << i + 1 << "\"" << data.Events.at(i).labelName << "\" at \"" << data.Events.at(i).xPosition << "\", graph 0.95 center" << std::endl;
		}

		// Plotting command strings.
		if (data.DataColumns == 2)
		{
			if (data.GraphStyle == 0)
			{
				// Dot Plot
				fileStream << "# plot '" << filename << "' using 1:2 with points ls 1" << std::endl;
			}
			if (data.GraphStyle == 1)
			{
				// Line Plot
				fileStream << "# plot '" << filename << "' using 1:2 with lines ls 1" << std::endl;

			}
			if (data.GraphStyle == 2)
			{
				// Lines + Dots
				fileStream << "# plot '" << filename << "' using 1:2 with linespoints ls 1" << std::endl;
			}
		}

		if (data.DataColumns == 3)
		{
			if (data.GraphStyle == 0)
			{
				// Dot Plot
				fileStream << "# plot '" << filename << "' using 1:2:3 with points ls 1" << std::endl;
			}
			if (data.GraphStyle == 1)
			{
				// Line Plot
				fileStream << "# plot '" << filename << "' using 1:2:3 with lines ls 1" << std::endl;
			}
			if (data.GraphStyle == 2)
			{
				// Lines + Dots
				fileStream << "# plot '" << filename << "' using 1:2:3 with linespoints ls 1" << std::endl;
			}
			if (data.GraphStyle == 3)
			{
				// Dots + errorbars
				fileStream << "# plot '" << filename << "' using 1:2:3 with yerrorbars ls 1" << std::endl;
			}
			if (data.GraphStyle == 4)
			{
				// Lines, Dots + Errorbars
				fileStream << "# plot '" << filename << "' using 1:2:3 with yerrorbars ls 1, \\ '' using 1:2 with lines ls 1" << std::endl;
			}
		}



		fileStream << "# End of file.";

		fileStream.close();




	}
	else
	{
		std::cout << "Could not open file for output. Aborting." << std::endl;
		return;
	}

}