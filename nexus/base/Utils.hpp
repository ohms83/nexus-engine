#ifndef __NXS_UTILS_H__
#define __NXS_UTILS_H__

#include <utility>
#include <iostream>

template<typename T1, typename T2>
inline std::ostream& operator << (std::ostream& ss, std::pair<T1, T2> value)
{
    ss << "(" << value.first << ", " << value.second << ")";
    return ss;
}

#endif // __NXS_UTILS_H__