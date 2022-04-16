//https://www.jianshu.com/p/a60e9d53b0cb

#include "eTcpSrvLayer.h"
#include "../emisc/misctool.h"
#include <event2/bufferevent.h>
#include <event2/event.h>
#include <memory>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <utility>

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



//#ifdef _WIN32
#include <event2/thread.h>
//#endif /* _WIN32 */

#ifdef EVENT__HAVE_NETINET_IN_H
#include <netinet/in.h>
# ifdef _XOPEN_SOURCE_EXTENDED
#  include <arpa/inet.h>
# endif
#endif

#include <boost/functional/hash.hpp>
#include <boost/functional/hash.hpp>
#include <json.hpp>
#include <spdlog/spdlog.h>

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


using Json = nlohmann::json;

eTcpSrvLayer::eTcpSrvLayer(){

}

eTcpSrvLayer::~eTcpSrvLayer(){

}

struct event_base_ctx{
    eTcpSrvLayer* inst = nullptr;
    unsigned int key = 0;
    struct event *notify_write_ev = 0;
    struct event *notify_close_ev = 0;
    void *user_arg = nullptr;
};

void eTcpSrvLayer::runGenerateData(const eDataLayer<eSocketShareData> *ctx){
    _datalayer = const_cast<eDataLayer<eSocketShareData> *>(ctx);
    Run(9696);
}

eErrServer eTcpSrvLayer::Run(const int port){
    struct event_config *cfg = nullptr;
    evthread_use_pthreads();
    //event_enable_debug_mode();
    cfg = event_config_new();

    
    _base = event_base_new();
    int err = evthread_make_base_notifiable(_base);

    //留着以后配置
    //base_ = event_base_new_with_config(cfg);
    event_config_free(cfg);
	if (!_base || err) {
		fprintf(stderr, "Could not initialize libevent!\n");
		return BASE_INIT_FAIL;
	}
    struct sockaddr_in sin = {0};
#ifdef _WIN32
	WSADATA wsa_data;
	WSAStartup(0x0201, &wsa_data);
#endif
    sin.sin_addr.s_addr = htonl(INADDR_ANY);
    sin.sin_family = AF_INET;
	sin.sin_port = htons(port);

    event_base_ctx* ctx = new event_base_ctx;
    ctx->inst = this;
    _listener = evconnlistener_new_bind(_base, listener_cb, (void *)ctx,
	    LEV_OPT_REUSEABLE|LEV_OPT_CLOSE_ON_FREE|LEV_OPT_REUSEABLE_PORT, -1,
	    (struct sockaddr*)&sin,
	    sizeof(sin));

	if (!_listener) {
		fprintf(stderr, "Could not create a listener!\n");
		return LISTEN_ERR;
	}

    event_base_dispatch(_base);

	evconnlistener_free(_listener);
	//event_free(signal_event);
	event_base_free(_base);
	printf("done\n");

    return NO_ERROR;
}

eErrServer eTcpSrvLayer::SendData2Client(const unsigned int key, const unsigned char* msg, const unsigned int msg_len, void* user_args){
    eErrServer errCode = NO_FIND_CLIENT;
    spdlog::info("SendData2Client begin");
    bool bfind = false;
    _clients_mutex.lock();
    ClientsUnorderMap::const_iterator got = _clinetsCollect.find(key);
    if( got != _clinetsCollect.end() ){
        errCode = NO_ERROR;
        got->second->args = user_args;
        got->second->sp_Package->Write_SendBuf( const_cast<unsigned char*>(msg), msg_len);
        bfind = true;
    }
    _clients_mutex.unlock();

    //int event = 0;
    //通知向客户端发送数据
    if(bfind){
        spdlog::info("SendData2Client");
        //event_active(got->second->write_ev, event, 0);
        event_base_ctx* ctx = new event_base_ctx();
        ctx->inst = this;
        ctx->key = key;
        ctx->user_arg = user_args;
        ctx->notify_write_ev = evtimer_new(_base,  client_notify_sendmsg_cb, (void*)ctx);
        //ctx->notify_write_ev = event_new(got->second->ctx->inst->_base, -1, EV_TIMEOUT, client_notify_sendmsg_cb, (void*)ctx);
        //got->second->write_ev = ev_write;
        //event_base_set(got->second->ctx->inst->_base, ctx->notify_write_ev);
        timeval tv;
        tv.tv_sec = 0;
        tv.tv_usec = 0;
        if(evtimer_add( ctx->notify_write_ev, &tv) == 0){
            //spdlog::info("timer event_add ok");
        }else{
            spdlog::info("timer event_add fail");
        }
    }else{
        spdlog::info("loss client, don't send!!");
    }
    
    return errCode;
}

