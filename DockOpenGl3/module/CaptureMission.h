#pragma once
#include "../Imgui/imgui.h"
#include "../Imgui/imgui_stdlib.h"
#include "../Tools/GetWindowsInfo.h"
#include "../Tools/m_Imgui.h"
#include "../Capture/MyCapture.h"
#include "../App/Console.h"
#include <memory>
#include <string>
#include <vector>
#include <gl/GL.h>
class CaptureMission
{
public:
	 bool RenderUI();
	 std::shared_ptr<Console> m_Console;
	 void DeleteTexture();
private:
	int OldNum = 999;
	std::string Path;

	bool FirstGetInfo = true;
	std::vector<std::string> WindowsName;
	std::vector<HWND>WindowsHwnd;
	int WindowsName_Num = 0;
	bool ThreadOpen = false;

	bool DrawSingWindow = false;
	bool IsCaptureIng = false;
	//  已经拥有的文件名
	std::vector<std::string> FileName;
// 视频录制 
	int FrameCount = 0;

	int FPS = 60;
	std::string VideoName;
	// 用来录制视频
	std::unique_ptr< MyCapture> capture = nullptr;
	std::string DoingSth;
	private:

		/// 显示预览图片

	std::unique_ptr< MyCapture> VideoPreview = nullptr;
	bool IsShowTheImage2 = false;
	bool IsShowTheImage = false;
	GLuint Show_Image;
	void ShowImage();
	

};

