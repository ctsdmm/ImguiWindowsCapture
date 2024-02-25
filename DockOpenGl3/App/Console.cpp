#include "Console.h"
#include "../Imgui/imgui.h"
#include <chrono>

Console::Console()
{
	TV.reserve(100); 
	IntVec.reserve(100);
	AddGreen("系统已经启动");
}

void Console::Add(const std::string& Str)
{
	// 黑白   0
	GetTime();
	IntVec.push_back(0);
	TV.push_back(Time+Str);
}

void Console::AddInfo(const std::string& Str)
{
	// 3 Blue
	GetTime();
	IntVec.push_back(3);
	TV.push_back(Time + Str);
}

void Console::AddWaring(const std::string& Str)
{
	 // 1 red
	GetTime();
	IntVec.push_back(1);
	TV.push_back(Time + Str);
}

void Console::AddGreen(const std::string& Str)
{
	// 2 blue
	GetTime();
	IntVec.push_back(2);
	TV.push_back(Time + Str);
}

void Console::RenderUI()
{
	ImGui::Begin("控制台");
	if (ImGui::Button("Clear"))
	{
		TV.clear();
		IntVec.clear();
	}
	ImGui::SameLine();
	if (ImGui::Button("Test"))
	{
		TV.push_back("wdad");
	}
	ImGui::BeginChild("console");

	for (size_t i = 0; i < TV.size(); i++)
	{
	

		int Type = IntVec[i];
		if (Type ==0)
		{
			ImGui::Text(TV[i].c_str());
		}
		else  if (Type == 1)
		{
			ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), TV[i].c_str());
		}													   
		else  if (Type == 2)
		{													   
			ImGui::TextColored(ImVec4(0.0f, 1.0f,0.0f, 1.0f), TV[i].c_str());
		}													   
		else  if (Type == 3)
		{													   
			ImGui::TextColored(ImVec4(0.0f, 0.0f, 1.0f, 1.0f),TV[i].c_str());
		}													   
		

	}

	if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY() || OldTextSize != TV.size())
	{
		ImGui::SetScrollHereY();
	}
	OldTextSize = TV.size();
	ImGui::EndChild();
	ImGui::End();
}

void Console::GetTime()
{
	
	std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
	time_t tt = std::chrono::system_clock::to_time_t(now);
	localtime_s(&local_tm, &tt);

	Time = std::to_string(local_tm.tm_hour) + ":" + std::to_string(local_tm.tm_min) + ":" + std::to_string(local_tm.tm_sec) + "  ";
}