eErrServer eTcpSrvLayer::CloseClient(const unsigned int key, void* user_args){
    eErrServer errCode = NO_FIND_CLIENT;
    _clients_mutex.lock();
    ClientsUnorderMap::const_iterator got = _clinetsCollect.find(key);
    if( got != _clinetsCollect.end() ){
        errCode = NO_ERROR;
        //通知关闭客户端
        int event = 0;
        got->second->args = user_args;
    }
    _clients_mutex.unlock();

    event_base_ctx* ctx = new event_base_ctx();
    ctx->inst = this;
    ctx->key = key;
    ctx->user_arg = user_args;
    ctx->notify_close_ev = evtimer_new(_base,  notify_close_client_cb, (void*)ctx);
    //ctx->notify_close_ev = event_new(got->second->ctx->inst->_base, -1, EV_TIMEOUT, notify_close_client_cb, (void*)ctx);
    //got->second->write_ev = ev_write;
    //event_base_set(got->second->ctx->inst->_base, ctx->notify_close_ev);
    timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    if(evtimer_add( ctx->notify_close_ev, &tv) == 0){
        //spdlog::info("timer event_add ok");
    }else{
        spdlog::info("timer event_add fail");
    }
    return errCode;
}

void eTcpSrvLayer::RegisterSendStatus(TClsMemFnDelegate_3Param<void, unsigned int, std::string, void*> cb){
    _send_data_cb = cb;
}

void eTcpSrvLayer::RegisterClientClose(TClsMemFnDelegate_3Param<void, unsigned int, std::string, void*> cb){
    _close_client_cb = cb;
}

void eTcpSrvLayer::listener_cb(struct evconnlistener *listener, evutil_socket_t fd,
            struct sockaddr *sa, int socklen, void *user_data){
    
    #ifdef DEBUG
    int i = 0;
    #endif
    //收到连接请求
    event_base_ctx *listen_ctx = (event_base_ctx*)user_data;
    eTcpSrvLayer *self = listen_ctx->inst;
    struct event_base *base = self->_base;
    struct bufferevent *bev;
    spdlog::info("accept new client listener_cb!");
    boost::hash<std::string> string_hash;
    unsigned int key = string_hash(CreateGuid());
    client_ctx* ctx = new client_ctx;
    if(!ctx){
        return;
    }
    ctx->key = key;
    ctx->inst = self;

    //创建并且初始化buffer缓冲区
    bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);
    if(!bev){
        delete ctx;
        fprintf(stderr, "Error constructing bufferevent!\n");
        return;
    }
    
     /*struct event* write_ev = event_new(base, -1, EV_READ | EV_PERSIST, client_notify_sendmsg_cb, (void*)ctx);
     //struct event* write_ev = evsignal_new(base, -1,  client_notify_sendmsg_cb, (void*)ctx);
     if(!write_ev || event_add(write_ev, NULL)<0){
         delete ctx;
         bufferevent_free(bev); //清理缓冲区
         spdlog::info("Error constructing event_new!");
         fprintf(stderr, "Error constructing event_new!\n");
         return;
     }
     
     struct event* notify_close_ev = event_new(base, 3, EV_READ | EV_PERSIST, notify_close_client_cb, (void*)ctx);
     if(!notify_close_ev || event_add(notify_close_ev, NULL)<0){
         delete ctx;
         bufferevent_free(bev); //清理缓冲区
         event_free(write_ev); //清理事件
         spdlog::info("Error constructing notify_close_ev!");
         fprintf(stderr, "Error constructing notify_close_ev!\n");
         return;
     }*/

    
    std::unique_ptr<tcpClient> client(new tcpClient(fd, bev, ctx,  key));
    //std::pair<ClientsUnorderMap::iterator, bool> res = self->clinetsCollect_.insert(std::pair<std::string, tcpClient*>(key, client));
    self->_clients_mutex.lock();
    client->sp_Package->_session_key = key; //保存socket guid
    std::pair<ClientsUnorderMap::iterator, bool> res = 
            self->_clinetsCollect.emplace(key, std::move(client));
    if(!res.second){
        self->_clients_mutex.unlock();
        fprintf(stderr, "Error clinetsCollect_ insert fail !\n");
        return;
    }
    self->_clients_mutex.unlock();
    
    bufferevent_setcb(bev, conn_readcb, nullptr, conn_eventcb, (void*)ctx);
	bufferevent_enable(bev, EV_READ|EV_PERSIST);
    //bufferevent_enable(bev, EV_CLOSED);
}

