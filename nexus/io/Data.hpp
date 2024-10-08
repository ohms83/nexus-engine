#ifndef __NXS_DATA_H__
#define __NXS_DATA_H__

#include "NxsDefine.h"

#include <memory>

NXS_NAMESPACE {
    /**
     *  @c Data object is designed to be a low overhead on copy. With this in mind,
     *  the copy assignment and constructor are marked deleted and the only way to transfered
     *  data is via move semantic and there can be no two codes holding the same @c Data 's object
     *  at the same time.
     */
    class Data
    {
    public:
        Data() noexcept {}
        /// Copy constructor (deleted)
        Data(const Data& rhs) = delete;
        /// Move constructor
        Data(Data&& rhs) noexcept;

        Data& operator = (Data&& rhs) noexcept;
        Data& operator = (const Data& rhs) = delete;

        bool isNull() const {
            return _buffer == nullptr;
        }

        /**
         * Copies data from the specified address.
         * @param data Pointer to an array where the storing data will be read from.
         * @param size Size of the data in byte.
         */
        void copy(char* data, uint64_t size);

        /**
         * Takes over the array of data pointed by @c data and manages it.
         * @warning The specified array's ownership will be taken over by this object.
         *          Please DO NOT delete it.
         * @warning The pointer must be dynamically allocated on heap. Please DO NOT
         *          pass the stack's pointer to this function because @c Data will try to destroy it
         *          in its destructor and that will cause memory corruption.
         */
        void take(char* data, uint64_t size);

        /**
         * Returns the ownership of currently managed buffer to the caller.
         * The caller will now responsible for managing the returned buffer.
         */
        char* give(uint64_t& outSize);

        const char* get() const {
            return _buffer.get();
        }

        uint64_t size() const {
            return _size;
        }

    private:
        std::unique_ptr<char> _buffer;
        /// Buffer size in bytes.
        uint64_t _size = 0;
    };
}

#endif // __NXS_DATA_H__
