#pragma once

#include <cstring>
#include "mpi.h"
#include <vector>

using namespace std;

namespace MPIMethods
{

int numOfProcesses;
int processRank;
int rootRank;

void Barrier()
{
	MPI_Barrier(MPI_COMM_WORLD);
}

void BroadcastValue(string* data, int maxLength)
{
	char tmp[maxLength];
	strcpy(tmp, (*data).c_str());
	MPI_Bcast(tmp, maxLength, MPI_CHAR, rootRank, MPI_COMM_WORLD);
	*data = string(tmp);
}

void BroadcastValue(int* data)
{
	MPI_Bcast(data, 1, MPI_INT, rootRank, MPI_COMM_WORLD);
}

void BroadcastValue(double* data)
{
	MPI_Bcast(data, 1, MPI_DOUBLE, rootRank, MPI_COMM_WORLD);
}

void BroadcastValues(double* data, int count)
{
	MPI_Bcast(data, count, MPI_DOUBLE, rootRank, MPI_COMM_WORLD);
}

void BroadcastValues(vector<double>& data)
{
	MPI_Bcast(data.data(), data.size(), MPI_DOUBLE, rootRank, MPI_COMM_WORLD);
}

void BroadcastValues(vector<vector<double> >& data)
{
	//TODO: if needed in TDVMC-calculations -> check performance
	for (unsigned int i = 0; i < data.size(); i++)
	{
		BroadcastValues(data[i]);
	}
}

vector<double> ReduceToMinMaxMean(double data)
{
	vector<double> result = { };
	double own = 0;
	double min = 0;
	double max = 0;
	double sum = 0;

	own = data;
	MPI_Reduce(&own, &min, 1, MPI_DOUBLE, MPI_MIN, rootRank, MPI_COMM_WORLD);
	MPI_Reduce(&own, &max, 1, MPI_DOUBLE, MPI_MAX, rootRank, MPI_COMM_WORLD);
	MPI_Reduce(&own, &sum, 1, MPI_DOUBLE, MPI_SUM, rootRank, MPI_COMM_WORLD);
	if (processRank == rootRank)
	{
		result = { min, max, sum / (double)numOfProcesses };
	}
	return result;
}

void ReduceToAverage(double* data)
{
	double ownValues;
	double reducedValues;

	ownValues = *data;
	MPI_Reduce(&ownValues, &reducedValues, 1, MPI_DOUBLE, MPI_SUM, rootRank, MPI_COMM_WORLD);
	if (processRank == rootRank)
	{
		*data = reducedValues / (double) numOfProcesses;
	}
}

void ReduceToAverage(double* data, int count)
{
	double* reducedValues = new double[count];
	MPI_Reduce(data, reducedValues, count, MPI_DOUBLE, MPI_SUM, rootRank, MPI_COMM_WORLD);
	if (processRank == rootRank)
	{
		for (int i = 0; i < count; i++)
		{
			data[i] = reducedValues[i] / (double) numOfProcesses;
		}
	}
	delete[] reducedValues;
}

void ReduceToAverage(vector<double>& data)
{
	double* reducedValues = new double[data.size()]; //TODO: check on zusie what method is faster

	MPI_Reduce(data.data(), reducedValues, data.size(), MPI_DOUBLE, MPI_SUM, rootRank, MPI_COMM_WORLD);
	//INFO: only root process holds the average values from all processes.
	if (processRank == rootRank)
	{
		for (unsigned int i = 0; i < data.size(); i++)
		{
			data[i] = reducedValues[i] / (double) numOfProcesses;
		}
	}

	delete[] reducedValues;
}

//INFO: is a little bit slower than ReduceToAverage with double* reducedValues
//void ReduceToAverage(vector<double>& data)
//{
//	vector<double> reducedValues(data.size());
//
//	MPI_Reduce(data.data(), reducedValues.data(), data.size(), MPI_DOUBLE, MPI_SUM, rootRank, MPI_COMM_WORLD);
//	//INFO: only root process holds the average values from all processes.
//	if (processRank == rootRank)
//	{
//		for (unsigned int i = 0; i < data.size(); i++)
//		{
//			data[i] = reducedValues[i] / (double) numOfProcesses;
//		}
//	}
//}

void ReduceToAverage(vector<vector<double> >& data)
{
	int size1 = data.size();
	int size2 = data[0].size();
	int totalSize = size1 * size2;
	//TODO: besseres kriterium überlegen und auch auf anderen servern testen
	//		der wert 500 ist für zusie bie 256 prozessen.
	//		wie viele elemente insgesamt? quadratische matrix oder eher recht lange/breite matrix? ...
	if (size2 > 500)
	{
		//TODO: warum funtioniert diese methode nicht? ist extrem langsam wenn auf zusie 257 prozesse gestartet werden (für 256 noch ganz normal)
		for (unsigned int i = 0; i < data.size(); i++)
		{
			ReduceToAverage(data[i]);
		}
	}
	else
	{
		//TODO: funktioniert auch für >257 prozesse, ist aber langsamer. effizientere methode möglich?
		vector<double> combinedValues(totalSize);
		for (int i = 0; i < totalSize; i++)
		{
			combinedValues[i] = data[i / size2][i % size2];
		}
		ReduceToAverage(combinedValues);
		if (processRank == rootRank)
		{
			for (int i = 0; i < totalSize; i++)
			{
				data[i / size2][i % size2] = combinedValues[i];
			}
		}
	}
}

map<int, int> GatherHistogram(int data)
{
	vector<int> gatheredValues(numOfProcesses);
	map<int, int> histogram;

	MPI_Gather(&data, 1, MPI_INT, gatheredValues.data(), 1, MPI_INT, rootRank, MPI_COMM_WORLD);
	if (processRank == rootRank)
	{
		for (int i = 0; i < numOfProcesses; i++)
		{
			histogram[gatheredValues[i]]++;
		}
	}
	return histogram;
}

}
