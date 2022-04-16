#pragma once
#include "../emisc/misctool.h"
#include <queue>
#include <memory>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <chrono>
#include "ePriority_Message_Queue.h"

template<class META_Message>
class eDataLayer{
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

public:
    class I_Process_Data_Base{
    public:
        I_Process_Data_Base(){
        };

        virtual ~I_Process_Data_Base(){
        };

        //handleNotifyMsg 工作在eDataLayer线程
        virtual void handleNotifyMsg(std::shared_ptr<META_Message>) = 0;
        virtual void handleCheckTimer() = 0;
    };

    class I_Generate_Data_Base{
    public:
        I_Generate_Data_Base(){
        };

        virtual ~I_Generate_Data_Base(){
        };

        //runGenerateData 工作在另一个线程
        virtual void runGenerateData(const eDataLayer<META_Message> *ctx) = 0;

    };

#ifndef _DEBUG
private:
#else
public:
#endif

public:
    eDataLayer(){

    };

    virtual ~eDataLayer(){

    };

    void Init(I_Generate_Data_Base *in, I_Process_Data_Base *out){
        _in = in;
        _out = out;
    }

    void RunMsgPump(){
        _ASSERT(_in);
        _ASSERT(_out);
        eDataLayer<META_Message> *self = this;
        std::thread gen_data_thread(&I_Generate_Data_Base::runGenerateData, _in, self);
        _in_tid = gen_data_thread.get_id();
        _out_tid = std::this_thread::get_id();
        while(true){
            std::unique_lock<std::mutex> lock(_data_lock);
            _out->handleCheckTimer();
            while(_queue.Get_Length() > 0){
                std::shared_ptr<META_Message> msg = _queue.PullMsg();
                if(msg){
                    //收到数据提交处理
                    if(_out){
                        _out->handleNotifyMsg(msg);
                    }
                }
                //std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
            //while(_queue.Get_Length() == 0){
                //有可能收到假事件,所以再加上列队长度判断
                //auto now = std::chrono::system_clock::now();
                _thread_con.wait_for(lock, std::chrono::seconds(1));
            //}
        }
        //gen_data_thread.get_id();
        //gen_data_thread.join();
    }

    // in模块(外部数据)写进来的
    ERR_DATA_LAYER WriteInQueue_Msg(std::shared_ptr<META_Message> newData, const PRIORITY_LEVEL priority = PRIORITY_LEVEL::PRIORITY_LEVEL_MID){
        ERR_DATA_LAYER  err = ERR_ZERO;
        std::thread::id curr_tid = std::this_thread::get_id();
        if(curr_tid != _out_tid){
            _queue.PushMsg(newData, priority);
            std::unique_lock<std::mutex> lock(_data_lock);
            _thread_con.notify_one();
            lock.unlock();
        }else if(curr_tid == _out_tid){
            //err = ERR_DIFFERENT_TID;
            _ASSERT(false);
            //direct writein queue????
        }
        return err;
    }
    

private:
    //Priority_Message_Queue _queue;
    ePriority_Message_Queue<META_Message> _queue;

    //数据流向
    //  _in --->--->-->---> _out

    //接收处理数据对像 如:(业务模块)
    I_Process_Data_Base *_out = nullptr;
    std::thread::id _out_tid;

    //产生数据 如:(socket)
    I_Generate_Data_Base *_in = nullptr;
    std::thread::id _in_tid;

    std::mutex _data_lock;
    std::condition_variable _thread_con;
};