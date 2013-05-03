#include "StdAfx.h"
#include "DirTraversing.h"

namespace indexcleaner
{
	bool DirTraversing::LoadDir(const string &dir_path, const string &file_type)
	{
		files_.clear();
		WIN32_FIND_DATAA FindFileData;
		HANDLE hFind;

		string slash_tmp = "";
		if (dir_path[dir_path.size()-1] != '\\' && dir_path[dir_path.size()-1] != '/')
		{
			slash_tmp = '\\';
		}
		hFind = FindFirstFileA((dir_path+slash_tmp+file_type).c_str(), &FindFileData);
		if (hFind == INVALID_HANDLE_VALUE)
		{
			return false;
		}
		do
		{
			files_.push_back(FindFileData.cFileName);
		} while(FindNextFileA(hFind, &FindFileData));

		FindClose(hFind);

		return true;
	}
}