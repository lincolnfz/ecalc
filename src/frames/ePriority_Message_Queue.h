#pragma once
#include <queue>
#include <memory>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <chrono>

//消息队列类
template<class META_Message>
class ePriority_Message_Queue{
private:
    struct Entry{
        std::shared_ptr<META_Message> pMsg;
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
    ePriority_Message_Queue() = default;
    ~ePriority_Message_Queue() = default;
    void PushMsg(std::shared_ptr<META_Message> pMsg, int priority) {
        std::unique_lock<std::mutex> guard(_Queue_lock);
        // Make an entry
        Entry *entry = new Entry;
        entry->pMsg = pMsg;
        entry->priority = priority;
        // Insert the element according to its priority
        _message_Queue.push(entry);
    }
    
    std::shared_ptr<META_Message> PullMsg() {
        std::unique_lock<std::mutex> guard(_Queue_lock);
        std::shared_ptr<META_Message> pMsg;
        
        // Check if the message queue is not empty
        if (!_message_Queue.empty()) {
            // Queue is not empty so get a pointer to the
            // first message in the queue
            Entry* top_item = _message_Queue.top();
            pMsg = top_item->pMsg;
            
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
}; //ePriority_Message_Queue over.