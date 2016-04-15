#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <fcntl.h>

#include <map>

#define MAX_EVENTS (1 << 16)

void setnonblocking(int fd) {
  int flags = fcntl(fd, F_GETFL, 0);
  fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

enum State {
  kStateConnecting = 0,
  kStateWriting
};

struct SocketState {
  uint32_t addr;
  State state;

  SocketState() : addr(0), state(kStateConnecting) {}
};

#define CONN_TIMEOUT 5000

const char* ntoa(in_addr_t addr) {
  struct in_addr saddr;
  saddr.s_addr = htonl(addr);
  return inet_ntoa(saddr);
}

int get_sock_error(int sock) {
  int error = 0;
  socklen_t errlen = sizeof(error);
  getsockopt(sock, SOL_SOCKET, SO_ERROR, (void *)&error, &errlen);
  return error;
}

int main(int argc, const char** argv) {
  if (argc < 3) {
    fprintf(stderr, "Usage: scan <ip start> <ip finish>\n");
    exit(1);
  }
  uint32_t addr_start = ntohl(inet_addr(argv[1]));
  uint32_t addr_finish = ntohl(inet_addr(argv[2]));

  struct sockaddr_in addr_in;
  addr_in.sin_family = AF_INET;
  addr_in.sin_port = htons(80);
  int epollfd = epoll_create(1);
  struct epoll_event ev;
  struct epoll_event events[MAX_EVENTS];

  std::map<int, SocketState> socks;

  const char* HTTP_GET = "GET / HTTP/1.1\r\nHost: %s\r\n"
                         "User-Agent: Mozilla/4.0 (compatible; MSIE 7.0; Windows NT 5.1)\r\n\r\n";

  uint32_t addr = addr_start;
  while (addr < addr_finish || socks.size() != 0) {
    int i = 0;
    while (addr < addr_finish && socks.size() < MAX_EVENTS) {
      // register new socket
	    int sock = socket(AF_INET, SOCK_STREAM, 0);
      setnonblocking(sock);
	    addr_in.sin_addr.s_addr = htonl(addr);
	    if (connect(sock, (struct sockaddr *)(&addr_in), sizeof(addr_in))) {
       if (errno != EINPROGRESS) {
        printf("%d\n", i++);
	      perror("connect");
	      ++addr;
        continue;
       }
	    }
	    ev.events = EPOLLOUT | EPOLLET | EPOLLRDHUP;
	    ev.data.fd = sock;
	    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, sock, &ev)) {
		    perror("epoll_ctl: sock");
		    exit(EXIT_FAILURE);
	    }
	    socks[sock].addr = addr;
      ++addr;
    }

    int events_num = epoll_wait(epollfd, events, MAX_EVENTS, CONN_TIMEOUT);
    if (events_num == 0) {
      for (auto sock : socks) {
        struct in_addr addr;
        addr.s_addr = htonl(sock.second.addr);
        printf("IP %s: connection timed out\n", inet_ntoa(addr));
        close(sock.first);
      }
      socks.clear();
    }
    for (int i = 0; i < events_num; i++) {
      int sock = events[i].data.fd;
      SocketState& state = socks[sock];
      if (events[i].events & EPOLLERR) {
        const char *sock_error = "unknown error";
        int error = get_sock_error(sock);
        if (error) {
          sock_error = strerror(error);
        }
        // error
        printf("IP %s: %s\n", ntoa(state.addr), sock_error);
        socks.erase(sock);
        close(sock);
        continue;
      }
      switch (state.state) {
		    case kStateConnecting: {
          // ready to write
          dprintf(sock, HTTP_GET, ntoa(state.addr));
          // change epoll type to EPOLLIN
          ev.data.fd = sock;
          ev.events = EPOLLIN | EPOLLET;
          if (epoll_ctl(epollfd, EPOLL_CTL_MOD, sock, &ev)) {
		        perror("epoll_ctl: modify sock");
		        exit(EXIT_FAILURE);
		      }
          state.state = kStateWriting;
		      break;
        }
        case kStateWriting: {
          // ready to read server's response
          char buf[1024] = {0};
          int rsize;
          const char *mask = isatty(STDOUT_FILENO)? "IP \033[1;32m%s\033[0m:\n" :
                                                    "IP %s:\n";
          printf("\n");
          printf(mask, ntoa(state.addr));
          bool finished = true;
          while ((rsize = read(sock, buf, sizeof(buf)))) {
            if (rsize < 0) {
              int error = get_sock_error(sock);
              if (error && error != EAGAIN) {
                perror("read");
              } else {
                finished = false;
              }
              break;
            }
            write(STDOUT_FILENO, buf, rsize);
            // Ideally, we should use Content-Length
            if (!memcmp(buf + rsize - 7, "</html>", 7)) {
              break;
            }
          }
          if (finished) {
            // unregister from epoll
            if (epoll_ctl(epollfd, EPOLL_CTL_DEL, sock, NULL)) {
  		        perror("epoll_ctl: modify sock");
  		        exit(EXIT_FAILURE);
  		      }
            socks.erase(sock);
            close(sock);
          }
          break;
        }
        default:
          break;
      } // switch
    } // epoll events for
  } // while
  close(epollfd);
  return 0;
}
