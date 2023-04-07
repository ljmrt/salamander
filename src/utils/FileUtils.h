#ifndef FILEUTILS_H
#define FILEUTILS_H

namespace FileUtils
{
    // read the file lines from a file path.
    //
    // @param filePath file path to read from.
    // @param resultFileLines stored read file lines.
    void readFileLines(const char *filePath, std::vector<std::string>& resultFileLines);

    // read the file characters from a file path.
    //
    // @param filePath file path to read from.
    // @param resultFileChars stored read file chars.
    void readFileChars(const char *filePath, std::vector<char>& resultFileChars);
}

#endif  // FILEUTILS_H
