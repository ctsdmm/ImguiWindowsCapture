#pragma once
#include <string>
#include <vector>
#include <io.h> 

// 获取指定路径下指定后缀格式的 文件名
inline void getFileName(std::string path, std::vector<std::string>& files, std::string Flags = "")
{
	bool Flag = false;
	if (Flags.size() >= 1)
	{
		Flag = true;
	}
	long long hFile = 0;
	struct _finddata_t fileinfo;
	std::string p;
	if ((hFile = _findfirst(p.assign(path).append("/*").c_str(), &fileinfo)) != -1)
	{
		do
		{
			if (!(fileinfo.attrib & _A_SUBDIR))
			{
				if (Flag)
				{
					std::string Str(fileinfo.name);
					if (Str.substr(Str.size() - Flags.size(), Str.size()) == Flags)
					{
						files.push_back(fileinfo.name);
					}
				}
				else
				{
					files.push_back(fileinfo.name);
				}

			}

		} while (_findnext(hFile, &fileinfo) == 0);

		_findclose(hFile);
	}
}
