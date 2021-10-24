#include "File.hpp"
#include <sstream>
#include <iostream>

USING_NAMESPACE_NXS;
using namespace std;

File::File(const string& path, Type type, File::Mode mode)
{
    open(path, type, mode);
}

void File::open(const string& path, Type type, File::Mode mode)
{
    _type = type;
    _mode = mode;
    
    if(_stream.is_open()) {
        _stream.close();
    }

    ios::openmode streamMode;
    
    switch (mode)
    {
    case Mode::READ:
        streamMode = ios::in;
        break;
    case Mode::WRITE:
        streamMode = ios::out;
        break;
    case Mode::APPEND:
        streamMode = ios::app;
        break;
    }

    if (type == Type::BIN) {
        streamMode |= ios::binary;
    }

    // Move file pointer to the end so that we can easily get the file size.
    _stream.open(path, streamMode | ios::ate);
    
    if (!_stream.good())
    {
        std::stringstream ss;
        ss << "Unable to open file: " << path;
        std::cout << ss.str() << std::endl;
        return;
    }

    _size = (uint64_t)_stream.tellg();
    _stream.seekg(0);
}

bool File::isOpen() const
{
    return _stream.is_open();
}

Data File::read()
{
    if (_mode != Mode::READ) {
        // TODO: Error message
        return {};
    }
    
    uint64_t bufSize = (_type == Type::TEXT ? _size + 1 : _size);
    char* tmpBuf = new char[bufSize];
    _stream.read(tmpBuf, _size);
    
    if (_type == Type::TEXT) {
        // Append null-terminator for text files.
        tmpBuf[_size] = '\0';
    }
    
    Data data;
    data.take(tmpBuf, _size);
    return data;
}

uint64_t File::write(const Data& data)
{
    if (_mode != Mode::WRITE) {
        // TODO: Error message
        return 0;
    }
    
    uint64_t pos = (uint64_t)_stream.tellg();
    _stream.write(data.get(), _size);
    return (uint64_t)_stream.tellg() - pos;
}

Data File::readData(const std::string& path, Type type)
{
    File file(path, type, Mode::READ);
    return file.read();
}
