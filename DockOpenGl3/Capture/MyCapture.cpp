#include "MyCapture.h"
#include "../Tools/GetWindowsInfo.h"
#include "direct3d11.interop.h"
#include"d3dHelpers.h"

#include <Oleacc.h>
MyCapture::MyCapture()
{
	auto Device1 = CreateD3DDevice();
	auto dxgiDevice = Device1.as<IDXGIDevice>();
	device = CreateDirect3DDevice(dxgiDevice.get());

	// Set up 
	d3dDevice = GetDXGIInterfaceFromObject<ID3D11Device>(device);
	d3dDevice->GetImmediateContext(d3dContext.put());




	this->m_oclCtx = cv::directx::ocl::initializeContextFromD3D11Device(d3dDevice.get());
}

void MyCapture::InitCapture(const HWND& hwnd)
{
	m_Hwnd = hwnd;
	if (m_session != nullptr)
	{
		m_session.Close();
	}
	auto activation_factory = winrt::get_activation_factory<winrt::Windows::Graphics::Capture::GraphicsCaptureItem>();
	auto interop_factory = activation_factory.as<IGraphicsCaptureItemInterop>();
	interop_factory->CreateForWindow(m_Hwnd, winrt::guid_of<ABI::Windows::Graphics::Capture::IGraphicsCaptureItem>(), reinterpret_cast<void**>(winrt::put_abi(this->m_item)));

	ItemSize = m_item.Size();

	m_lastSize = ItemSize;
	m_swapChain = CreateDXGISwapChain(
		d3dDevice,
		static_cast<uint32_t>(ItemSize.Width),
		static_cast<uint32_t>(ItemSize.Height),
		static_cast<DXGI_FORMAT>(winrt::Windows::Graphics::DirectX::DirectXPixelFormat::B8G8R8A8UIntNormalized),
		2);

	// Create framepool, define pixel format (DXGI_FORMAT_B8G8R8A8_UNORM), and frame size. 
	m_framePool = winrt::Windows::Graphics::Capture::Direct3D11CaptureFramePool::CreateFreeThreaded (
		device,
		winrt::Windows::Graphics::DirectX::DirectXPixelFormat::B8G8R8A8UIntNormalized,
		2,
		ItemSize);

	m_session = m_framePool.CreateCaptureSession(m_item);
	m_frameArrived = m_framePool.FrameArrived(winrt::auto_revoke, { this, &MyCapture::OnFrameArrived });
	CheckClosed();
	m_session.StartCapture();
}

cv::Mat MyCapture::GetCaptureImage()
{

		std::lock_guard<std::mutex>lck(ImageMutex);
		return  resMat.clone();

	
}

void MyCapture::StartCapture()
{
	std::string Path = "Video//TTTTDEO.mp4";

	int fourcc = cv::VideoWriter::fourcc('m', 'p', '4', 'v');
	videowirter.open(Path, -1, FPS, cv::Size(ItemSize.Width, ItemSize.Height), true);

	std::string path = "Video//TTTTDEO.wav";
	DWORD h;
	GetWindowThreadProcessId(m_Hwnd, &h);

	std::wstring wstr = Str_To_Wstr(path);

	StartVideo = true;
	AudioCapture.StartCaptureAsync(h, true, wstr.c_str());


}

void MyCapture::Close()
{
	auto expected = false;
	if (m_closed.compare_exchange_strong(expected, true))
	{
		m_frameArrived.revoke();
		m_framePool.Close();
		m_session.Close();

		m_swapChain = nullptr;
		m_framePool = nullptr;
		m_session = nullptr;
		m_item = nullptr;
	}
}

void MyCapture::Stop(std::string Name)
{
	AudioCapture.StopCaptureAsync();
	StartVideo = false;
	videowirter.release();
	CloseHandle(AudioCapture.hand);

	// 创建一个线程 
	ThreadOver = false;
	m_Thread = std::thread(&MyCapture::Ffpmeg,this,Name, std::ref(Message),std::ref(ThreadOver));
	m_Thread.detach();
	//char line[1024];
	//std::string st = ".\\module\\FFpmeg\\ffmpeg.exe -i Video//TTTTDEO.wav -i Video//TTTTDEO.mp4  -safe 0 -c:v copy -c:a aac -strict experimental Video//"+ Name + ".mp4";
	//FILE* fp = _popen(st.c_str(), "r");


}

