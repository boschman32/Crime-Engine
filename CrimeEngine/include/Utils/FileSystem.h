#pragma once

namespace ce
{
	class FileSystem
	{
	public:
		static bool ReadFileToString(const char* a_filepath, std::string& a_data);
		static bool ReadFileToBuffer(const char* a_filepath, const char*& a_buffer, size_t a_bufferSize);
		static bool SaveFile(const char* a_filepath, const char*& a_data);

		static bool ComparePath(const std::string& a_pathA, const std::string& a_pathB);
	};

	inline std::string ExtractFilenameFromPath(const std::string& a_filepath)
	{
		auto lastSlash = a_filepath.find_last_of("/\\");
    	lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;
    	const auto lastDot = a_filepath.rfind('.');
    	const auto count = lastDot == std::string::npos ? a_filepath.size() - lastSlash : lastDot - lastSlash;
    	return a_filepath.substr(lastSlash, count);
	}

	inline std::string ExtractFolderFromPath(const std::string& a_filepath)
	{
		size_t lastOf = a_filepath.find_last_of("\\");
        if (lastOf == std::string::npos)
        {
            lastOf = a_filepath.find_last_of("//");
            if (lastOf == std::string::npos)
            {
                CE_CORE_CRITICAL("Couldn't extract filename from path: {0}", a_filepath);
                return "";
            }
        }
        return a_filepath.substr(0, a_filepath.size() - a_filepath.substr(lastOf).size());
	}
}