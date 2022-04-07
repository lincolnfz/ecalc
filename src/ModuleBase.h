#pragma once
#include <regex>
#include "./emisc/FunDelegate.h"
#include "eDataLayer.h"

#define REGISTER_HANDLER_MEMBER_FUNCTION(class_name, function_name) \
	this->RegisterFunction( #function_name , &class_name::##function_name , this )

#define REGISTER_HANDLER_MEMBER_FUNCTION_EX(storage_ptr , class_name, function_name) \
	storage_ptr->RegisterFunction( #function_name , &class_name::##function_name , this )

#define REGISTER_HANDLER_STATIC_FUNCTION(storage_ptr , function_name) \
	storage_ptr->RegisterFunction( #function_name , &function_name )

#define CLASS_NAME(class_d) \
	typeid(class_d).name()

template<class T_MSG>
class ModuleBase:public eDataLayer<T_MSG>::I_Process_Data_Base {
public:
    ModuleBase(){};
    virtual ~ModuleBase(){};
	virtual const char* className() {
		if(_class_name.empty()){
			const std::type_info& cinfo = typeid(*this);
			_class_name = cinfo.name();
		}
	    return _class_name.c_str();
	};

	//注册成员回调函数
	/*template<typename T>
	bool RegisterFunction( const char* pFunctionName , BSTR (T::*function)(unsigned long, BSTR , unsigned long) , T* obj  )
	{
		boost::hash<std::string> string_hash;
		return m_FunctionMap.insert( std::make_pair(string_hash( pFunctionName ) , 
			boost::bind( function , obj , _1 , _2 , _3) )).second;
	}*/

	//注册普通回调函数
	/*bool RegisterFunction( const char* pFunctionName , MethodCallBackFunc functoin )
	{
		boost::hash<std::string> string_hash;
		return m_FunctionMap.insert( std::make_pair(string_hash( pFunctionName ) , functoin ) ).second;
	}*/

	virtual void hadleNotifyMsg(std::shared_ptr<T_MSG>) override{
		
	}

private:
	std::string _class_name;
};