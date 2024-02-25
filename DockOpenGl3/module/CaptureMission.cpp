#include "CaptureMission.h"
#include "../Tools/Tools.h"
#include <Windows.h>
#include <iostream>

#include <algorithm>

#include "../App/App.h"

bool CaptureMission::RenderUI()
{
	

	// 微秒
	ImGui::Begin("录制");
	if (FirstGetInfo)
	{
		auto info = EnumerateWindows();
		WindowsName.clear();
		WindowsHwnd.clear();
		for (size_t i = 0; i < info.size(); i++)
		{
			WindowsName.push_back(info[i].StrTitle());
			WindowsHwnd.push_back(info[i].Hwnd());
		}
		FirstGetInfo = false;
	}
	
	
	m_Imgui_Combo("窗口名", WindowsName, WindowsName_Num);
	if (ImGui::Button("刷新"))
	{
		FirstGetInfo = true;
	}
	ImGui::SameLine();
	/// 显示预览图片 

	ImGui::Checkbox("预览", &IsShowTheImage);
	{
		if (IsShowTheImage)
		{
			if (OldNum!= WindowsName_Num)
			{
				if (VideoPreview!=nullptr)
				{
					VideoPreview->Close();
					FrameCount = 0;
					IsShowTheImage2 = false;
					VideoPreview=nullptr;
				}
				VideoPreview = std::make_unique<MyCapture>();
				VideoPreview->SetFps(30);
				VideoPreview->InitCapture(WindowsHwnd[WindowsName_Num]);
				OldNum = WindowsName_Num;
				
			}
		}
	
	}


	ImGui::InputInt("FPS", &FPS);
	ImGui::InputText("视频名", &VideoName);
	if (ImGui::Button("开始录制"))
	{
		if (!IsCaptureIng)
		{
			getFileName("Video\\", FileName, ".mp4");

			std::vector<std::string>::iterator it = std::find(FileName.begin(), FileName.end(), VideoName + ".mp4");

			if (it != FileName.end())
			{
				//  输出  有重名
				m_Console->AddInfo("有重名");
			}
			else
			{
				IsCaptureIng = true;
				if (capture!=nullptr)
				{
					capture.release();
				
				}
				capture = std::make_unique<MyCapture>();
				capture->SetFps(FPS);
				capture->InitCapture(WindowsHwnd[WindowsName_Num]);
				OldNum = WindowsName_Num;
				capture->StartCapture();
				DoingSth = "正在录制中..";
			}
		}
		else
		{
			m_Console->AddInfo("正在录制中。。。");
		}
	}
	// 判断是否存在
	  //首先找到所有文件名
	
	ImGui::SameLine();

	 if (ImGui::Button("录制结束"))
	{
		if (IsCaptureIng)
		{
			capture->Stop(VideoName);
			ThreadOpen = true;
			IsCaptureIng = false;
			DoingSth = "录制结束..";
		}
		else
		{
			m_Console->AddInfo("没有开始录制");
		}
		
	}
	if (ThreadOpen)
	{
		capture->mutex.lock();

		//	m_Console->AddGreen(capture->Message);
		DoingSth = capture->Message;
		ThreadOpen = !capture->ThreadOver;
		capture->mutex.unlock();
	}


	//  状态
	ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "状态:"); ImGui::SameLine();
	ImGui::TextColored(ImVec4(0.0f, 1.0f, 1.0f, 1.0f), DoingSth.c_str());
	//  状态


	ImGui::InputText("文件名",&Path);

	if (ImGui::Button("Start!"))
	{
		DrawSingWindow = !DrawSingWindow;
	} 

	





	ImGui::End();



	if (IsShowTheImage&&IsShowTheImage2)
	{



		ImGui::Begin("Show");
		cv::Mat Texture = VideoPreview->GetCaptureImage();

		if (Texture.data!=NULL)
		{
			glGenTextures(1, &Show_Image);
			glBindTexture(GL_TEXTURE_2D, Show_Image);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Texture.cols, Texture.rows, 0, GL_RGBA, GL_UNSIGNED_BYTE, Texture.data);
			ImGui::Image(reinterpret_cast<void*>(static_cast<intptr_t>(Show_Image)), ImVec2(Texture.cols, Texture.rows));
		}
		
		ImGui::End();
		Texture.release();
		
	}



	if (DrawSingWindow)
	{
		// 绘制一个小窗 并且置顶
		const ImGuiViewport* viewport = ImGui::GetMainViewport();
		const ImVec2 base_pos = viewport->Pos;

		// By default, Windows are uniquely identified by their title.
		// You can use the "##" and "###" markers to manipulate the display/ID.

		// Using "##" to display same title but have unique identifier.
		ImGui::SetNextWindowPos(ImVec2(base_pos.x + 100, base_pos.y + 100), ImGuiCond_FirstUseEver);

		ImGui::Begin("Recording");
		ImGui::Text("Please Press F2 To Record The Misson");
		ImGui::End();

		HWND hwnd = FindWindowW(NULL, L"Recording");
		SetWindowPos(hwnd, HWND_TOPMOST,0,0,100,50, SWP_NOMOVE|SWP_NOSIZE);

	}

	


	FrameCount < 1000 ? FrameCount++ : FrameCount = 0;
	if (FrameCount>50)
	{
		IsShowTheImage2 = true;
	}
	return true;
}

void CaptureMission::DeleteTexture()
{
	glDeleteTextures(1, &Show_Image);
}

void CaptureMission::ShowImage()
{
	ImGui::Begin("预览");

	ImGui::End();
}
