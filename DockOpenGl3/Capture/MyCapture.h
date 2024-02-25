#pragma once
#include"AudioCapture/LoopbackCapture.h"
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Graphics.Capture.h>
#include <winrt/Windows.Graphics.DirectX.h>
#include <winrt/Windows.Graphics.DirectX.Direct3d11.h>
#include <windows.graphics.capture.interop.h>
#include <windows.graphics.directx.direct3d11.interop.h>
#include <Windows.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core/directx.hpp>
#include <wrl.h>
#include <d3d11.h>
#include <chrono>
#include <dxgi1_2.h>
#include <mutex>


class MyCapture
{
public:
	MyCapture();
	void InitCapture(const HWND& hwnd);
	cv::Mat GetCaptureImage();
	void SetFps(int x = 60) { FPS = x; TimeCur = 1000 / FPS; }
	void StartCapture();
	
	void Close();
	void Stop(std::string Name);

	std::mutex mutex;
	std::string Message="正在处理视频。。";
	bool ThreadOver;

	void GetSrv(ID3D11ShaderResourceView** srv)
	{
		std::lock_guard<std::mutex>lck(ImageMutex);
		//srv = Out_Srv.Get();
	//	Out_Srv.CopyTo(srv);
	}
	int ImageWidth = 0;
	int ImageHeight = 0;
private:

	void Ffpmeg(std::string Name, std::string& ThreadMessage,bool& ThreadOver);


	std::thread m_Thread;

	int FPS = 60;
	long long TimeCur;
	bool StartVideo = false;
	cv::VideoWriter videowirter;
	//Microsoft::WRL::ComPtr<cv::Mat>Res_Out;
	cv::Mat resMat;
	cv::Mat re;
	//Microsoft::WRL::ComPtr< ID3D11ShaderResourceView> Out_Srv;

	std::mutex ImageMutex;

	winrt::Windows::Graphics::SizeInt32 m_lastSize;

	HWND m_Hwnd;
	winrt::com_ptr<ID3D11Device> d3dDevice{ nullptr };
	
	winrt::Windows::Graphics::DirectX::Direct3D11::IDirect3DDevice device;
	cv::ocl::Context  m_oclCtx;
	winrt::Windows::Graphics::Capture::GraphicsCaptureItem m_item{ nullptr };
	winrt::Windows::Graphics::SizeInt32 ItemSize;
	winrt::com_ptr<ID3D11DeviceContext> d3dContext{ nullptr };

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;

	D3D11_TEXTURE2D_DESC desc1;

	/////////////// 


	winrt::Windows::Graphics::Capture::Direct3D11CaptureFramePool m_framePool{ nullptr };
	winrt::Windows::Graphics::Capture::GraphicsCaptureSession m_session{ nullptr };

	winrt::com_ptr<IDXGISwapChain1> m_swapChain{ nullptr };
	std::atomic<bool> m_closed = false;

	//  
	winrt::Windows::Graphics::Capture::Direct3D11CaptureFramePool::FrameArrived_revoker m_frameArrived;

	std::chrono::steady_clock::time_point StartPoint;
	std::chrono::steady_clock::time_point NextPoint;
	bool FirstGetPoint = true;
	void SetVideoFps();




	/// <summary>
	///  音频录制
	/// </summary>
	CLoopbackCapture AudioCapture;
	void OnFrameArrived(
		winrt::Windows::Graphics::Capture::Direct3D11CaptureFramePool const& sender,
		winrt::Windows::Foundation::IInspectable const& args);

	void CheckClosed()
	{
		if (m_closed.load() == true)
		{
			throw winrt::hresult_error(RO_E_CLOSED);
		}
	}
};

