#ifndef BUFFER_H
#define BUFFER_H

#include<cstring>
#include<iostream>
#include<unistd.h>
#include<sys/uio.h>
#include<vector>
#include<atomic>
#include<assert.h>
class Buffer{
public:
    Buffer(int initBufferSize=1024);
    ~Buffer()=default;

    size_t WritableBytes() const;
    size_t ReadableBytes() const;
    size_t PrependableBytes() const;

    const char* Peek() const;
    void EnsureWriteable(size_t len);
    void HasWritten(size_t len);

    void Retrieve(size_t len);
    void RetrieveUntil(const char* end);

    void RetrieveAll();
    std::string RetrieveAllToStr();

    const char* BeginWriteConst() const;
    char* BeginWrite();

    void Append(const std::string& str);
    void Append(const char* str,size_t len);
    void Append(const void* data,size_t len);
    void Append(const Buffer& buff);

    ssize_t ReadFd(int fd,int* Errno);
    ssize_t WriteFd(int fd,int* Errno);

private:
    char* BeginPtr_(); //缓冲区起点
    const char* BeginPtr_() const;
    void MakeSpace_(size_t len);  //关键扩容函数

    std::vector<char>buffer_;  //底层以vector实现的缓冲区
    std::atomic<std::size_t> readPos_; //原子操作修饰的读下标
    std::atomic<std::size_t> writePos_; //原子操作修饰的写下标
};

#endif