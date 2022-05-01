#ifndef TIME_HEAP_H
#define TIME_HEAP_H
#include <functional>
#include <netinet/in.h>
#include <time.h>
struct Client_data;
class Timer //定时器类
{
public:
    inline void init(int delay)
    {
        expire=time(NULL)+delay;
        index=0;
    }
    int index;     //定时器在时间堆数组中的index
    time_t expire; //定时器生效的绝对时间
    std::function<void()> cb_func; //定时器回调函数
    //Client_data *user_data; //用户数据
};
struct client_data
{
    sockaddr_in addr;
    int sock_fd;
    Timer *timer;
};

class Timer_heap //时间堆类
{
public:
    Timer_heap(int cap); //初始化一个大小为cap的堆
    Timer_heap(Timer **init_array,
               int size, int capacity); //用已有数组来初始化堆
    ~Timer_heap();
    void adjust(Timer *timer);    //调整timer的位置
    void add_timer(Timer *timer); //添加目标定时器
    void del_timer(Timer *timer); //删除目标定时器
    Timer *top() const;           //获取堆定定时器
    void pop_timer();             //获取堆顶部定时器
    void tick();                  //心搏函数
    bool empyt() const;           //堆是否为空
private:
    void percolate_down(int hole); //最小堆的下滤操作，
    void resize();                 //将数组容量扩大一倍

    Timer **array; //堆数组
    int capacity;  //堆数组容量
    int cur_size;  //堆数组当前包含元素的个数
};

#endif