#pragma once

namespace Blah
{
	enum class Endian
	{
		Little,
		Big
	};

	template<class T>
	inline void swap_endian(T* value)
	{
		for (int i = 0; i < sizeof(T) / 2; i++)
		{
			char* _ptr = (char*)&value;
			char _temp = *(_ptr + i);
			*(_ptr + i) = *(_ptr + sizeof(T) - i - 1);
			*(_ptr + sizeof(T) - i - 1) = _temp;
		}
	}

	inline bool is_big_endian()
	{
		return (*((short*)"AB") == 0x4243);
	}

	inline bool is_little_endian()
	{
		return (*((short*)"AB") != 0x4243);
	}

	inline bool is_endian(const Endian& endian)
	{
		return
			(endian == Endian::Little && is_little_endian()) ||
			(endian == Endian::Big && is_big_endian());
	}
}