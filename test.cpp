
#include "test.h"
#include <cstdlib>
#include <iostream>
#include <mutex>
#include <memory>
#include <chrono>
#include <string>
#include <thread>
#include <unordered_map>
#include <condition_variable>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include "./src/Data.h"
#include <list>
#include "./src/frames/eDataLayer.h"
#include "./src/frames/eTcpSrvLayer.h"
#include "./src/frames/ePackage.h"
#include "./src/frames/ModuleBase.h"
#include "./src/frames/eTcpClientsManager.h"
#include "./src/frames/PingPongFn.h"
#include "./src/eEmuManager.h"
#include <boost/functional/hash.hpp>
#include "./src/eEmuClient.h"
#include "./src/DataPacket.h"
#include "./src/frames/eWebInterface.h"
#include "./src/frames/ePriority_Message_Queue.h"
#include "./src/emisc/elog.h"

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
        std::cout << "products ??????: " << product_id << std::endl;
        std::unique_lock<std::mutex> lock(g_mutex);
        // ?????????????????????
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
            std::cout << "?????????????????????" << std::endl;
            // ?????????????????????????????????
            g_con.wait(lock);
        } else {
            int product_id = products.front();
            products.pop_front();
            std::cout << "???????????? " << product_id << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(2));
            if (product_id > 10) break;
        }
    }
    t1.join();
    return 0;
}

ePriority_Message_Queue<Data> test_queue;

void create_data(std::condition_variable* q_con){
    long idd = syscall(SYS_gettid);
    int product_id = 0;
    int priority = 100;
    while (true) {
        std::shared_ptr<Data> gen_msg = std::make_shared<Data>(product_id);
        //std::cout << "????????????" << product_id <<  std::endl;
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
                std::cout << "????????????" << msg->i <<  std::endl;
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
    return i;
}

class newsermod: public ModuleBase<eSocketShareData>{
public:
    newsermod() = default;
    ~newsermod() = default;
    void handleNotifyMsg(std::shared_ptr<eSocketShareData> msg) override{
    }

    
};

int test_module(){
    newsermod a;
    const char* szname = a.className();
    szname = a.className();
    std::string nn = CLASS_NAME(newsermod);
    printf(szname);
    return 0;
}

class eEnumHuawei : public eTcpClientsManager{
public:
    eEnumHuawei(){

    };

    ~eEnumHuawei(){

    };

};

int getn(){
    static int i = 3;
    --i;
    return i;
}
int test_clientmanager(){
    int len = 0;
    while( len = getn(), len>0){
        int i = 0;
        i = 0;
    }
    eEmuManager a;
    //a.handleNotifyMsg(std::shared_ptr<eSocketShareData>(new eSocketShareData()));
    a.Start();
    return 0;
}

class fncb : public ResponseHandle<TClsMemFnDelegate_2Param<void, char*, unsigned int>> {
public:
    fncb();
    ~fncb(){};

    void recv(char* , unsigned int size){
        int i = 0;
    };
};

fncb::fncb(){
    REGISTER_RESPONSE_FUNCTION(fncb, recv);
    //TClsMemFnDelegate_2Param<void, char*, unsigned int> fn;
    //fn.BindRaw(this, &fncb::recv);
    //bool b = _response_tmp_fn.isbind();
    int i = 0;
}

int test_fncb(){
    boost::hash<std::string> string_hash;
    unsigned int hash_val = string_hash("43242");
    hash_val = string_hash("43242");
    fncb aa;
    return 0;
}

BYTE* gendata(unsigned short size){
    BYTE* d = nullptr;
    d = (BYTE*)calloc(size, 1);
    return d;
}

int test_package(){
    eEmuClient client(655, nullptr);
    char sz[] = "aaa454sd58";
    char szz2[] = "dsfdsa454sd58";
    client.testreqresponse(5, sz);
    client.testreqresponse(7, szz2);

    BYTE* data = gendata(200);
    DATACHANNELPACKET testpack;
    testpack.pbyData = data;
    testpack.wDataLen = 200;
    testpack.wPacketNo = 1;
    testpack.byCommandType = 3;
    CDataPacket cdp;
    DWORD sz1 = 0;
    data = cdp.BuildPacket(&testpack, sz1);

    BYTE* data2 = gendata(600);
    DATACHANNELPACKET testpack2;
    testpack2.pbyData = data2;
    testpack2.wDataLen = 600;
    testpack2.wPacketNo = 2;
    testpack2.byCommandType = 56;
    CDataPacket cdp2;
    DWORD sz2 = 0;
    data2 = cdp2.BuildPacket(&testpack2, sz2);

    BYTE* data3 = gendata(9000);
    DATACHANNELPACKET testpack3;
    testpack3.pbyData = data2;
    testpack3.wDataLen = 9000;
    testpack3.wPacketNo = 3;
    testpack3.byCommandType = 43;
    CDataPacket cdp3;
    DWORD sz3 = 0;
    data3 = cdp3.BuildPacket(&testpack3, sz3);

    
    BYTE* sumdata = (BYTE*)calloc(sz1+sz2+sz3, 1);
    memcpy(sumdata, data, sz1);
    memcpy(sumdata+sz1, data2, sz2-9);
    memcpy(sumdata+sz1+sz2-9, data3, sz3);

    cdp.FilterPacket(sumdata, sz1+sz2-9);
    cdp.FilterPacket(sumdata+sz1+sz2-9, sz3);
    return 0;
}

int test_webinterface(){
    ELOG("%s , %d", "lhhh", 878);
    eWebInterface web;
    //web.RunMsgPump();
    //std::thread t = web.Run();
    //web.WriteInQueue_Msg(std::shared_ptr<eWebRequestData>(new eWebRequestData));
    //t.join();
    return 0;
}