void eTcpSrvLayer::conn_readcb(struct bufferevent *bev, void *user_data)
{
    client_ctx* ctx = (client_ctx*)user_data;
	/*char msg[1024];

    unsigned long len = bufferevent_read(bev , msg, sizeof(msg));
    msg[len] = '\0';

    printf("recv %s from server\n", msg);*/
    eTcpSrvLayer *self = ctx->inst;
    std::shared_ptr<eSocketShareData> sp;
    self->_clients_mutex.lock();
    ClientsUnorderMap::const_iterator got = self->_clinetsCollect.find(ctx->key);
    if( got != self->_clinetsCollect.end() ){
        while(true){
            unsigned char msg[8192];
            unsigned long len = bufferevent_read(bev , msg, sizeof(msg));
            if(len <= 0){
                break;
            }else{
                got->second->sp_Package->Write_RecvBuf(msg, len);
                sp = got->second->sp_Package;
            }
        }
    }
    self->_clients_mutex.unlock();
    if(sp){
        spdlog::info("recv network msg");
        self->_datalayer->WriteInQueue_Msg(sp, eDataLayer<eSocketShareData>::PRIORITY_LEVEL::PRIORITY_LEVEL_MID);
    }
}

void eTcpSrvLayer::conn_writecb(struct bufferevent *bev, void *user_data)
{
    //dummy
    client_ctx* ctx = (client_ctx*)user_data;
    //这个回调只是判断是否还有待发送的数据
	/*struct evbuffer *output = (struct evbuffer *)bufferevent_get_output(bev);
	if (evbuffer_get_length(output) == 0) {
		printf("flushed answer\n");
		bufferevent_free(bev);
	}*/
    eTcpSrvLayer *self = ctx->inst;
    self->_clients_mutex.lock();
    ClientsUnorderMap::const_iterator got = self->_clinetsCollect.find(ctx->key);
    if( got != self->_clinetsCollect.end() ){
        
    }
    self->_clients_mutex.unlock();
}

void eTcpSrvLayer::conn_eventcb(struct bufferevent *bev, short events, void *user_data)
{
    client_ctx* ctx = (client_ctx*)user_data;
	if (events & BEV_EVENT_EOF) {
		printf("Connection closed.\n");
	} else if (events & BEV_EVENT_ERROR) {
		printf("Got an error on the connection: %s\n", strerror(errno));/*XXX win32*/
	} else if (events & BEV_EVENT_TIMEOUT) {
        //
        printf("Timed out\n");
    }else if (events & BEV_EVENT_CONNECTED) {
        //
        printf("the client has connected to server\n");
        return;
    }else if (events & BEV_EVENT_READING) {
        printf("the client has BEV_EVENT_READING\n");
        return;
    }else if (events & BEV_EVENT_WRITING) {
        printf("the client has BEV_EVENT_WRITING\n");
        return;
    }
    
    ctx->inst->freeClient(ctx->key, events, nullptr);
    
	/* None of the other events can happen here, since we haven't enabled
	 * timeouts */
     //bufferevent_free 在结构体中清理
	//bufferevent_free(bev);
	
}

