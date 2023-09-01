#include "MidiHandler.h"
#include <chrono>
#include <thread>
#include <cstdio>


// NOTE: Need far more robust releasing of resources in the case of failure to init.

int main(int argc, char *argv[])
{
	// Kick off the processing.
	auto result = JackInit();

	// Near infinite loop.
	if (result == 0)
	{
		while (!shouldExitNow)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}
	}

	// Clean up.
	JackExit();

	return 0;
}