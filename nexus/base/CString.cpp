//
//  CString.cpp
//  Nexus
//
//  Created by nuttachai on 4/11/21.
//

#include "CString.hpp"

USING_NAMESPACE_NXS;

const char CString::DELIMITERS[] = " \f\n\r\t\v";

char* CString::getLine(char* str)
{
    bool isEOL = false;
    char* ch = str;
    const size_t len = strlen(str);
    
    for (size_t i = 0; i < len; ++i)
    {
        bool isLineBreak = (*ch == '\n') || (*ch == '\r');
        
        if (isLineBreak) {
            *ch = '\0';
            isEOL = true;
        }
        else if (isEOL) {
            break;
        }
        
        ++ch;
        if (*ch == '\0') {
            return nullptr;
        }
    }
    
    return ch;
}

std::vector<char*> CString::getLines(char* str)
{
    char* line = str;
    std::vector<char*> lines;

    while(line)
    {
        char* eol = getLine(line);
        if (line) {
            lines.emplace_back(line);
        }
        
        line = eol;
    }
    
    return lines;
}


void CString::iterateLines(char* str, std::function<bool(char*)> task)
{
    char* line = str;
    
    while(line)
    {
        char* eol = getLine(line);
        if (!task(line)) {
            return;
        }
        
        line = eol;
    }
}

std::vector<char*> CString::split(char* str, const char* delimeters, bool joinDelim)
{
    std::vector<char*> tokens;
    const size_t len = strlen(str);
    const size_t numDelim = strlen(delimeters);
    char* token = str;
    char* ch = str;
    const char* endStr = str + len;
    
    auto isDelim = [delimeters, numDelim] (char c) -> bool {
        for (size_t i = 0; i < numDelim; ++i)
        {
            if (c == delimeters[i])
            {
                return true;
            }
        }
        return false;
    };
    
    while (endStr != ch)
    {
        if (isDelim(*ch))
        {
            // Scan through all the delimeters
            while(endStr != ch && isDelim(*ch))
            {
                *ch = '\0';
                ++ch;
                // Immediately break out of the loop if joinDelim is false.
                if (!joinDelim) {
                    break;
                }
            }
            
            tokens.emplace_back(token);
            token = ch;
        }
        else
        {
            ++ch;
        }
    }
    
    if (token) {
        tokens.emplace_back(token);
    }
    
    return tokens;
}

char* CString::trim(char* str, const char* delimeters)
{
    return trimLeft(trimRight(str, delimeters), delimeters);
}

char* CString::trimLeft(char* str, const char* delimeters)
{
    while (str && *str != '\0')
    {
        if (!strchr(delimeters, (int)*str)) {
            break;
        }
        
        ++str;
    }
    
    return str;
}

char* CString::trimRight(char* str, const char* delimeters)
{
    char* endStr = str + strlen(str);
    while (endStr && endStr != str)
    {
        char ch = *endStr;
        
        if (strchr(delimeters, (int)ch)) {
            *endStr = '\0';
        }
        else if (ch != '\0') {
            break;
        }
        
        --endStr;
    }
    
    return str;
}
