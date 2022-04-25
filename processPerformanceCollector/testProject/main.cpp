#include <stdio.h>

#include <thread>

#include "../headers/processPerformanceCollector.h"
#pragma comment(lib, "../release/processPerformanceCollector")


unsigned __stdcall func(void*) {

	int* arr[100];

	for (;;) {

		for (int i = 0; i < 100; i++) {
			arr[i] = new int;
		}

	}

}


int main() {

	CProcessPerformanceCollector procPerformance(GetCurrentProcess());

	_beginthreadex(nullptr, 0, func, nullptr, 0, nullptr);
	_beginthreadex(nullptr, 0, func, nullptr, 0, nullptr);
	_beginthreadex(nullptr, 0, func, nullptr, 0, nullptr);

	for (;;) {

		procPerformance.update();

		printf("CPU usage (total) : %.2lf\n", procPerformance._processCpuUsageTotal);
		printf("CPU usage (kernel): %.2lf\n", procPerformance._processCpuUsageKernel);
		printf("CPU usage (user)  : %.2lf\n\n", procPerformance._processCpuUsageUser);

		printf("Private Memory    : %.2lf\n", procPerformance._privateMemory);
		printf("Non Paged Pool    : %.2lf\n", procPerformance._nonPagedPoolMemory);
		printf("\n\n\n");

		Sleep(1000);
		system("cls");

	}

	return 0;

}