#include "Benchmark.h"
#include <windows.h>
#include <psapi.h>
#include <shellapi.h>
#include <algorithm>
#include <fstream>
#include <iomanip>
#pragma comment(lib, "psapi.lib")
#pragma comment(lib, "shell32.lib")

namespace {
	struct Record {
		std::string metric;
		std::string label;
		double value;
	};

	std::vector<Record>& records()
	{
		static std::vector<Record> r = [] {
			std::vector<Record> v;
			v.reserve(4096);
			return v;
		}();
		return r;
	}

	double qpcFrequency()
	{
		static double freq = [] {
			LARGE_INTEGER f;
			QueryPerformanceFrequency(&f);
			return static_cast<double>(f.QuadPart);
		}();
		return freq;
	}

	bool startsWith(const std::wstring& s, const wchar_t* prefix, std::wstring& rest)
	{
		size_t len = wcslen(prefix);
		if (s.compare(0, len, prefix) != 0) return false;
		rest = s.substr(len);
		return true;
	}
}

namespace Bench {

	Config& config()
	{
		static Config c;
		return c;
	}

	void parseCommandLine()
	{
		int argc = 0;
		LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
		if (!argv) return;

		Config& c = config();
		for (int i = 1; i < argc; i++) {
			std::wstring arg = argv[i];
			std::wstring rest;
			if (arg == L"--bench") c.enabled = true;
			else if (startsWith(arg, L"--stage=", rest)) c.stage = std::clamp(_wtoi(rest.c_str()), 1, 3);
			else if (startsWith(arg, L"--run=", rest)) c.run = _wtoi(rest.c_str());
			else if (startsWith(arg, L"--seed=", rest)) c.seed = static_cast<unsigned>(_wtoi(rest.c_str()));
			else if (startsWith(arg, L"--out=", rest)) {
				int size = WideCharToMultiByte(CP_ACP, 0, rest.c_str(), -1, nullptr, 0, nullptr, nullptr);
				std::string path(size - 1, '\0');
				WideCharToMultiByte(CP_ACP, 0, rest.c_str(), -1, path.data(), size, nullptr, nullptr);
				c.outPath = path;
			}
		}
		LocalFree(argv);
	}

	double nowMs()
	{
		LARGE_INTEGER t;
		QueryPerformanceCounter(&t);
		return static_cast<double>(t.QuadPart) * 1000.0 / qpcFrequency();
	}

	static double gStartMs = 0.0;

	void markStart()
	{
		gStartMs = nowMs();
	}

	double msSinceStart()
	{
		return nowMs() - gStartMs;
	}

	double msSinceProcessCreation()
	{
		FILETIME creation, exitTime, kernel, user, now;
		GetProcessTimes(GetCurrentProcess(), &creation, &exitTime, &kernel, &user);
		GetSystemTimePreciseAsFileTime(&now);
		ULARGE_INTEGER c, n;
		c.LowPart = creation.dwLowDateTime; c.HighPart = creation.dwHighDateTime;
		n.LowPart = now.dwLowDateTime; n.HighPart = now.dwHighDateTime;
		return static_cast<double>(n.QuadPart - c.QuadPart) / 10000.0; //100ns単位 -> ms
	}

	void record(const std::string& metric, const std::string& label, double value)
	{
		records().push_back({ metric, label, value });
	}

	void writeCsv()
	{
		std::ofstream file(config().outPath);
		if (!file) return;
		file << "stage,run,metric,label,value\n";
		file << std::fixed << std::setprecision(4);
		for (const auto& r : records()) {
			file << config().stage << ',' << config().run << ',' << r.metric << ',' << r.label << ',' << r.value << '\n';
		}
	}

	ProcessMemory queryProcessMemory()
	{
		ProcessMemory result;
		PROCESS_MEMORY_COUNTERS_EX pmc = {};
		if (GetProcessMemoryInfo(GetCurrentProcess(), reinterpret_cast<PROCESS_MEMORY_COUNTERS*>(&pmc), sizeof(pmc))) {
			result.workingSet = pmc.WorkingSetSize;
			result.privateBytes = pmc.PrivateUsage;
		}
		return result;
	}

	ScopeTimer::ScopeTimer(std::string metric, std::string label)
		: mMetric(std::move(metric)), mLabel(std::move(label)), mStart(nowMs())
	{
	}

	ScopeTimer::~ScopeTimer()
	{
		if (config().enabled) record(mMetric, mLabel, nowMs() - mStart);
	}
}
