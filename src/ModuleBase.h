#pragma once
#include <regex>

#define REGISTER_HANDLER_MEMBER_FUNCTION(class_name, function_name) \
	this->RegisterFunction( #function_name , &class_name::##function_name , this )

#define REGISTER_HANDLER_MEMBER_FUNCTION_EX(storage_ptr , class_name, function_name) \
	storage_ptr->RegisterFunction( #function_name , &class_name::##function_name , this )

#define REGISTER_HANDLER_STATIC_FUNCTION(storage_ptr , function_name) \
	storage_ptr->RegisterFunction( #function_name , &function_name )

class ModuleBase{
public:
    ModuleBase();
    virtual ~ModuleBase();
	virtual const char* className() {
		if(_class_name.empty()){
			const std::type_info& cinfo = typeid(*this);
			_class_name = cinfo.name();
		}
	    return _class_name.c_str();
	};

protected:
	std::string _class_name;
};