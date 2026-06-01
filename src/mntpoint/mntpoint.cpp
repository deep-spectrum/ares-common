/**
 * @file mntpoint.cpp
 *
 * @brief
 *
 * @date 6/1/26
 *
 * @author Tom Schmitz \<tschmitz@andrew.cmu.edu\>
 */

#include <ares/mntpoint/mntpoint.hpp>
#include <cerrno>
#include <climits>

extern "C" {

#include <assert.h>
#include <mntent.h>
#include <sys/stat.h>

int stat_directory(const char *path, dev_t *dev) {
    assert(path);
    assert(dev);
    struct stat st {};
    int ret = stat(path, &st);
    if (ret != 0) {
        return -errno;
    }
    *dev = st.st_dev;
    return 0;
}

int get_mount(const char *path, char *dev, size_t dev_len) {
    assert(path);
    assert(dev);
    assert(dev_len > 0);
    dev_t dev_id;
    struct mntent *m, mnt;
    struct stat st;
    char strings[4096];

    int ret = stat_directory(path, &dev_id);
    if (ret < 0) {
        return ret;
    }

    FILE *mounts = setmntent("/proc/mounts", "r");
    if (mounts == NULL) {
        return -EACCES;
    }

    ret = -ENODATA;
    while ((m = getmntent_r(mounts, &mnt, strings, sizeof(strings))) != NULL) {
        if (stat(m->mnt_dir, &st) != 0) {
            continue;
        }

        if (st.st_dev == dev_id) {
            snprintf(dev, dev_len, "%s", m->mnt_fsname);
            ret = 0;
            break;
        }
    }
    endmntent(mounts);

    return ret;
}
}

namespace ares {
std::string find_mount_device(const std::string &path) {
    char dev[PATH_MAX] = "";

    if (path.empty()) {
        return "";
    }

    int ret = get_mount(path.c_str(), dev, sizeof(dev));
    if (ret < 0) {
        return "";
    }

    return std::string{dev};
}

bool mount_device_nvme(const std::string &path) {
    std::string dev = find_mount_device(path);

    if (dev.empty()) {
        return false;
    }

    const std::string prefix("/dev/nvme");
    return dev.compare(0, prefix.size(), prefix) == 0;
}
} // namespace ares
