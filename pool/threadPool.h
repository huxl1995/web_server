#ifndef THREADPOOL_H
#define THREADPOOL_H

#include"taskQueue.h"
#include<mutex>
#include<thread>
#include<memory>
#include<iostream>
#include<condition_variable>
class ThreadPool
{
public:
    template<class T>
    void add(T&& task)
    {
        //std::lock_guard<std::mutex> locker(m_tex);
        m_tex.lock();
        tasks->add(task);
        cv.notify_one();
        m_tex.unlock();
    }
    ~ThreadPool()
    {
        stop=true;
        cv.notify_all();
        delete[] is_closed;
    }
//private:
    
    ThreadPool(int n=8)
    {
        is_closed=new bool[8];
        n_thread=n;
        for(int i=0;i<n;i++)
        {
            std::thread([&]{
                std::unique_lock<std::mutex> locker(m_tex);
                int p_num=i;
                while(true)
                {
                    if(!tasks->empty())
                    {
                        auto item=tasks->pop();
                        locker.unlock();
                        item();
                        locker.lock();
                    }
                    else if(stop&&tasks->empty()) break;
                    else cv.wait(locker);
                }
                is_closed[p_num]=true;
                printf("thread exit\n");
            }).detach();
        }
    }
    bool closed(){
        for(int i=0;i<n_thread;++i){
            std::cout<<is_closed[i]<<std::endl;
            if(!is_closed[i]){
                return false;
            }
        }
        return true;
    }
    void close()
    {
        for(int i=0;i<n_thread;++i)
        {
            is_closed[i]=true;
        }
        cv.notify_all();
    }
  //  static ThreadPool* threadpool=new ThreadPool();
private:
    bool stop=false;
    bool* is_closed;
    int n_thread;
    std::shared_ptr<Tasks> tasks=std::make_shared<Tasks>();
    std::mutex m_tex;
    std::condition_variable cv;
};

#endif