void eTcpSrvLayer::client_notify_sendmsg_cb(evutil_socket_t fd, short events, void* arg){
    event_base_ctx* ctx = (event_base_ctx*)arg;
    //得到bufferevent指针,目的是为了写到bufferevent的写缓冲区
    //struct bufferevent* bev =  ;

    //向外sock发送数据
    //bufferevent_write(bev, msg, ret);
    spdlog::info("client_notify_sendmsg_cb");
    event_free(ctx->notify_write_ev);
    int err = 0;
    eTcpSrvLayer *self = ctx->inst;
    std::shared_ptr<eSocketShareData> sp;
    self->_clients_mutex.lock();
    ClientsUnorderMap::const_iterator got = self->_clinetsCollect.find(ctx->key);
    if( got != self->_clinetsCollect.end() ){
        sp = got->second->sp_Package;
        struct bufferevent* bev = got->second->bev;
        while(true){
            unsigned char msg[8192];
            unsigned int len = got->second->sp_Package->Read_SendBuf(msg, sizeof(msg));
            //unsigned long len = bufferevent_read(bev , msg, sizeof(msg));
            if( bufferevent_write(bev, msg, len) < 0 ){
                //error here
                spdlog::info("SendData2Client fail!");
                err = SEND_DATA_FAIL;
                _ASSERT(false);
            }
            if(len <= 0){
                err = NO_ERROR;
                spdlog::info("SendData2Client ok!!!!!!!!!!");
                break;
            }
        }
    }
    self->_clients_mutex.unlock();
    if(sp){
        Json val;
        val['err'] = err;
        sp->PushCB(self->_send_data_cb, sp->_session_key, val.dump().c_str(), ctx->user_arg);
        self->_datalayer->WriteInQueue_Msg(sp, eDataLayer<eSocketShareData>::PRIORITY_LEVEL::PRIORITY_LEVEL_MID);
        spdlog::info("SendData2Client exit");
    }
    delete ctx;
}

void eTcpSrvLayer::notify_close_client_cb(int fd, short events, void* arg){
    event_base_ctx* ctx = (event_base_ctx*)arg;
    event_free(ctx->notify_close_ev);
    /*ctx->inst->_clients_mutex.lock();
    ClientsUnorderMap::const_iterator got = ctx->inst->_clinetsCollect.find(ctx->key);
    if( got != ctx->inst->_clinetsCollect.end() ){
        //关掉socket, 触发关闭事件,处理清理资源操作。
        //close(got->second->fd);
        if(got->second->bev){
            //bufferevent_setcb(got->second->bev, nullptr, nullptr, nullptr, nullptr);
            bufferevent_free(got->second->bev);
            got->second->bev = nullptr;
        }
    }
    ctx->inst->_clients_mutex.unlock();*/
    spdlog::info("notify_close_client_cb");
    //主动断开客户端
    ctx->inst->freeClient(ctx->key, 0, ctx->user_arg);
    delete ctx;
}

//重要!!!! 执行freeclient后,客户端资源就被销毁,不能再操作任何相关的业务
bool eTcpSrvLayer::freeClient(const unsigned int key, const int code, void *resv){
    bool ret = false;
    std::shared_ptr<eSocketShareData> sp;
    spdlog::info("free client begin");
    _clients_mutex.lock();
    Json val;
    ClientsUnorderMap::const_iterator got = _clinetsCollect.find(key);
    if( got != _clinetsCollect.end() ){
        if(got->second->bev){
            bufferevent_setcb(got->second->bev, nullptr, nullptr, nullptr, nullptr);
            //bufferevent_disable(got->second->bev, EV_READ|EV_WRITE);
            bufferevent_free(got->second->bev);
            got->second->bev = nullptr;
        }
        /*if(got->second->write_ev)
            event_free(got->second->write_ev);
        if(got->second->notify_close_ev)
            event_free(got->second->notify_close_ev);
        got->second->bev = nullptr;
        got->second->write_ev = nullptr;
        got->second->notify_close_ev = nullptr;*/
        sp = got->second->sp_Package;
        ret = true;
        _clinetsCollect.erase(got);
    }/*else{
        val["err"] = NO_FIND_CLIENT;
        got->second->sp_Package->PushCB(_close_client_cb, got->second->sp_Package->_session_key, val.dump().c_str(), 0);
        _datalayer->WriteInQueue_Msg(got->second->sp_Package, eDataLayer<eSocketShareData>::PRIORITY_LEVEL::PRIORITY_LEVEL_MID);
        fprintf(stderr, "Error freeClient no found fail !\n");
    }*/
    _clients_mutex.unlock();
    spdlog::info("free client ok");
    if(sp){
        //回调通知当前客户sock已关闭
        val["err"] = NO_ERROR;
        sp->PushCB(_close_client_cb, sp->_session_key, val.dump().c_str(), resv);
        _datalayer->WriteInQueue_Msg(sp, eDataLayer<eSocketShareData>::PRIORITY_LEVEL::PRIORITY_LEVEL_MID);
        spdlog::info("free client noitfy");
    }else{
        spdlog::info("free client can't noitfy!!");
    }
    return ret;
}