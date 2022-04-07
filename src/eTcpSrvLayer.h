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
    struct bufferevent *bev;
    struct event *write_ev;
    struct event *notify_close_ev;
    client_ctx *ctx;
    unsigned int Key;
    std::shared_ptr<eSocketShareData> sp_Package;

    tcpClient(struct bufferevent *bev, struct event *write_ev,
             struct event *notify_close_ev, client_ctx* ctx, const unsigned int key){
        this->Key = key;
        this->bev = bev;
        this->write_ev = write_ev;
        this->notify_close_ev = notify_close_ev;
        this->ctx = ctx;
        sp_Package = std::make_shared<eSocketShareData>();
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

    eErrServer SendData2Client(const unsigned int key, const unsigned char* msg, const unsigned int msg_len, TClsMemFnDelegate_0Param<void> cb);
    eErrServer CloseClient(const unsigned int key, TClsMemFnDelegate_0Param<void> cb);

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

    eDataLayer<eSocketShareData> *_datalayer = nullptr;
};