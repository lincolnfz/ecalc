#pragma once
#include "ePriority_Message_Queue.h"
#include "../emisc/misctool.h"

template<typename META_Message>
class eWorkThread{
public:
    enum ERR_DATA_LAYER{
        ERR_ZERO = 0,
        ERR_DIFFERENT_TID = 1,
    };

    enum PRIORITY_LEVEL{
        PRIORITY_LEVEL_IDLE = 0,
        PRIORITY_LEVEL_LOW = 3,
        PRIORITY_LEVEL_MID = 5,
        PRIORITY_LEVEL_HIGH = 7,
        PRIORITY_LEVEL_REAL = 9,
    };

    class I_Notify_CallBadk{
        friend eWorkThread;
        protected:
            virtual void handleNotifyMsg(std::shared_ptr<META_Message>) = 0;
    };

public:
    eWorkThread(){

    };
    ~eWorkThread(){

    };

    ERR_DATA_LAYER WriteInQueue_Msg(std::shared_ptr<META_Message> newData, const PRIORITY_LEVEL priority = PRIORITY_LEVEL::PRIORITY_LEVEL_MID){
        ERR_DATA_LAYER  err = ERR_ZERO;
        std::thread::id curr_tid = std::this_thread::get_id();
        if(curr_tid != _self_tid){
            _queue.PushMsg(newData, priority);
            std::unique_lock<std::mutex> lock(_data_lock);
            _thread_con.notify_one();
            lock.unlock();
        }else if(curr_tid == _self_tid){
            //err = ERR_DIFFERENT_TID;
            _ASSERT(false);
            //direct writein queue????
        }
        return err;
    }

    void RequestStop(){
        _ASSERT(std::this_thread::get_id() != _self_tid);
        _done = true;
    }

protected:
    void RunMsgPump(){
        _ASSERT(_gule);
        _self_tid = std::this_thread::get_id();
        while(!_done){
            std::unique_lock<std::mutex> lock(_data_lock);
            while(_queue.Get_Length() > 0){
                std::shared_ptr<META_Message> msg = _queue.PullMsg();
                if(msg){
                    //收到数据提交处理
                    if(_gule){
                        _gule->handleNotifyMsg(msg);
                    }
                }
                //std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
            //while(_queue.Get_Length() == 0){
                //有可能收到假事件,所以再加上列队长度判断
                //_thread_con.wait(lock);
                _thread_con.wait_for(lock, std::chrono::seconds(1));
            //}
        }
    };

    //virtual void handleNotifyMsg(std::shared_ptr<META_Message>) = 0;

protected:
    void Attach(I_Notify_CallBadk* cb){
        _gule = cb;
    };

private:
    I_Notify_CallBadk *_gule = nullptr;
    ePriority_Message_Queue<META_Message> _queue;
    std::mutex _data_lock;
    std::condition_variable _thread_con;
    std::thread::id _self_tid; //自已所在的线程id
    bool _done = false;
};