#include "buffer.h"

//初始化
Buffer::Buffer(int initBufferSize):buffer_(initBufferSize),readPos_(0),writePos_(0){}

//可写数量
size_t Buffer::WritableBytes() const{
    return buffer_.size()-writePos_;
}

//可读数量(左闭右开)
size_t Buffer:: ReadableBytes() const{
    return writePos_-readPos_;
}

//可预留数量
size_t Buffer:: PrependableBytes() const{
    return readPos_;
}

//可读起点
const char* Buffer::Peek() const{
    return &buffer_[readPos_];
}

//确保可写的长度
void Buffer::EnsureWriteable(size_t len){
    if(len>WritableBytes()){
        MakeSpace_(len);
    }
    assert(len<=WritableBytes());
}

//写了len长度，移动写下标

void Buffer::HasWritten(size_t len){
    writePos_+=len;
}

//读取len长度，移动读下标
void Buffer::Retrieve(size_t len){
    readPos_+=len;
}

//读取到end位置
void Buffer::RetrieveUntil(const char* end){
    assert(Peek()<=end);
    Retrieve(end-Peek());
}

//取出所有数据，buffer归零，读写下标归零
void Buffer::RetrieveAll(){
    bzero(&buffer_[0],buffer_.size());
    readPos_=writePos_=0;
}

//取出剩余可读str
std::string Buffer::RetrieveAllToStr(){
    std::string str(Peek(),ReadableBytes());
    RetrieveAll();
    return str;
}

//写指针的位置
const char* Buffer::BeginWriteConst() const{
    return &buffer_[writePos_];
}

char* Buffer::BeginWrite(){
    return &buffer_[writePos_];
}

//添加str到缓冲区
void Buffer::Append(const char* str,size_t len){
    assert(str);
    EnsureWriteable(len); //确保可写，进行可能的扩容
    std::copy(str,str+len,BeginWrite());
    HasWritten(len);
}

void Buffer::Append(const std::string& str){
    Append(str.c_str(),str.size());
}

void Append(const void* data,size_t len){
    Append(static_cast<const char*>(data),len);
}

//将buffer中可读部分append到buff中
void Append(const Buffer& buff){
    Append(buff.Peek(),buff.ReadableBytes());
}

//将fd的内容赌到缓冲区
ssize_t Buffer::ReadFd(int fd,int* Errno){
    char buff[65535]; //栈上空间用于读写缓冲
    struct iovec iov[2];
    size_t Writeable=WritableBytes();
    //分散度，保证数据读完
    iov[0].iov_base=BeginWrite();
    iov[0].iov_len=Writeable;
    iov[1].iov_base=buff;
    iov[1].iov_len=sizeof(buff);

    ssize_t len=readv(fd,iov,2);

    if(len<0){
        *Errno=errno;
    }
    else if(static_cast<size_t>(len)<=Writeable){
        writePos_+=len;      //写的长度小于本身缓冲区可写长度，直接移动写下标
    }
    else{
        writePos_=buffer_.size(); //len大于可写区，原缓冲区写满，写下标移到最后
        Append(buff,static_cast<size_t>(len-Writeable));  //将栈上缓冲也写到原缓冲区
    }
    return len;

}

//将buffer中可读区域写入fd
ssize_t Buffer::WriteFd(int fd,int* Errno){
    ssize_t len=write(fd,Peek(),ReadableBytes());
    if(len<0){
        *Errno=errno;
        return len;
    }
    Retrieve(len);
    return len;
}

//扩容
void Buffer::MakeSpace_(size_t len){
    if(WritableBytes()+PrependableBytes()<len){
    buffer_.resize(writePos_+len+1);
    }
    else{
        size_t readable=ReadableBytes();
        std::copy(BeginPtr_()+readPos_,BeginPtr_()+writePos_,BeginPtr_());
        readPos_=0;
        writePos_=readable;
        assert(readable==ReadableBytes());
    }

}