void MyCapture::SetVideoFps()
{
	if (FirstGetPoint)
	{
		StartPoint = std::chrono::steady_clock::now();
	}
	else
	{

		NextPoint = std::chrono::steady_clock::now();
		// 微秒
		auto x = std::chrono::duration_cast<std::chrono::milliseconds>(NextPoint - StartPoint).count();
		if (x < TimeCur)
		{
			Sleep(TimeCur-x);
		}
		StartPoint = NextPoint;
	}

	FirstGetPoint = false;

}

void MyCapture::OnFrameArrived(winrt::Windows::Graphics::Capture::Direct3D11CaptureFramePool const& sender, winrt::Windows::Foundation::IInspectable const& args)
{
	auto start = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now()).time_since_epoch().count();

	auto newSize = false;
	{
		auto frame = sender.TryGetNextFrame();

		auto frameContentSize = frame.ContentSize();

		if (frameContentSize.Width != m_lastSize.Width ||
			frameContentSize.Height != m_lastSize.Height)
		{
		
			newSize = true;
			m_lastSize = frameContentSize;
			m_swapChain->ResizeBuffers(
				2,
				static_cast<uint32_t>(m_lastSize.Width),
				static_cast<uint32_t>(m_lastSize.Height),
				static_cast<DXGI_FORMAT>(winrt::Windows::Graphics::DirectX::DirectXPixelFormat::B8G8R8A8UIntNormalized),
				0);
		}


		{
			auto frameSurface = GetDXGIInterfaceFromObject<ID3D11Texture2D>(frame.Surface());

			winrt::com_ptr<ID3D11Texture2D> backBuffer;
			winrt::check_hresult(m_swapChain->GetBuffer(0, winrt::guid_of<ID3D11Texture2D>(), backBuffer.put_void()));

			d3dContext->CopyResource(backBuffer.get(), frameSurface.get());

			if (backBuffer != nullptr)
			{
				std::lock_guard<std::mutex>lck(ImageMutex);
				cv::Mat Ou;
				cv::directx::convertFromD3D11Texture2D(backBuffer.get(), Ou);
				if (StartVideo)
				{
					videowirter.write(resMat.clone());
				}
				resMat = Ou.clone();
				Ou.release();
			}
		}
		
	}

	
	if (m_swapChain!=nullptr)
	{
		DXGI_PRESENT_PARAMETERS presentParameters = { 0 };
		m_swapChain->Present1(1, 0, &presentParameters);
	}


	// 微秒
	auto end = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now()).time_since_epoch().count();
	auto x = end - start;
	if (x < TimeCur)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(TimeCur-x));
	}

	if (newSize)
	{
		m_framePool.Recreate(
			device,
			winrt::Windows::Graphics::DirectX::DirectXPixelFormat::B8G8R8A8UIntNormalized,
			2,
			m_lastSize);
	}

}

void MyCapture::Ffpmeg(std::string Name, std::string& ThreadMessage, bool& ThreadOver)
{
	char line[1024];
	std::string str = ".";
	std::string st = ".\\module\\FFpmeg\\ffmpeg.exe -i Video//TTTTDEO.wav -i Video//TTTTDEO.mp4  -safe 0 -c:v copy -c:a aac -strict experimental Video//" + Name + ".mp4";
	FILE* fp = _popen(st.c_str(), "r");
	int x = 1;
	while (fgets(line, 1024, fp))
	{
		mutex.lock();
		for (size_t i = 0; i < x; i++)
		{
			str += ".";
		}
		str = ThreadMessage + str;
		mutex.unlock();
			
	}
	Sleep(2000);
	remove("Video//TTTTDEO.mp4");
	remove("Video//TTTTDEO.wav");
	mutex.lock();
	ThreadMessage = "视频处理完成";
	ThreadOver = true;
	mutex.unlock();
}
