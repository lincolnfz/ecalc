#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/syscall.h>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <getopt.h>
#include <io.h>
#include <fcntl.h>
#ifndef S_ISDIR
#define S_ISDIR(x) (((x) & S_IFMT) == S_IFDIR)
#endif
#else /* !_WIN32 */
#include <sys/stat.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#endif /* _WIN32 */
#include <signal.h>

#ifdef EVENT__HAVE_SYS_UN_H
#include <sys/un.h>
#endif
#ifdef EVENT__HAVE_AFUNIX_H
#include <afunix.h>
#endif

#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/listener.h>
#include <event2/util.h>
#include <event2/event.h>

#ifdef _WIN32
#include <event2/thread.h>
#endif /* _WIN32 */

#ifdef EVENT__HAVE_NETINET_IN_H
#include <netinet/in.h>
# ifdef _XOPEN_SOURCE_EXTENDED
#  include <arpa/inet.h>
# endif
#endif

#include <pthread.h>

#ifdef _WIN32
#ifndef stat
#define stat _stat
#endif
#ifndef fstat
#define fstat _fstat
#endif
#ifndef open
#define open _open
#endif
#ifndef close
#define close _close
#endif
#ifndef O_RDONLY
#define O_RDONLY _O_RDONLY
#endif
#endif /* _WIN32 */
#include <string>

#include "./src/emisc/misctool.h"
//test
#include <thread>
#include "src/Data.h"
#include "./src/3rd/uuid/uuid.h"
#include "./src/emisc/FunDelegate.h"
#include "./src/emisc/event.h"
#include "test.h"
//test

static const char MESSAGE[] = "Hello, World!\n";

static const unsigned short PORT = 9996;

static void listener_cb(struct evconnlistener *, evutil_socket_t,
    struct sockaddr *, int socklen, void *);
static void conn_writecb(struct bufferevent *, void *);
static void conn_eventcb(struct bufferevent *, short, void *);
static void signal_cb(evutil_socket_t, short, void *);

std::string CreateGuid_test()
{
    uuid_t uu;
    char str[37] = {0};
    uuid_generate( uu );
    uuid_unparse(uu, str);
    return std::string(str);
}

void tedo(TClsMemFnDelegate_0Param<void> pp){
	pp();
}

void te222do(TClsMemFnDelegate_2Param<int, int, std::string> pp){
	int a = pp(22, "45ade");
}

void noparm(TRawFnDelegate_2Param<void, int, int> fn){
	fn(16, 33);
}

void labdatest(std::function<bool(int, int)>  fn){
	bool x = fn(68, 77);
	int i = 0;
}

void aaa(int x, int y){

}

void tt(const char* file, const char* line){

}

int main(int argc, char** argv){
	{
		//test code
		//test();
		//test_mutex();
		//thread_product_test();
		test_ring_buf();
		test_unique_ptr();
		printf("done\n");
		test_Priority_Message_Queue();
    	//return 0;
		eEvent* eve = eEvent::CreateEvent(777, false, false, nullptr);
		long to = eEvent::WaitForSingleObject(eve, 1000);
		_ASSERT(true);
		int ptid = getpid();
		int tid = gettid();
		int t = syscall(SYS_gettid);
		int l = sizeof(long long);
		std::string s = CreateGuid_test();
		l = s.size();
		CreateGuid_test();
		Data tt;
		std::function<void()> funcptr;
		bool hhhh = funcptr == nullptr;
		TClsMemFnDelegate_0Param<void> a;
		hhhh = a.isbind();
		a.unbind();
		a.unbind();
		a.BindRaw(&tt, &Data::Recv);
		hhhh = a.isbind();
		tedo(a);

		TClsMemFnDelegate_2Param<int, int, std::string> t2pam;
		t2pam.BindRaw(&tt, &Data::test2parm);
		te222do(t2pam);
		//noparm([](int x, int y) -> void{
		//	int i = 0;
		//});
		TRawFnDelegate_2Param<void, int, int> fnn;
		fnn.BindRaw(aaa);
		noparm(fnn); //ok

		labdatest( [](int x, int y) -> bool{
			return true;
		} );
		return 0;
	}


    struct event_base *base;
	struct evconnlistener *listener;
	struct event *signal_event;
	struct sockaddr_in sin = {0};
#ifdef _WIN32
	WSADATA wsa_data;
	WSAStartup(0x0201, &wsa_data);
#endif
	
	base = event_base_new();
	if (!base) {
		fprintf(stderr, "Could not initialize libevent!\n");
		return 1;
	}
    sin.sin_family = AF_INET;
	sin.sin_port = htons(PORT);

	listener = evconnlistener_new_bind(base, listener_cb, (void *)base,
	    LEV_OPT_REUSEABLE|LEV_OPT_CLOSE_ON_FREE, -1,
	    (struct sockaddr*)&sin,
	    sizeof(sin));

	if (!listener) {
		fprintf(stderr, "Could not create a listener!\n");
		return 1;
	}

	signal_event = evsignal_new(base, SIGINT, signal_cb, (void *)base);

	if (!signal_event || event_add(signal_event, nullptr)<0) {
		fprintf(stderr, "Could not create/add a signal event!\n");
		return 1;
	}

	event_base_dispatch(base);

	evconnlistener_free(listener);
	event_free(signal_event);
	event_base_free(base);
	printf("done\n");

    return 0;
}

static void
listener_cb(struct evconnlistener *listener, evutil_socket_t fd,
    struct sockaddr *sa, int socklen, void *user_data)
{
	struct event_base *base = (struct event_base*)user_data;
	struct bufferevent *bev;

	//创建并且初始化buffer缓冲区
	bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);
	if (!bev) {
		fprintf(stderr, "Error constructing bufferevent!");
		event_base_loopbreak(base);
		return;
	}
	bufferevent_setcb(bev, nullptr, conn_writecb, conn_eventcb, nullptr);
	bufferevent_enable(bev, EV_WRITE);
	bufferevent_disable(bev, EV_READ);

	bufferevent_write(bev, MESSAGE, strlen(MESSAGE));
}

static void
conn_writecb(struct bufferevent *bev, void *user_data)
{
	struct evbuffer *output = (struct evbuffer *)bufferevent_get_output(bev);
	if (evbuffer_get_length(output) == 0) {
		printf("flushed answer\n");
		bufferevent_free(bev);
	}
}

static void
conn_eventcb(struct bufferevent *bev, short events, void *user_data)
{
	if (events & BEV_EVENT_EOF) {
		printf("Connection closed.\n");
	} else if (events & BEV_EVENT_ERROR) {
		printf("Got an error on the connection: %s\n",
		    strerror(errno));/*XXX win32*/
	}
	/* None of the other events can happen here, since we haven't enabled
	 * timeouts */
	bufferevent_free(bev);
	
}

static void
signal_cb(evutil_socket_t sig, short events, void *user_data)
{
	struct event_base *base = (struct event_base *)user_data;
	struct timeval delay = { 2, 0 };

	printf("Caught an interrupt signal; exiting cleanly in two seconds.\n");

	event_base_loopexit(base, &delay);
}