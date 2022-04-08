#pragma once

#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/listener.h>
#include <event2/util.h>
#include <event2/event.h>
#include <unordered_map>
#include <string>
#include "emisc/misctool.h"
#include <string.h>
#include <mutex>
#include <memory>
#include "./emisc/FunDelegate.h"
#include "eDataLayer.h"
#include "ePackage.h"

enum eErrServer{
        NO_ERROR = 0,
        BASE_INIT_FAIL = 1,
        LISTEN_ERR = 2,
        NO_FIND_CLIENT = 3,
        SEND_DATA_FAIL = 4,
    };

class eTcpSrvLayer;

struct client_ctx{
    eTcpSrvLayer* inst;
    //tcpClient* client;
    unsigned int key;

    client_ctx(){
        key = 0;
    }
};

struct tcpClient{
    evutil_socket_t fd;
    struct bufferevent *bev;
    struct event *write_ev;
    struct event *notify_close_ev;
    client_ctx *ctx;
    unsigned int Key;
    std::shared_ptr<eSocketShareData> sp_Package;
    void* args;

    tcpClient(evutil_socket_t fd, struct bufferevent *bev, struct event *write_ev,
             struct event *notify_close_ev, client_ctx* ctx, const unsigned int key){
        this->fd = fd;
        this->Key = key;
        this->bev = bev;
        this->write_ev = write_ev;
        this->notify_close_ev = notify_close_ev;
        this->ctx = ctx;
        sp_Package = std::make_shared<eSocketShareData>();
        args = nullptr; //回调时传递的上下文现在没有用到
    }

    ~tcpClient(){
        if(this->bev)
            bufferevent_free(this->bev);
        if(this->write_ev)
            event_free(this->write_ev);
        if(this->notify_close_ev)
            event_free(this->notify_close_ev);
        if(this->ctx)
            delete this->ctx;
    }
};

typedef std::unordered_map<unsigned int, std::unique_ptr<tcpClient>> ClientsUnorderMap;

class eTcpSrvLayer : public eDataLayer<eSocketShareData>::I_Generate_Data_Base {
public:
    
    eTcpSrvLayer();
    virtual ~eTcpSrvLayer();

    virtual void runGenerateData(const eDataLayer<eSocketShareData> *ctx) override;
    eErrServer Run(const int port);

    eErrServer SendData2Client(const unsigned int key, const unsigned char* msg, const unsigned int msg_len, void* user_args = nullptr);
    eErrServer CloseClient(const unsigned int key, void* user_args = nullptr);

    void RegisterSendStatus(TClsMemFnDelegate_3Param<void, unsigned int, std::string, void*> cb);
    void RegisterClientClose(TClsMemFnDelegate_3Param<void, unsigned int, std::string, void*> cb);

protected:
    static void listener_cb(struct evconnlistener *listener, evutil_socket_t fd,
            struct sockaddr *sa, int socklen, void *user_data);
    static void conn_readcb(struct bufferevent *bev, void *user_data);
    static void conn_writecb(struct bufferevent *bev, void *user_data);
    static void conn_eventcb(struct bufferevent *bev, short events, void *user_data);
    static void client_notify_sendmsg_cb(int fd, short events, void* arg);
    static void notify_close_client_cb(int fd, short events, void* arg);
    bool freeClient(const unsigned int key, const int code);

private:
    struct event_base *_base;
	struct evconnlistener *_listener;
    ClientsUnorderMap _clinetsCollect;
    std::mutex _clients_mutex;
    TClsMemFnDelegate_3Param<void, unsigned int, std::string, void*> _send_data_cb, _close_client_cb;

    eDataLayer<eSocketShareData> *_datalayer = nullptr;
};