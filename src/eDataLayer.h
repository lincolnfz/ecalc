#pragma once
#include "emisc/misctool.h"
#include <queue>
#include <memory>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <chrono>

template<class META_Message>
class eDataLayer{

public:
    class I_Process_Data_Base{
    public:
        I_Process_Data_Base(){
        };

        virtual ~I_Process_Data_Base(){
        };

        //hadleNotifyMsg 工作在eDataLayer线程
        virtual void hadleNotifyMsg(std::unique_ptr<META_Message>) = 0;
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
    //消息队列类
    class Priority_Message_Queue{
    private:
        struct Entry{
            std::unique_ptr<META_Message> pMsg;
            int priority;
            ~Entry(){
            };
        };
    
        struct Compare_Messages {
            bool operator () (const Entry *left , const Entry * right) {
                return (left->priority < right->priority);
            }
        };  
    
    typedef std::priority_queue<Entry*, std::vector<Entry*>, Compare_Messages>
                Message_Queue_Type;
    
    Message_Queue_Type _message_Queue;
    std::mutex _Queue_lock;
        
    public:
        Priority_Message_Queue() = default;
        ~Priority_Message_Queue() = default;
        void PushMsg(std::unique_ptr<META_Message> pMsg, int priority) {
                std::unique_lock<std::mutex> guard(_Queue_lock);
                // Make an entry
                Entry *entry = new Entry;
                entry->pMsg = std::move(pMsg);
                entry->priority = priority;
                // Insert the element according to its priority
                _message_Queue.push(entry);
        }
        
        std::unique_ptr<META_Message> PullMsg() {
                std::unique_lock<std::mutex> guard(_Queue_lock);
                std::unique_ptr<META_Message> pMsg;
                
                // Check if the message queue is not empty
                if (!_message_Queue.empty()) {
                    // Queue is not empty so get a pointer to the
                    // first message in the queue
                    Entry* top_item = _message_Queue.top();
                    pMsg = std::move(top_item->pMsg);
                    
                    // Now remove the pointer from the message queue
                    _message_Queue.pop();
                    delete top_item;
                }
                return pMsg;
        }
        
        const size_t Get_Length() {
                std::unique_lock<std::mutex> guard(_Queue_lock);
                return _message_Queue.size();
        } 
    };

public:
    eDataLayer(){

    };

    virtual ~eDataLayer(){

    };

    void Init(const I_Generate_Data_Base *in, const I_Process_Data_Base *out){
        _in = in;
        _out = out;
    }

    void RunPump(){
        _ASSERT(_in);
        _ASSERT(_out);
        std::thread gen_data_thread(_in->runGenerateData, this);

        while(true){
            std::unique_lock<std::mutex> lock(_data_lock);
            while(_queue.Get_Length() > 0){
                std::unique_ptr<META_Message> msg = std::move(_queue.PullMsg());
                if(msg){
                    
                }
                //std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
            while(_queue.Get_Length() == 0){
                _thread_con.wait(lock);
            }
        }
        gen_data_thread.join();
    }

private:
    Priority_Message_Queue _queue;

    //数据流向
    //  _in --->--->-->---> _out

    //接收处理数据对像 如:(业务模块)
    I_Process_Data_Base *_out;

    //产生数据 如:(socket)
    I_Generate_Data_Base *_in;

    std::mutex _data_lock;
    std::condition_variable _thread_con;
};