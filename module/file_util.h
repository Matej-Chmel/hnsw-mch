#pragma once
#include "mch.h"

namespace mch
{
	constexpr size_t READ_BYTES = 65536;

	template <typename T>
	T* load_file_data(const char* path, size_t& out_length);

	template <typename T>
	T* load_file_data(const char* path, size_t& out_length)
	{
		FILE* file = fopen(path, "rb");

		if(!file)
			return nullptr;

		T* current;
		vector<T*> buffers;
		size_t count = READ_BYTES / sizeof(T);
		size_t read;

		for(;;)
		{
			current = new T[count];
			read = fread(current, sizeof(T), count, file);

			if(read != count)
				break;

			buffers.push_back(current);
		}

		T* output = new T[buffers.size() * count + read];
		T* ptr = output;

		for(auto& buffer : buffers)
		{
			for(size_t i = 0; i < count; i++)
				*ptr++ = buffer[i];
			delete[] buffer;
		}

		for(size_t i = 0; i < read; i++)
			*ptr++ = current[i];
		delete[] current;

		fclose(file);
		return output;
	}
}
