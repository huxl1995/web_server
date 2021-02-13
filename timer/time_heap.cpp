#include "time_heap.h"
Timer_heap::Timer_heap(int cap)
{
    capacity = cap;
    cur_size = 0;
    array = new Timer *[capacity];
    for (int i = 0; i < capacity; i++)
    {
        array[i] = NULL;
    }
}
Timer_heap::Timer_heap(Timer **init_array, int size, int cap)
{
    capacity = cap;
    cur_size = size;
    if (capacity < size)
    {
        capacity = size;
    }
    array = new Timer *[capacity]; //创建堆数组
    for (int i = 0; i < capacity; i++)
    {
        array[i] = NULL;
    }
    if (size != 0)
    {
        for (int i = 0; i < size; i++)
        {
            array[i] = init_array[i];
        }
        for (int i = (cur_size - 1) / 2; i >= 0; --i)
        {
            //对数组中的第(cursize-1)/2~0个元素执行下滤操作
            percolate_down(i);
        }
    }
}
Timer_heap::~Timer_heap()
{
    for (int i = 0; i < cur_size; i++)
    {
        delete array[i];
    }
    delete[] array;
}

void Timer_heap::add_timer(Timer *timer)
{
    if (!timer)
    {
        return;
    }
    if (cur_size >= capacity) //如果当前堆数组容量不共，则将其扩大一倍
    {
        resize();
    }
    //新插入一个元素，当前堆大小加1，hole是新建空穴的位置
    int hole = cur_size++;
    int parent = 0;
    //对从空穴到根节点的路径上的所有定时器执行上滤操作
    for (; hole > 0; hole = parent)
    {
        parent = (hole - 1) / 2;
        if (array[parent]->expire <= timer->expire)
        {
            break;
        }
        array[hole] = array[parent];
    }
    timer->index = hole;
    array[hole] = timer;
}
//删除目标定时器timer
void Timer_heap::del_timer(Timer *timer)
{
    if (!timer)
    {
        return;
    }
    timer->cb_func = NULL;
}
//获得堆部的定时器
Timer *Timer_heap::top() const
{
    if (empyt())
    {
        return NULL;
    }
    return array[0];
}
//删除堆顶部的定时器
void Timer_heap::pop_timer()
{
    if (empyt())
    {
        return;
    }
    if (array[0])
    {
        delete array[0];
        //将原来的堆顶元素替换为堆数组中最后一个元素
        array[0] = array[--cur_size];
        array[0]->index = 0;
        //对新的堆顶元素执行下滤操作
        percolate_down(0);
    }
}
//心搏函数
void Timer_heap::tick()
{
    Timer *tmp = array[0];
    time_t cur = time(NULL);
    while (!empyt())
    {
        if (!tmp)
        {
            break;
        }
        //如果堆顶定时器没到期，则推出循环
        if (tmp->expire > cur)
        {
            break;
        }
        //否则就执行堆定定时器中的任务
        if (array[0]->cb_func)
        {
            //printf("time close %d\n", time(NULL));
            array[0]->cb_func();
        }
        //将堆顶的定时器删除，同时生成新的堆顶定时器(array[0])
        pop_timer();
        tmp = array[0];
    }
}
bool Timer_heap::empyt() const
{
    return cur_size == 0;
}
//最小堆的下滤操作，它确保数组中第hole个节点作为根的子树拥有最小堆性质
void Timer_heap::percolate_down(int hole)
{
    Timer *temp = array[hole];
    int child = 0;
    for (; (hole * 2 + 1) <= (cur_size - 1); hole = child)
    {
        if (child < (cur_size - 1) && array[child + 1]->expire < array[child]->expire)
        {
            ++child;
        }
        if (array[child]->expire < temp->expire)
        {
            array[hole] = array[child];
            array[hole]->index = hole;
        }
        else
        {
            break;
        }
    }
    temp->index = hole;
    array[hole] = temp;
}
//将数组容量扩大1倍
void Timer_heap::resize()
{
    Timer **temp = new Timer *[2 * capacity];
    for (int i = 0; i < 2 * capacity; i++)
    {
        temp[i] = NULL;
    }
    capacity = 2 * capacity;
    for (int i = 0; i < cur_size; i++)
    {
        temp[i] = array[i];
    }
    delete[] array;
    array = temp;
}
void Timer_heap::adjust(Timer *timer)
{
    percolate_down(timer->index);
}