#include "String.hpp"

#include <cstring>

USING_NAMESPACE_NXS;
using namespace std;

const string String::DELIMITERS = " \f\n\r\t\v";

string String::trim(const string& original, const string delimiters)
{
    string result = trimLeft(original, delimiters);
    return trimRight(result, delimiters);
}

string String::trimLeft(const string& original, const string delimiters)
{
    string result = original;
    size_t index = original.find_first_not_of(delimiters);
    
    if(index != string::npos)
    {
        result = original.substr(index);
    }

    return result;
}

string String::trimRight(const string& original, const string delimiters)
{
    string result = original;
    size_t index = original.find_last_not_of(delimiters);
    
    if(index != string::npos)
    {
        result = original.substr(index);
    }

    return result;
}

vector<string> String::split(const string& str, const std::string& delimeters)
{
    vector<string> tokens;
    
    if (!str.empty())
    {
        size_t bufSize = str.size() + 1;
        char* tmpBuf = new char[bufSize];
        memcpy(tmpBuf, str.c_str(), bufSize);
        tmpBuf[bufSize - 1] = '\0';
        
        const char* substr = strtok(tmpBuf, delimeters.c_str());
        while (substr)
        {
            tokens.emplace_back(substr);
            substr = strtok(nullptr, delimeters.c_str());
        }
        
        delete [] tmpBuf;
    }
    
    return tokens;
}

std::string String::join( const std::vector<std::string>& stringList, const std::string& separator )
{
    std::stringstream ss;
    const size_t numStr = stringList.size();
    
    for (size_t i = 0; i < numStr; ++i)
    {
        const string& str = stringList[i];
        
        ss << str;
        if( i != numStr - 1) {
            ss << separator;
        }
    }
    
    return ss.str();
}
