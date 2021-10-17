#pragma once
#include "nxsMacros.h"

#include <memory>
#include <fstream>
#include <string>

#include "Data.hpp"

NXS_NAMESPACE {
    class File
    {
    public:
        enum Type
        {
            /// Binary file
            BIN,
            /// Text file
            TEXT,
        };

        enum Mode
        {
            READ,
            WRITE,
            APPEND,
        };

        File() {}
        File(const std::string& path, Type type, Mode mode = Mode::READ);

        void open(const std::string& path, Type type, Mode mode = Mode::READ);
        /**
         *  Reads and return the data from the currently opened file.
         */
        Data read();
        /**
         *  Writes the specified data to currently opened file.
         */
        uint64_t write(const Data& data);

        uint64_t getSize() const {
            return _size;
        }

        /// Opens and returns the data read from specified path.
        static Data readData(const std::string& path, Type type);

    private:
        std::fstream _stream;
        uint64_t _size = 0;
    };
}