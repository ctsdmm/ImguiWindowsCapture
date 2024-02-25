#pragma once
#include <dwmapi.h>
#include <string>
#include<algorithm>

#include <array>
#include <vector>
#include <locale>
#include <codecvt>
inline std::string Wstr_To_str(const std::wstring& wstr)
{

	if (wstr.empty()) 
		return std::string();
	int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
	std::string strTo(size_needed, 0);
	WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
	return strTo;

}

inline std::wstring Str_To_Wstr(const std::string& wstr)
{

	if (wstr.empty())
		return std::wstring();
	int size_needed = MultiByteToWideChar(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0);
	std::wstring strTo(size_needed, 0);
	MultiByteToWideChar(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed);
	return strTo;

}

struct Window
{
public:
	Window(nullptr_t) {}
	Window(HWND hwnd, std::wstring const& title, std::wstring& className)
	{
		m_hwnd = hwnd;
		m_title = title;
		m_className = className;
		this->title = Wstr_To_str(m_title);
	}

	HWND Hwnd() const noexcept { return m_hwnd; }
	std::wstring Title() const noexcept { return m_title; }
	std::wstring ClassName() const noexcept { return m_className; }
	std::string StrTitle() const noexcept { return title; }
	void SetStrTitle(const std::string& str)  { title=str	; }

private:
	HWND m_hwnd;
	std::string title;

	std::wstring m_title;
	std::wstring m_className;

};

inline std::wstring GetClassName(HWND hwnd)
{
	std::array<WCHAR, 1024> className;


	::GetClassName(hwnd, className.data(), (int)className.size());

	std::wstring title(className.data());
	return title;
}

inline std::wstring GetWindowText(HWND hwnd)
{
	std::array<WCHAR, 1024> windowText;

	::GetWindowText(hwnd, windowText.data(), (int)windowText.size());

	std::wstring title(windowText.data());
	return title;
}

inline bool IsAltTabWindow(Window const& window)
{
	HWND hwnd = window.Hwnd();
	HWND shellWindow = GetShellWindow();

	auto title = window.Title();
	auto className = window.ClassName();

	if (hwnd == shellWindow)
	{
		return false;
	}

	if (title.length() == 0)
	{
		return false;
	}

	if (!IsWindowVisible(hwnd))
	{
		return false;
	}

	if (GetAncestor(hwnd, GA_ROOT) != hwnd)
	{
		return false;
	}

	LONG style = GetWindowLong(hwnd, GWL_STYLE);
	if (!((style & WS_DISABLED) != WS_DISABLED))
	{
		return false;
	}

	DWORD cloaked = FALSE;
	HRESULT hrTemp = DwmGetWindowAttribute(hwnd, DWMWA_CLOAKED, &cloaked, sizeof(cloaked));
	if (SUCCEEDED(hrTemp) &&
		cloaked == DWM_CLOAKED_SHELL)
	{
		return false;
	}

	return true;
}

inline BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
	auto class_name = GetClassName(hwnd);
	auto title = GetWindowText(hwnd);

	auto window = Window(hwnd, title, class_name);

	if (!IsAltTabWindow(window))
	{
		return TRUE;
	}

	std::vector<Window>& windows = *reinterpret_cast<std::vector<Window>*>(lParam);
	int num = 1;
	std::string name = window.StrTitle();

	for (size_t i = 0; i < windows.size(); i++)
	{
		if (windows[i].StrTitle()==name)
		{
			name += std::to_string(num++);
			i = 0;
		}
	}
	window.SetStrTitle(name);



	windows.push_back(window);

	return TRUE;
}

inline const std::vector<Window> EnumerateWindows()
{
	std::vector<Window> windows;
	EnumWindows(EnumWindowsProc, reinterpret_cast<LPARAM>(&windows));
	return windows;
}

