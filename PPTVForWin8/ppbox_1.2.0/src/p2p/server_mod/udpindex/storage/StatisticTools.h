#pragma once

/************************************************************************
// 
// Filename: StatisticTools.h
// Comment:  一些做统计用的函数或结构
// Date:	 2008-9-2
//
************************************************************************/

namespace storage
{
	// 统计命中率
	struct HitRate
	{
		UINT64 from_mem;
		UINT64 from_disk;
		HitRate() : from_mem(0), from_disk(0) {}
		float GetHitRate() { size_t r = (from_mem+from_disk)==0 ? 0:float(from_mem)/(from_mem+from_disk)*100;return r; }
	};

	struct MemoryPercentage
	{
		float mem_percent_;

		MemoryPercentage() : mem_percent_(0.0) {}
	};

} // namespace storage