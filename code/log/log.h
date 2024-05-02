#ifndef LOG_H
#define LOG_H

#include<mutex>
#include<string>
#include<thread>
#include<sys/time.h>
#include<string.h>
#include<stdio.h>
#include<stdarg.h>       //不定参数
#include<assert.h>
#include<sys/stat.h>
#include"blockqueue.h"
#include"../buffer/buffer.h"

class Log{
public:
    //初始化日志实例，包括阻塞队列最大容量，日志保存路径和日志文件后缀
    void init(int level,const char* path="./log",const char* suffix=".log",int maxQueueCapacity=1024);
    //获取实例的接口
    static Log* Instance();
    static void FlushLogThread();  //异步写日志共有方法，调用私有方法asyncwrite

    void write(int level,const char* format,...);
    void flush();

    int GetLevel();
    void SetLevel(int level);
    bool IsOpen(){return isOpen_;}

private:
    Log();
    void AppendLogLevelTitle_(int level);
    virtual ~Log();
    void AsyncWrite_();//异步写

private:
    static const int LOG_PATH_LEN=256;//最长文件名
    static const int LOG_NAME_LEN=256;//最长名称
    static const int MAX_LINES=50000; //日志文件最长日志数

    const char* path_; //路径
    const char* suffix_;//后缀

    int MAX_LINES_;   //最大日志行数

    int lineCount_;   //日志行数
    int toDay_;       //日期

    bool isOpen_;

    Buffer buff_;  //缓冲区
    int level_;    //日志等级
    bool isAsync_; //是否开启异步日志

    FILE* fp_;     //打开log的文件指针
    std::unique_ptr<BlockQueue<std::string>> deque_; //阻塞队列
    std::unique_ptr<std::thread> writeThread_; //写线程指针
    std::mutex mtx_;  //互斥用的锁
};


#define LOG_BASE(level,format,...) \
    do{\
        Log* log=Log::Instance();\
        if(log->IsOpen()&&log->GetLevel()<=level){\
            log->write(level,format,##__VA_ARGS__);\
            log->flush();\
        }  \
    }while(0);

#define LOG_DEBUG(format, ...) do {LOG_BASE(0, format, ##__VA_ARGS__)} while(0);    
#define LOG_INFO(format, ...) do {LOG_BASE(1, format, ##__VA_ARGS__)} while(0);
#define LOG_WARN(format, ...) do {LOG_BASE(2, format, ##__VA_ARGS__)} while(0);
#define LOG_ERROR(format, ...) do {LOG_BASE(3, format, ##__VA_ARGS__)} while(0);

#endif