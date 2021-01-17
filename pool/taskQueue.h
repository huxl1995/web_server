#ifndef TASKQUEUE_H
#define TASKQUEUE_H

#include<queue>
#include<functional>
#include<assert.h>
struct Tasks
{
    private:
    std::queue<std::function<void()>> tasks;
    public:
    template<class T>
    void add(T&& task)
    {
        
        tasks.emplace(task);
    }
    
    auto pop()
    {
        assert(!tasks.empty());
        auto task=tasks.front();
        tasks.pop();
        return task;
    }
    
    bool empty()
    {
    	return tasks.empty();
    }
};
#endif
