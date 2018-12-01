#pragma once

#include <type_traits>

template <typename T>
T Clamp(T value, T max, T min)
{
    if (value > max)
        return max;
    if (value < min)
        return min;
    return value;
}

template <typename EnumT>
auto CastToUnderlying(EnumT enumt)
{
    return typename std::underlying_type<EnumT>::type(enumt);
}
