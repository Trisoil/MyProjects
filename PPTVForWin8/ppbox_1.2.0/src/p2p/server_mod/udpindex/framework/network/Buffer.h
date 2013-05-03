#pragma once

namespace framework
{
	struct buffer
	{
		shared_array<char> data;
		size_t length;
	};
}