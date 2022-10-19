#include <sys/inotify.h>
#include <unistd.h>
#include <string>
#include <cstdio>

/*
struct inotify_event {
   int      wd;       // Watch descriptor
   uint32_t mask;     // Mask of events
   uint32_t cookie;   // Unique cookie associating related  events (for
rename(2)) uint32_t len;      // Size of name field char     name[];   //
Optional null-terminated name
};
*/

int process_inotify_events(int fd) {
    char buf[512];
    struct inotify_event *event;
    int event_size = sizeof(struct inotify_event);

    // 检测事件是否发生，没有发生就会阻塞
    int read_len = read(fd, buf, sizeof(buf));

    // 如果read的返回值，小于inotify_event大小出现错误
    if (read_len < event_size) {
        printf("could not get event!\n");
        return -1;
    }

    // 因为read的返回值存在一个或者多个inotify_event对象，需要一个一个取出来处理
    int pos = 0;
    while (read_len >= event_size) {
        event = (struct inotify_event *)(buf + pos);
        if (event->len) {
            if (event->mask & IN_CREATE) {
                printf("create file: %s\n", event->name);
            } else {
                printf("delete file: %s\n", event->name);
            }
        }

        // 一个事件的真正大小：inotify_event 结构体所占用的空间 +
        // inotify_event->name 所占用的空间
        int temp_size = event_size + event->len;
        read_len -= temp_size;
        pos += temp_size;
    }
    return 0;
}

int wait(std::string path) {

    // inotify初始化
    int InotifyFd = inotify_init();
    if (InotifyFd == -1) {
        printf("inotify_init error!\n");
        return -1;
    }

    // 添加watch对象
    std::string filePath = path;
    // 监控目标文件被删除、修改、元数据被修改
    int wd = inotify_add_watch(InotifyFd, filePath.c_str(),
                               IN_DELETE | IN_MODIFY | IN_ATTRIB);
    if (wd < 0) {
        printf("inotify_add_watch failed: %s\n", filePath.c_str());
        return -1;
    }

    // 处理事件
    int ret = process_inotify_events(InotifyFd);
    if (ret != 0) {
        return -1;
    }

    // 删除inotify的watch对象
    if (inotify_rm_watch(InotifyFd, wd) == -1) {
        printf("notify_rm_watch error!\n");
        return -1;
    }

    // 关闭inotify描述符
    close(InotifyFd);
    return 0;
}
