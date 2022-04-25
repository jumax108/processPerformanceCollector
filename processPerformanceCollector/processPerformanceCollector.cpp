#include "headers/processPerformanceCollector.h"

#include <stdio.h>

CProcessPerformanceCollector::CProcessPerformanceCollector(HANDLE process) {
	
	/* var init */ {
		_process = process;

		_processCpuUsageTotal = 0;
		_processCpuUsageKernel = 0;
		_processCpuUsageUser = 0;

		ZeroMemory(&_lastProcessTime, sizeof(LARGE_INTEGER));
		ZeroMemory(&_lastProcessCpuUsageKernel, sizeof(LARGE_INTEGER));
		ZeroMemory(&_lastProcessCpuUsageUser, sizeof(LARGE_INTEGER));

		_privateMemory = 0;
		_nonPagedPoolMemory = 0;
	}

	/* get processor num */{
		SYSTEM_INFO systemInfo; 
		GetSystemInfo(&systemInfo);
		_processorNum = systemInfo.dwNumberOfProcessors;
	}

	// get process name
	wchar_t* processName; {
		wchar_t processFullName[50] = { 0, };
		GetModuleBaseName(process, NULL, processFullName, 50);
		wchar_t token[] = L".";
		wchar_t wcstokContext[100] = { 0, };
		processName = wcstok_s(processFullName,token, (wchar_t**)&wcstokContext);
	}

	// open pdh query
	PdhOpenQuery(NULL, NULL, &_pdhQuery);

	/* private memory counter init */ {
		wchar_t privateMemoryQuery[100];
		wsprintf(privateMemoryQuery, L"\\Process(%s)\\Private Bytes", processName);
		PdhAddCounter(_pdhQuery, privateMemoryQuery, NULL, &_privateMemoryCounter);
	}
	
	/* non paged pool counter init */ {
		wchar_t nonPagedPoolQuery[100];
		wsprintf(nonPagedPoolQuery, L"\\Process(%s)\\Pool Nonpaged Bytes", processName);
		PdhAddCounter(_pdhQuery, nonPagedPoolQuery, NULL, &_nonPagedPoolCounter);
	}
}

void CProcessPerformanceCollector::update() {

	/* cpu usage update */ {
		LARGE_INTEGER kernel;
		LARGE_INTEGER user;

		LARGE_INTEGER none;
		LARGE_INTEGER nowTime;

		GetSystemTimeAsFileTime((LPFILETIME)&nowTime);

		GetProcessTimes(_process, (LPFILETIME)&none, (LPFILETIME)&none, (LPFILETIME)&kernel, (LPFILETIME)&user);

		__int64 timeDiff = nowTime.QuadPart - _lastProcessTime.QuadPart;
		__int64 userDiff = user.QuadPart - _lastProcessCpuUsageUser.QuadPart;
		__int64 kernelDiff = kernel.QuadPart - _lastProcessCpuUsageKernel.QuadPart;

		double total = kernelDiff + userDiff;

		_processCpuUsageTotal = (double)(total / (double)_processorNum / (double)timeDiff * 100.0);
		_processCpuUsageKernel = (double)(kernelDiff / (double)_processorNum / (double)timeDiff * 100.0);
		_processCpuUsageUser = (double)(userDiff / (double)_processorNum / (double)timeDiff * 100.0);

		_lastProcessTime = nowTime;
		_lastProcessCpuUsageKernel = kernel;
		_lastProcessCpuUsageUser = user;
	}

	// pdh update
	PdhCollectQueryData(_pdhQuery);

	/* private memory update */ {
		PDH_FMT_COUNTERVALUE counterValue;
		PdhGetFormattedCounterValue(_privateMemoryCounter, PDH_FMT_DOUBLE, NULL, &counterValue);
		_privateMemory = counterValue.doubleValue;
	}

	/* non paged pool update */ {
		PDH_FMT_COUNTERVALUE counterValue;
		PdhGetFormattedCounterValue(_nonPagedPoolCounter, PDH_FMT_DOUBLE, NULL, &counterValue);
		_nonPagedPoolMemory = counterValue.doubleValue;
	}

}

