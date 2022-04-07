#pragma once
#include <map>
#include "Instruct.h"

#define REGISTER_RESPONSE_FUNCTION(class_name, function_name) \
    _tmp_fn.BindRaw(this, &class_name::function_name); \
	this->RegisterResponseFunction( #function_name , _tmp_fn ); \
    _tmp_fn.unbind();

#define REGISTER_ACK_FUNCTION(class_name, function_name) \
    _tmp_fn.BindRaw(this, &class_name::function_name); \
	this->RegisterAckFunction( #function_name , _tmp_fn ) \
    _tmp_fn.unbind();

template<class Fn_T>
class ResponseHandle
{
public:
	ResponseHandle(){};
	virtual ~ResponseHandle(){};
    void HandleResponse(){

    };
	//bool handleQuest(const int id, const std::shared_ptr<cyjh::Instruct>, std::shared_ptr<cyjh::Instruct>);

    void RegisterResponseFunction(const char* fn_Name, Fn_T fn_obj){
        _response_fn_map.insert(std::make_pair(std::string(fn_Name), fn_obj));
    };

protected:
	std::map<std::string, Fn_T> _response_fn_map;
    Fn_T _tmp_fn;
};

template<class Fn_T>
class AckHandle
{
public:
	AckHandle(){};
	virtual ~AckHandle(){};

    void HandleAck(){

    };

    void RegisterAckFunction(const char* fn_Name, Fn_T fn_obj){
        _ack_fn_map.insert(std::make_pair(std::string(fn_Name), fn_obj));
    };

protected:
    std::map<std::string, Fn_T> _ack_fn_map;
    Fn_T _tmp_fn;
};