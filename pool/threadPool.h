#ifndef THREADPOOL_H
#define THREADPOOL_H

#include"taskQueue.h"
#include<mutex>
#include<thread>
#include<memory>
#include<condition_variable>
class ThreadPool
{
public:
    // static ThreadPool* instance()
    // {
    //     return threadpool;
    // }
    template<class T>
    void add(T&& task)
    {
        std::lock_guard<std::mutex> locker(m_tex);
        tasks->add(task);
        cv.notify_one();
    }
    ~ThreadPool()
    {
        stop=true;
        cv.notify_all();
    }
//private:
    
    ThreadPool(int n=8)
    {
        
        for(int i=0;i<n;i++)
        {
            std::thread([&]{
                std::unique_lock<std::mutex> locker(m_tex);
                while(true)
                {
                    if(!tasks->empty())
                    {
                        auto item=tasks->pop();
                        locker.unlock();
                        item();
                        locker.lock();
                    }
                    else if(stop==true) break;
                    else cv.wait(locker);
                }
            }).detach();
        }
    }
    
  //  static ThreadPool* threadpool=new ThreadPool();
private:
    bool stop=false;
    std::shared_ptr<Tasks> tasks=std::make_shared<Tasks>();
    std::mutex m_tex;
    std::condition_variable cv;
};

#endif
