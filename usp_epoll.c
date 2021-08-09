/**
 * @Author: Nonrustknife <chris>
 * @Date:   06-Aug-2021
 * @Email:  cm656879@outlook.com
 * @Last modified by:   chris
 * @Last modified time: 09-Aug-2021
 */
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/event.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <stdio.h>
#include "usp_epoll.h"



int usp_epoll_create(int size __attribute__((unused)))
{
  int kq = kqueue();

  if(-1 == kq)
  {
    printf("kqueue failed:%d",errno);
    return -1;
  }

  return kq;
}

void update_kevent_from_epevent(struct kevent* kqevent, struct epoll_event* epevent)
{
    (kqevent)->ident = epevent->fd;
    (kqevent)->fflags = 0;
    (kqevent)->data = 0;
    (kqevent)->udata = NULL;
}

int usp_epoll_ctl(int epfd, int op, int listenfd, struct epoll_event* epevent)
{
  struct kevent kqevent;

  int ret = 0;
  switch (op) {
    case EPOLL_CTL_ADD :
    {
      update_kevent_from_epevent(&kqevent, epevent);
      if(epevent->events & EPOLLIN)
      {
        kqevent.filter = EVFILT_READ;
      }
      else if(epevent->events & EPOLLOUT)
      {
        kqevent.filter = EVFILT_WRITE;
      }
      kqevent.flags = EV_ADD|EV_ENABLE;
      ret = kevent(epfd, &kqevent, 1, NULL, 0, NULL);
      //printf("[%s]op:%d\n", __FUNCTION__, op);
      break;
    }
    case EPOLL_CTL_DEL :
    {
      update_kevent_from_epevent(&kqevent, epevent);
      if(epevent->events & EPOLLIN)
      {
        kqevent.filter = EVFILT_READ;
      }
      else if(epevent->events & EPOLLOUT)
      {
        kqevent.filter = EVFILT_WRITE;
      }
      kqevent.flags = EV_DELETE;
      ret = kevent(epfd, &kqevent, 1, NULL, 0, NULL);
      //printf("[%s]op:%d\n", __FUNCTION__, op);
      break;
    }
    case EPOLL_CTL_MOD :
    {
      update_kevent_from_epevent(&kqevent, epevent);
      if(epevent->events & EPOLLOUT)
      {
        kqevent.filter = EVFILT_READ;
      }
      else if(epevent->events & EPOLLIN)
      {
        kqevent.filter = EVFILT_WRITE;
      }
      kqevent.flags = EV_DELETE;
      ret = kevent(epfd, &kqevent, 1, NULL, 0, NULL);

      kqevent.filter = (-3 - kqevent.filter);
      kqevent.flags = EV_ADD|EV_ENABLE;
      ret = kevent(epfd, &kqevent, 1, NULL, 0, NULL);
      //printf("[%s]op:%d\n", __FUNCTION__, op);
      break;
    }

    default:
      printf("op err!\n");
  }

  if(ret != 0)
  {
    printf("errno:%d\n", errno);
  }
  return ret;
}

void update_epevent_from_kevent(struct epoll_event* epevent, struct kevent* kqevent)
{
  epevent->fd = kqevent->ident;
  if(kqevent->filter == EVFILT_READ)
    epevent->events = EPOLLIN;
  if(kqevent->filter == EVFILT_WRITE)
    epevent->events = EPOLLOUT;
}

int usp_epoll_wait(int epfd, struct epoll_event* epevent, int maxevents, int eptimeout)
{
  struct kevent eventlist[maxevents];
  struct timespec timeout = {eptimeout/1000, (eptimeout%1000)*1000000};
  int ret = kevent(epfd, NULL, 0, eventlist, maxevents, &timeout);

  for(int loop = 0; loop < ret; loop++)
  {
    update_epevent_from_kevent((epevent+loop), (eventlist+loop));
  }

  return ret;
}
