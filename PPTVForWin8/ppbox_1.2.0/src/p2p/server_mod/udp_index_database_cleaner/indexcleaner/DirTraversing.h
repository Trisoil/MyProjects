#pragma once
#include "stdafx.h"
#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>

namespace indexcleaner
{
	class DirTraversing
		: public boost::noncopyable
		, public boost::enable_shared_from_this<DirTraversing>
	{
	public:
		typedef boost::shared_ptr<DirTraversing> p;
		static p Create() { return p(new DirTraversing()); }
	public:
		bool LoadDir(const string &dir_path, const string &file_type);
		inline const vector<string> &GetFiles(){return files_;};
		inline bool IsExist(string str){return std::find(files_.begin(),files_.end(),str) != files_.end();};

	private:
		vector<string> files_;
	private:
		DirTraversing(void){};
	};
}