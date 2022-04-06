
#include "test.h"
#include <iostream>
#include <mutex>
#include <memory>
#include <chrono>
#include <thread>
#include <unordered_map>
#include <condition_variable>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include "./src/Data.h"
#include <list>
#include "./src/eDataLayer.h"
#include "./src/eTcpSrvLayer.h"
#include "./src/ePackage.h"

std::mutex g_mutex;

void independentThread() {
    std::cout << "Starting concurrent thread.\n";
    std::this_thread::sleep_for(std::chrono::seconds(20));
    std::cout << "Exiting concurrent thread.\n";
 }

void threadCaller()  {
    std::cout << "Starting thread caller.\n";
    std::thread t(independentThread);
    t.detach();
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << "Exiting thread caller.\n";
}

void test(){
    threadCaller();
    std::this_thread::sleep_for(std::chrono::seconds(5));
}

void run(int a, int b){
    std::cout << "run id" <<  std::this_thread::get_id() << "\n";
    std::unique_lock<std::mutex> guard(g_mutex);
    std::cout << "run \n";
}

void test_mutex(){
    std::cout << "test_mutex id " <<  std::this_thread::get_id() << "\n";
    auto fPtr3 = std::make_unique<int>();
    std::unique_lock<std::mutex> guard(g_mutex, std::defer_lock);
    std::thread t1(run, 3, 6);
    t1.join();
    std::cout << "test_mutex run \n";
}

struct Entry{
        std::unique_ptr<Data> pMsg;
        int priority;
        ~Entry(){}
    };

void test_unique_ptr(){
    std::unique_ptr<Data> pMsg;
    std::unique_ptr<Data> ptr(new Data());
    Entry* pentry = new Entry();
    pentry->pMsg = std::move(ptr);
    std::unique_ptr<Data> ptr1 = std::move(pentry->pMsg);
    delete pentry;

    if(ptr){
        ptr->Recv();
    }
    ptr.reset();
    if(ptr){
        ptr->Recv();
    }

    std::unique_ptr<Data> dp(new Data);
    std::unique_ptr<Data> dp1(new Data);
    std::unordered_map<std::string, std::unique_ptr<Data>> tmpmap;
    tmpmap.emplace("adsafs", std::move(dp));
    std::pair<std::unordered_map<std::string, std::unique_ptr<Data>>::iterator, bool> res = tmpmap.emplace("adsafs", std::move(dp1));
    std::unordered_map<std::string, std::unique_ptr<Data>>::const_iterator got = tmpmap.find("adsafs");
    if(got != tmpmap.end()){
        got->second->Recv();
        tmpmap.erase(got);
    }
    int i = 0;
}

std::condition_variable g_con;
std::list<int> products;

void test_cc() {
    int product_id = 0;
    while (true) {
        products.push_back(++product_id);
        std::cout << "products 生产: " << product_id << std::endl;
        std::unique_lock<std::mutex> lock(g_mutex);
        // 通知消费者消费
        g_con.notify_one();
        lock.unlock();
        if (product_id > 10) {
            break;
        }
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }
}

int thread_product_test() {
    std::thread t1(test_cc);
    while (true) {
        std::unique_lock<std::mutex> lock(g_mutex);
        if (products.empty()) {
            std::cout << "没有产品，等待" << std::endl;
            // 进入等待，知道有新产品
            g_con.wait(lock);
        } else {
            int product_id = products.front();
            products.pop_front();
            std::cout << "消费产品 " << product_id << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(2));
            if (product_id > 10) break;
        }
    }
    t1.join();
    return 0;
}

eDataLayer<Data>::Priority_Message_Queue test_queue;

void create_data(std::condition_variable* q_con){
    long idd = syscall(SYS_gettid);
    int product_id = 0;
    int priority = 100;
    while (true) {
        std::shared_ptr<Data> gen_msg = std::make_shared<Data>(product_id);
        //std::cout << "生产数据" << product_id <<  std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        test_queue.PushMsg(gen_msg, ++priority);
        /*{
            std::unique_ptr<Data> gen_msg(new Data(2));
            test_queue.Add(std::move(gen_msg), 101);
            std::unique_ptr<Data> gen_msg22(new Data(4));
            test_queue.Add(std::move(gen_msg22), 101);
            std::unique_ptr<Data> gen_msg33(new Data(5));
            test_queue.Add(std::move(gen_msg33), 101);
            //std::unique_ptr<Data> msg = std::move(test_queue.PeekMsg());
            int i = 0;
        }*/
        std::unique_lock<std::mutex> lock(g_mutex);
        q_con->notify_one();
        lock.unlock();
        ++product_id;
        if(product_id > 20) break;
    }
}

struct shartptr{
    std::shared_ptr<Data> data;
    ~shartptr(){
        int i = 0;
    };
};

int test_Priority_Message_Queue(){
    std::shared_ptr<Data> tmp;
    {
        shartptr * p = new shartptr;
        p->data = std::make_shared<Data>(878);
        //tmp = p->data;
        delete p;
    }
    int xxxd = 9;
    //return 0;
    std::condition_variable q_con;
    std::thread t1(create_data, &q_con);
    std::thread::id tid = t1.get_id();
    std::thread::id rid = std::this_thread::get_id();
    bool tid_equal = tid == rid;
    //std::thread::native_handle_type currnet_nid = std::thread::native_handle();
    long idd = syscall(SYS_gettid);
    

    int proc_num = 0;
    while(true){
        std::unique_lock<std::mutex> lock(g_mutex);
        while(test_queue.Get_Length() > 0){
            std::shared_ptr<Data> msg = test_queue.PullMsg();
            if(msg){
                std::cout << "处理数据" << msg->i <<  std::endl;
                ++proc_num;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        while(test_queue.Get_Length() == 0){
            q_con.wait(lock);
        }
        if(proc_num >= 20) break;
    }
    t1.join();
    return 0;
}

int test_tcp_srv(){
    eTcpSrvLayer srv;
}

int test_ring_buf(){
    unsigned int len = 0;
    ring_buf_t ring_buf;
    unsigned char buf[600];
    unsigned char wbuf[700];

    {
        eSocketShareData test;
        test.Write_RecvBuf(buf, sizeof(buf));
        //test.Write_RecvBuf(buf, sizeof(buf));
        //test.Write_RecvBuf(buf, sizeof(buf));
        len = test.Read_RecvBuf(wbuf, sizeof(wbuf));
        len = test.Read_RecvBuf(wbuf, sizeof(wbuf));
    }

    len = sizeof(unsigned long);

    len = 0xffffffffffffffff + 5;

    len = ring_buf_create(&ring_buf, buf, sizeof(buf));

    len = ring_buf_put(&ring_buf, wbuf, sizeof(wbuf));

    len = ring_buf_put(&ring_buf, wbuf, sizeof(wbuf));

    len - ring_buf_get(&ring_buf, wbuf, sizeof(wbuf));

    len = ring_buf_put(&ring_buf, wbuf, sizeof(wbuf));

    len - ring_buf_get(&ring_buf, wbuf, sizeof(wbuf));

    len = ring_buf_put(&ring_buf, wbuf, sizeof(wbuf));

    int i = 0;
}