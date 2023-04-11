#include <utils/FileUtils.h>
#include <core/Logging/ErrorLogger.h>

#include <vector>
#include <string>
#include <fstream>
#include <ios>
#include <iterator>
#include <algorithm>


void FileUtils::readFileLines(const char *filePath, std::vector<std::string>& resultFileLines)
{
    std::ifstream fileStream(filePath);

    if (!fileStream.is_open()) {
        throwDebugException("Failed to open file: \"" + (std::string)(filePath) + "\".");
    }

    // uint32_t fileLineCount = std::count(std::istream_iterator<char>(fileStream), std::istream_iterator<char>(), '\n');
    // resultFileLines.resize(fileLineCount);
    
    for (std::string line; getline(fileStream, line);) {
        resultFileLines.push_back(line);
    }

    fileStream.close();
}

void FileUtils::readFileChars(const char *filePath, bool binaryFile, std::vector<char>& resultFileChars)
{
    std::ifstream fileStream;
    if (binaryFile) {
        fileStream = std::ifstream(filePath, std::ios::binary | std::ios::ate);  // avoid text transformations and start at the end of the file.
    } else {
        fileStream = std::ifstream(filePath);
    }

    if (!fileStream.is_open()) {
        throwDebugException("Failed to open file: \"" + (std::string)(filePath) + "\".");
    }

    size_t fileSize = (size_t)fileStream.tellg();
    resultFileChars.resize(fileSize);

    fileStream.seekg(0);  // goto the start of the file.
    fileStream.read(resultFileChars.data(), fileSize);
    
    fileStream.close();
}
