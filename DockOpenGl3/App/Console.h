#pragma once
#include <string>
#include <vector>

class  Console {

public:
	Console();
	void Add(const std::string& Str);
	void AddInfo(const std::string& Str);
	void AddWaring(const std::string& Str);
	void AddGreen(const std::string& Str);
	void RenderUI();


private:
	tm local_tm = {};
	void GetTime();
	std::string Time{};
	std::vector<int> IntVec{};
	std::vector<std::string> TV{};
	size_t TextSize = 0;
	size_t OldTextSize = 0;


};


