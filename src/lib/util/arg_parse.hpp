#ifndef MCH_ARG_PARSE
#define MCH_ARG_PARSE

#include <array>
#include <cstdarg>
#include <cstdio>
#include <ios>
#include <iostream>
#include <sstream>
#include <string>
#include <type_traits>

namespace mch
{
	using namespace std;

	enum class arg_type
	{
		BOOL = 0,
		CHAR = 1,
		FLOAT = 2,
		SIZE_T = 3
	};

	bool init(int& args_len, const int& EXPECTED_LEN, char** args);
	bool create_graph_file(char* dir_path, FILE*& file);
	string lowercase(char* data);
	bool open_file(FILE*& file, char* path, const char* mode);

	template <typename T>
	bool parse_arg(T& dest, char* const& argument, const char* arg_name);
	template <typename T>
	void parse_arg(T& dest, char* const& argument, const char* arg_name, T default_value);
	template <size_t dest_len>
	bool parse_args
	(
		array<arg_type, dest_len> dest_types,
		array<const char*, dest_len> arg_names,
		char** arguments, ...
	);

	template <typename T>
	inline bool parse_arg(T& dest, char* const& argument, const char* arg_name)
	{
		stringstream converter;

		if constexpr (is_same<T, char>::value)
		{
			size_t temp;
			converter << string(argument);
			converter >> temp;
			dest = char(temp);
		}
		else
		{
			converter << lowercase(argument);
			converter >> boolalpha >> dest;
		}

		if (!converter.fail())
		{
			cout << arg_name << ": " << boolalpha << dest << '\n';
			return true;
		}

		string arg_type_name;

		if constexpr (is_same<T, bool>::value)
			arg_type_name = "bool";
		else if constexpr (is_same<T, char>::value)
			arg_type_name = "char";
		else if constexpr (is_same<T, float>::value)
			arg_type_name = "float";
		else if constexpr (is_same<T, size_t>::value)
			arg_type_name = "size_t";
		else arg_type_name = "other type";
		
		printf("String %s could not be converted to %s.", argument, arg_type_name.c_str());
		return false;
	}

	template <typename T>
	inline void parse_arg(T& dest, char* const& argument, const char* arg_name, T default_value)
	{
		if (parse_arg(dest, argument, arg_name))
			return;
		dest = default_value;
		cout << arg_name << " set to default value: " << default_value << '\n';
	}

	template <size_t dest_len>
	inline bool parse_args
	(
		array<arg_type, dest_len> dest_types,
		array<const char*, dest_len> arg_names,
		char** arguments, ...
	)
	{
		bool success = false;
		stringstream converter;
		va_list dests;
		va_start(dests, arguments);

		for (size_t i = 0; i < dest_len; i++)
		{
			arg_type item_type = dest_types[i];
			switch (item_type)
			{
				case arg_type::BOOL:
					if (!parse_arg<bool>(*(va_arg(dests, bool*)), arguments[i], arg_names[i]))
						goto parse_args_finally;
					break;
				case arg_type::CHAR:
					if (!parse_arg<char>(*(va_arg(dests, char*)), arguments[i], arg_names[i]))
						goto parse_args_finally;
					break;
				case arg_type::FLOAT:
					if (!parse_arg<float>(*(va_arg(dests, float*)), arguments[i], arg_names[i]))
						goto parse_args_finally;
					break;
				case arg_type::SIZE_T:
					if (!parse_arg<size_t>(*(va_arg(dests, size_t*)), arguments[i], arg_names[i]))
						goto parse_args_finally;
					break;
			}
		}

		success = true;

	parse_args_finally:
		fputc('\n', stdout);
		va_end(dests);
		return success;
	}
}

#endif
