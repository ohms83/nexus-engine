#include "File.hpp"

USING_NAMESPACE_NXS;
using namespace std;

File::File(const string& path, Type type, File::Mode mode)
{
    open(path, type, mode);
}

void File::open(const string& path, Type type, File::Mode mode)
{
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
    if (!_stream.good()) {
        throw new runtime_error(string("Unable to open file: ").append(path));
    }

    _size = (uint64_t)_stream.tellg();
    _stream.seekg(0);
}

Data File::read()
{
    char* tmpBuf = new char[_size];
    _stream.read(tmpBuf, _size);
    
    Data data;
    data.set(tmpBuf, _size);
    return data;
}

uint64_t File::write(const Data& data)
{
    uint64_t pos = (uint64_t)_stream.tellg();
    _stream.write(data.get(), _size);
    return (uint64_t)_stream.tellg() - pos;
}

Data File::readData(const std::string& path, Type type)
{
    File file(path, type, Mode::READ);
    return file.read();
}