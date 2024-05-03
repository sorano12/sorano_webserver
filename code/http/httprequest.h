#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H

#include<unordered_map>
#include<unordered_set>
#include<string>
#include<regex>
#include<errno.h>
#include<mysql/mysql.h>

#include"../buffer/buffer.h"
#include"../log/log.h"
#include"../pool/sqlconnpool.h"

class HttpRequest{
public:
    //parse处理的枚举状态，分别代表解析请求头，请求请求行，请求体和结束
    enum PARSE_STATE{
        REQUEST_LINE,
        HEADERS,
        BODY,
        FINISH,
    };
    HttpRequest() {Init();}
    ~HttpRequest()=default;

    void Init();
    bool parse(Buffer& buff);

    std::string path() const;
    std::string& path();
    std::string method() const;
    std::string version() const;
    std::string GetPost(const std::string& key) const;
    std::string GetPost(const char* key)const;

    bool IsKeepAlive() const;

private:
    bool ParseRequestLine_(const std::string& line);
    void ParseHeader_(const std::string& line);
    void ParseBody_(const std::string& line);

    void ParsePath_(); //处理请求路径
    void ParsePost_(); //处理POST请求
    void ParseFromUrlencoded_(); //解析编码

    static bool UserVerify(const std::string& name,const std::string&pwd,bool isLogin); //用户验证

    PARSE_STATE state_; //状态枚举
    std::string method_,path_,version_,body_;
    std::unordered_map<std::string,std::string>header_;
    std::unordered_map<std::string,std::string> post_;

    static const std::unordered_set<std::string> DEFAULT_HTML;
    static const std::unordered_map<std::string,int> DEFAULT_HTML_TAG;
    static int ConverHex(char ch);  //进制转化


};


#endif