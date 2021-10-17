#include "Path.hpp"

#include <regex>
#include <iostream>

USING_NAMESPACE_NXS;

using namespace std;

const string Path::SEPARATOR = "/";
const string Path::ROOT = ".";
const string Path::TOP_FOLDER = "..";


Path::Path(Path&& rhs)
{
    _path = std::move(rhs._path);
}

Path::Path(const std::string& path)
{
    _path = path;
}

Path::Path(std::string&& path)
{
    _path = std::move(path);
}
    
Path& Path::operator = (Path&& rhs)
{
    _path = std::move(rhs._path);
    return *this;
}
Path& Path::operator = (const std::string& path)
{
    _path = path;
    return *this;
}

Path& Path::operator = (std::string&& path)
{
    _path = std::move(path);
    return *this;
}

const string& Path::normalize()
{
    // TODO: Implementation.
    throw new runtime_error("Unimplemented function!");
    return _path;
}

string Path::filename(bool withExt) const
{
    const auto extIndex = _path.find_last_of(".");
    const auto pathLen  = _path.size();

    if (pathLen == 0 || extIndex == pathLen - 1) {
        return string();
    }

    auto headTailPair = split();
    return (withExt ? headTailPair.second : Path(headTailPair.second).splitExt().first);
}

string Path::extension() const
{
    const auto extIndex = _path.find_last_of(".");
    const auto pathLen  = _path.size();

    // If path ends with '.' or SEPARATOR, or the extension cannot be found, returns an empty string.
    if (pathLen == 0 || extIndex == pathLen - 1 || extIndex == string::npos) {
        return string();
    }

    auto headTailPair = split();
    return Path(headTailPair.second).splitExt().second;
}

pair<string, string> Path::split() const
{
    if (_path.empty()) {
        return pair<string, string>("", "");
    }

    const auto sepIndex  = _path.find_last_of(SEPARATOR); 
    const auto pathLen   = _path.size();
    const auto sepLen    = SEPARATOR.size();
    const auto tailIndex = std::min(pathLen, sepIndex + sepLen);

    // If there's only one separator and it's a leading separator, it will be included in head.
    if (sepIndex == 0) {
        return pair<string, string>(_path.substr(0, sepLen), _path.substr(tailIndex));
    }

    // If there's no SEPARATOR, head will be empty
    if (sepIndex == string::npos) {
        return pair<string, string>(string(), _path);
    }
    
    return pair<string, string>(_path.substr(0, sepIndex), _path.substr(tailIndex));
}

pair<string, string> Path::splitExt() const
{
    if (_path.empty()) {
        return pair<string, string>("", "");
    }

    const auto dotIndex = _path.find_last_of(".");
    const auto pathLen  = _path.size();
    
    // If dotIndex is 0 or period cannot be found
    if (dotIndex == 0 || dotIndex == string::npos) {
        return pair<string, string>(_path, "");
    }
    
    // If the path ends with SEPARATOR or the name only contains period
    const auto sepIndex = _path.find_last_of(SEPARATOR);
    if (sepIndex == pathLen - 1 || sepIndex == dotIndex - 1) {
        return pair<string, string>(_path, "");
    }
    
    // From dotIndex, seaching for filename's index
    int64_t filenameIndex = dotIndex;
    for (int64_t i = dotIndex - 1; i >= 0 && i != sepIndex; --i)
    {
        const char& c = _path.at(i);
        if (c != '.') {
            filenameIndex = i;
        }
    }
    
    return (filenameIndex != dotIndex ? pair<string, string>(_path.substr(0, dotIndex), _path.substr(dotIndex)) : pair<string, string>(_path, ""));
}