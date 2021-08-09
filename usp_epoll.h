/**
 * @Author: Nonrustknife <chris>
 * @Date:   06-Aug-2021
 * @Email:  cm656879@outlook.com
 * @Last modified by:   chris
 * @Last modified time: 07-Aug-2021
 */

 #include <sys/types.h>
 #include <sys/event.h>
 #include <sys/time.h>

#define MAX_EVENT_COUNT 64
#define OPEN_MAX 100

#define EPOLLIN       (1)
#define EPOLLOUT      (4)
#define EPOLLERR      (8)
#define EPOLL_CTL_ADD (1)
#define EPOLL_CTL_DEL (2)
#define EPOLL_CTL_MOD (3)

// #define EV_ADD        (1)
// #define EV_DELETE     (2)
// #define EV_ENABLE     (4)
// #define EV_DISABLE    (8)

struct epoll_event
{
  int fd;
  long events;
};

int usp_epoll_create(int size __attribute__((unused)));
void update_epevent_from_kevent(struct epoll_event* epevent, struct kevent* kqevent);
void update_kevent_from_epevent(struct kevent* kqevent, struct epoll_event* epevent);
int usp_epoll_ctl(int epfd, int op, int listenfd, struct epoll_event* epevent);
int usp_epoll_wait(int epfd, struct epoll_event* epevent, int maxevents, int eptimeout);
