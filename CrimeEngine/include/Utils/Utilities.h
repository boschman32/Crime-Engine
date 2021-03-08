#pragma once

namespace ce
{
	namespace utils
	{
		//From https://stackoverflow.com/questions/5878775/how-to-find-and-replace-string
		void ReplaceString(std::string& a_subject, const std::string& a_search, const std::string& a_replace);
	}
}