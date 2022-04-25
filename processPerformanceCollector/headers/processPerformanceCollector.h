#pragma once

#include <Windows.h>

#include <Pdh.h>
#pragma comment(lib, "Pdh")

#include <Psapi.h>

class CProcessPerformanceCollector {

public:

	CProcessPerformanceCollector(HANDLE process);

	void update();

	double _processCpuUsageTotal;
	double _processCpuUsageKernel;
	double _processCpuUsageUser;

	double _privateMemory;
	double _nonPagedPoolMemory;

private:

	HANDLE _process;

	LARGE_INTEGER _lastProcessTime;
	LARGE_INTEGER _lastProcessCpuUsageKernel;
	LARGE_INTEGER _lastProcessCpuUsageUser;

	int _processorNum;
	
	PDH_HQUERY _pdhQuery;

	PDH_HCOUNTER _privateMemoryCounter;
	PDH_HCOUNTER _nonPagedPoolCounter;

};