/**
 * @file mntpoint.hpp
 *
 * @brief
 *
 * @date 6/1/26
 *
 * @author Tom Schmitz \<tschmitz@andrew.cmu.edu\>
 */

#ifndef ARES_MNTPOINT_HPP
#define ARES_MNTPOINT_HPP

#include <string>

namespace ares {
std::string find_mount_device(const std::string &path);
bool mount_device_nvme(const std::string &path);
} // namespace ares

#endif // ARES_MNTPOINT_HPP
