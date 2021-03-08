#include "cepch.h"
#include "Utils/Utilities.h"

namespace ce
{
	namespace utils
	{
		void ReplaceString(std::string& a_subject, const std::string& a_search, const std::string& a_replace)
		{
			size_t pos = 0;
			while((pos = a_subject.find(a_search, pos)) != std::string::npos)
			{
				a_subject.replace(pos, a_search.length(), a_replace);
				pos += a_replace.length();
			}
		}
	}
}