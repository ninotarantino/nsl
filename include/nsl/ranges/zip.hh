/*  __    __   ______   __
 * |  \  |  \ /      \ |  \
 * | &&\ | &&|  &&&&&&\| &&
 * | &&&\| &&| &&___\&&| &&
 * | &&&&\ && \&&    \ | &&
 * | &&\&& && _\&&&&&&\| &&
 * | && \&&&&|  \__| &&| &&_____
 * | &&  \&&& \&&    &&| &&     \
 *  \&&   \&&  \&&&&&&  \&&&&&&&&
 *
 *     Nino Standard Library
 *
 * Author: Nino Tarantino
 * Purpose: C++20 support for std::zip
 */
#pragma once

#include "impl/zip_impl.hh"

namespace nsl {

inline constexpr detail::zip_factory zip;

} // namespace nsl
