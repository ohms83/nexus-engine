#include <nexus/NxsDefine.h>
#include <atomic>

NXS_NAMESPACE
{
    class UniqueID final
    {
    public:
        UniqueID() : m_id(s_uuid++) {}

        bool operator == (const UniqueID& rhs)
        {
            return m_id == rhs.m_id;
        }
        bool operator != (const UniqueID& rhs)
        {
            return m_id != rhs.m_id;
        }
        bool operator < (const UniqueID& rhs)
        {
            return m_id < rhs.m_id;
        }
        bool operator > (const UniqueID& rhs)
        {
            return m_id > rhs.m_id;
        }
        bool operator <= (const UniqueID& rhs)
        {
            return m_id <= rhs.m_id;
        }
        bool operator >= (const UniqueID& rhs)
        {
            return m_id >= rhs.m_id;
        }

    private:
        uint32_t m_id;
        static std::atomic<uint32_t> s_uuid;
    };
    
}