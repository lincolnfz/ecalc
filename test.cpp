
#include "test.h"
#include <iostream>
#include <mutex>
#include <memory>
#include <chrono>
#include <thread>
#include <unordered_map>
#include "./src/Data.h"

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

void test_unique_ptr(){
    std::unique_ptr<Data> ptr(new Data());
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