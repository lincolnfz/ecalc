#pragma once
#include <map>
#include "../emisc/FunDelegate.h"
//#include "Instruct.h"

enum FN_HANDLE_TYPE{
    FN_HANDLE_OK = 0,
    FN_HANDLE_NIL = -1,
};

#define REGISTER_RESPONSE_FUNCTION(class_name, function_name) \
    _response_tmp_fn.BindRaw(this, &class_name::function_name); \
	this->RegisterResponseFunction( #function_name , _response_tmp_fn ); \
    _response_tmp_fn.unbind();

#define REGISTER_ACK_FUNCTION(class_name, function_name) \
    _ack_tmp_fn.BindRaw(this, &class_name::function_name); \
	this->RegisterAckFunction( #function_name , _ack_tmp_fn ) \
    _ack_tmp_fn.unbind();

template<class Fn_T>
class ResponseHandle
{
public:
	ResponseHandle(){};
	virtual ~ResponseHandle(){};

protected:
    template<typename... ArgcTypes>
    FN_HANDLE_TYPE HandleResponse(char* name, ArgcTypes... args){
        FN_HANDLE_TYPE err = FN_HANDLE_NIL;
        auto fnit = _response_fn_map.find(std::string(name));
        if(fnit != _response_fn_map.end()){
            fnit->second(args...);
            err = FN_HANDLE_OK;
        }
        return err;
    };
	//bool handleQuest(const int id, const std::shared_ptr<cyjh::Instruct>, std::shared_ptr<cyjh::Instruct>);

    void RegisterResponseFunction(const char* fn_Name, Fn_T fn_obj){
        _response_fn_map.insert(std::make_pair(std::string(fn_Name), fn_obj));
    };

	std::map<std::string, Fn_T> _response_fn_map;
    Fn_T _response_tmp_fn;
};

template<class Fn_T>
class AckHandle
{
public:
	AckHandle(){};
	virtual ~AckHandle(){};

protected:
    template<typename... ArgcTypes>
    FN_HANDLE_TYPE HandleAck(char* name, ArgcTypes... args){
        FN_HANDLE_TYPE err = FN_HANDLE_NIL;
        auto fnit = _ack_fn_map.find(std::string(name));
        if(fnit != _ack_fn_map.end()){
            fnit->second(args...);
            err = FN_HANDLE_OK;
        }
        return err;
    };

    void RegisterAckFunction(const char* fn_Name, Fn_T fn_obj){
        _ack_fn_map.insert(std::make_pair(std::string(fn_Name), fn_obj));
    };

    std::map<std::string, Fn_T> _ack_fn_map;
    Fn_T _ack_tmp_fn;
};


#define REGISTER_RESPONSE_0x_FUNCTION(class_name, function_name, sign) \
    _response_tmp_fn_0x.BindRaw(this, &class_name::function_name); \
	this->RegisterResponseFunction_0x( sign , _response_tmp_fn_0x ); \
    _response_tmp_fn_0x.unbind();

#define REGISTER_ACK_0x_FUNCTION(class_name, function_name, sign) \
    _ack_tmp_fn_0x.BindRaw(this, &class_name::function_name); \
	this->RegisterAckFunction( sign , _ack_tmp_fn_0x ) \
    _ack_tmp_fn_0x.unbind();

template<class Fn_T>
class ResponseHandle_0x
{
public:
	ResponseHandle_0x(){};
	virtual ~ResponseHandle_0x(){};

protected:
    template<typename... ArgcTypes>
    FN_HANDLE_TYPE HandleResponse_0x(int sign, ArgcTypes... args){
        FN_HANDLE_TYPE err = FN_HANDLE_NIL;
        auto fnit = _response_0x_fn_map.find(sign);
        if(fnit != _response_0x_fn_map.end()){
            fnit->second(args...);
            err = FN_HANDLE_OK;
        } 
        return err;
    };
	//bool handleQuest(const int id, const std::shared_ptr<cyjh::Instruct>, std::shared_ptr<cyjh::Instruct>);

    void RegisterResponseFunction_0x(const int sign, Fn_T fn_obj){
        _response_0x_fn_map.insert(std::make_pair(sign, fn_obj));
    };

	std::map<int, Fn_T> _response_0x_fn_map;
    Fn_T _response_tmp_fn_0x;
};

template<class Fn_T>
class AckHandle_0x
{
public:
	AckHandle_0x(){};
	virtual ~AckHandle_0x(){};

protected:
    template<typename... ArgcTypes>
    FN_HANDLE_TYPE HandleAck_0x(int sign, ArgcTypes... args){
        FN_HANDLE_TYPE err = FN_HANDLE_NIL;
        auto fnit = _ack_fn_map_0x.find(sign);
        if(fnit != _ack_fn_map_0x.end()){
            fnit->second(args...);
            err = FN_HANDLE_OK;
        }
        return err;
    };

    void RegisterAckFunction_0x(const int sign, Fn_T fn_obj){
        _ack_fn_map_0x.insert(std::make_pair(sign, fn_obj));
    };

    std::map<std::string, Fn_T> _ack_fn_map_0x;
    Fn_T _ack_tmp_fn_0x;
};