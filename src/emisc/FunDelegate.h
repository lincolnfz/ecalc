#pragma once
#include <functional>

#define DELEGATE_BODY \
	bool isbind() const{ \
		return !(_funcptr == nullptr); \
	} \
\
	void unbind(){ \
		if(isbind()){ \
			std::function<RetType(ArgcTypes...)> tmp; \
			_funcptr.swap(tmp); \
		} \
	} \
\
	RetType call(ArgcTypes... arg) { \
 return _funcptr(arg...); \
	} \
\
	RetType operator()(ArgcTypes... arg) { \
		return _funcptr(arg...); \
	} \
\
  private: \
	std::function<RetType(ArgcTypes...)> _funcptr;

template<typename CLASSNAME, typename RetType>
struct DelegateClassMemFunNoParamsType
{
	using Type = RetType(CLASSNAME::*)();
};

template<typename CLASSNAME, typename RetType, typename... ArgcTypes>
struct DelegateClassMemFunWithParamsType
{
	using Type = RetType(CLASSNAME::*)(ArgcTypes...);
};

template<typename RetType>
class TClsMemFnDelegate_0Param {
public:
	template<typename CLASSNAME>
	void BindRaw(CLASSNAME* MyClass, typename DelegateClassMemFunNoParamsType<CLASSNAME, RetType>::Type FuncPtr) {
		_funcptr = std::bind(FuncPtr, MyClass);
	}

	~TClsMemFnDelegate_0Param(){
		unbind();
	}

	bool isbind() const{
		return !(_funcptr == nullptr);
	}

	void unbind(){
		if(isbind()){
			std::function<RetType()> tmp;
			_funcptr.swap(tmp);
		}
	}

	RetType call() {
		return _funcptr();
		//return true;
	}

	RetType operator()() {
		return _funcptr();
	}

private:
	std::function<RetType()> _funcptr;
};

template<typename RetType, typename... ArgcTypes>
class TClsMemFnDelegateBaseParam {
public:
	template<typename CLASSNAME>
	void BindRaw(CLASSNAME* MyClass, typename DelegateClassMemFunWithParamsType<CLASSNAME, RetType, ArgcTypes...>::Type FuncPtr) {}

	~TClsMemFnDelegateBaseParam(){
		unbind();
	}

	bool isbind() const{
		return !(_funcptr == nullptr);
	}

	void unbind(){
		if(isbind()){
			std::function<RetType(ArgcTypes...)> tmp;
			_funcptr.swap(tmp);
		}
	}

	virtual RetType call(ArgcTypes... arg) {
		return _funcptr(arg...);
	}

	virtual RetType operator()(ArgcTypes... arg) {
		return _funcptr(arg...);
	}

protected:
	std::function<RetType(ArgcTypes...)> _funcptr;
};

template<typename RetType, typename... ArgcTypes>
class TClsMemFnDelegate_1Param {
public:
	template<typename CLASSNAME>
	void BindRaw(CLASSNAME* MyClass, typename DelegateClassMemFunWithParamsType<CLASSNAME, RetType, ArgcTypes...>::Type FuncPtr) {
		_funcptr = std::bind(FuncPtr, MyClass, std::placeholders::_1);
	}

	~TClsMemFnDelegate_1Param(){
		unbind();
	}

	bool isbind() const{
		return !(_funcptr == nullptr);
	}

	void unbind(){
		if(isbind()){
			std::function<RetType(ArgcTypes...)> tmp;
			_funcptr.swap(tmp);
		}
	}

	RetType call(ArgcTypes... arg) {
		return _funcptr(arg...);
	}

	RetType operator()(ArgcTypes... arg) {
		return _funcptr(arg...);
	}

protected:
	std::function<RetType(ArgcTypes...)> _funcptr;
};

template<typename RetType, typename... ArgcTypes>
class TClsMemFnDelegate_2Param {
public:
	template<typename CLASSNAME>
	void BindRaw(CLASSNAME* MyClass, typename DelegateClassMemFunWithParamsType<CLASSNAME, RetType, ArgcTypes...>::Type FuncPtr) {
		_funcptr = std::bind(FuncPtr, MyClass, std::placeholders::_1, std::placeholders::_2);
	}

	~TClsMemFnDelegate_2Param(){
		unbind();
	}

	DELEGATE_BODY
};

template<typename RetType, typename... ArgcTypes>
class TClsMemFnDelegate_3Param {
public:
	template<typename CLASSNAME>
	void BindRaw(CLASSNAME* MyClass, typename DelegateClassMemFunWithParamsType<CLASSNAME, RetType, ArgcTypes...>::Type FuncPtr) {
		_funcptr = std::bind(FuncPtr, MyClass, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
	}

	~TClsMemFnDelegate_3Param(){
		unbind();
	}

	DELEGATE_BODY
};

template<typename RetType, typename... ArgcTypes>
class TClsMemFnDelegate_4Param {
public:
	template<typename CLASSNAME>
	void BindRaw(CLASSNAME* MyClass, typename DelegateClassMemFunWithParamsType<CLASSNAME, RetType, ArgcTypes...>::Type FuncPtr) {
		_funcptr = std::bind(FuncPtr, MyClass, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
	}

	~TClsMemFnDelegate_4Param(){
		unbind();
	}

	DELEGATE_BODY
};

template<typename RetType, typename... ArgcTypes>
class TClsMemFnDelegate_5Param {
public:
	template<typename CLASSNAME>
	void BindRaw(CLASSNAME* MyClass, typename DelegateClassMemFunWithParamsType<CLASSNAME, RetType, ArgcTypes...>::Type FuncPtr) {
		_funcptr = std::bind(FuncPtr, MyClass, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4,
			std::placeholders::_5);
	}

	~TClsMemFnDelegate_5Param(){
		unbind();
	}

	DELEGATE_BODY
};

template<typename RetType, typename... ArgcTypes>
class TClsMemFnDelegate_6Param {
public:
	template<typename CLASSNAME>
	void BindRaw(CLASSNAME* MyClass, typename DelegateClassMemFunWithParamsType<CLASSNAME, RetType, ArgcTypes...>::Type FuncPtr) {
		_funcptr = std::bind(FuncPtr, MyClass, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4,
			std::placeholders::_5, std::placeholders::_6);
	}

	~TClsMemFnDelegate_6Param(){
		unbind();
	}

	DELEGATE_BODY
};

template<typename RetType, typename... ArgcTypes>
class TClsMemFnDelegate_7Param {
public:
	template<typename CLASSNAME>
	void BindRaw(CLASSNAME* MyClass, typename DelegateClassMemFunWithParamsType<CLASSNAME, RetType, ArgcTypes...>::Type FuncPtr) {
		_funcptr = std::bind(FuncPtr, MyClass, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4,
			std::placeholders::_5, std::placeholders::_6, std::placeholders::_7);
	}

	~TClsMemFnDelegate_7Param(){
		unbind();
	}

	DELEGATE_BODY
};

template<typename RetType, typename... ArgcTypes>
class TClsMemFnDelegate_8Param {
public:
	template<typename CLASSNAME>
	void BindRaw(CLASSNAME* MyClass, typename DelegateClassMemFunWithParamsType<CLASSNAME, RetType, ArgcTypes...>::Type FuncPtr) {
		_funcptr = std::bind(FuncPtr, MyClass, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4,
			std::placeholders::_5, std::placeholders::_6, std::placeholders::_7, std::placeholders::_8);
	}

	~TClsMemFnDelegate_8Param(){
		unbind();
	}

	DELEGATE_BODY
};

template<typename RetType, typename... ArgcTypes>
class TClsMemFnDelegate_9Param {
public:
	template<typename CLASSNAME>
	void BindRaw(CLASSNAME* MyClass, typename DelegateClassMemFunWithParamsType<CLASSNAME, RetType, ArgcTypes...>::Type FuncPtr) {
		_funcptr = std::bind(FuncPtr, MyClass, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4,
			std::placeholders::_5, std::placeholders::_6, std::placeholders::_7, std::placeholders::_8, std::placeholders::_9);
	}

	~TClsMemFnDelegate_9Param(){
		unbind();
	}

	DELEGATE_BODY
};

template<typename RetType, typename... ArgcTypes>
class TClsMemFnDelegate_10Param {
public:
	template<typename CLASSNAME>
	void BindRaw(CLASSNAME* MyClass, typename DelegateClassMemFunWithParamsType<CLASSNAME, RetType, ArgcTypes...>::Type FuncPtr) {
		_funcptr = std::bind(FuncPtr, MyClass, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4,
			std::placeholders::_5, std::placeholders::_6, std::placeholders::_7, std::placeholders::_8, std::placeholders::_9,
			std::placeholders::_10);
	}

	~TClsMemFnDelegate_10Param(){
		unbind();
	}

	DELEGATE_BODY
};

template<typename RetType, typename... ArgcTypes>
class TClsMemFnDelegate_11Param {
public:
	template<typename CLASSNAME>
	void BindRaw(CLASSNAME* MyClass, typename DelegateClassMemFunWithParamsType<CLASSNAME, RetType, ArgcTypes...>::Type FuncPtr) {
		_funcptr = std::bind(FuncPtr, MyClass, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4,
			std::placeholders::_5, std::placeholders::_6, std::placeholders::_7, std::placeholders::_8, std::placeholders::_9,
			std::placeholders::_10, std::placeholders::_11);
	}

	~TClsMemFnDelegate_11Param(){
		unbind();
	}

	DELEGATE_BODY
};

template<typename RetType, typename... ArgcTypes>
class TClsMemFnDelegate_12Param {
public:
	template<typename CLASSNAME>
	void BindRaw(CLASSNAME* MyClass, typename DelegateClassMemFunWithParamsType<CLASSNAME, RetType, ArgcTypes...>::Type FuncPtr) {
		_funcptr = std::bind(FuncPtr, MyClass, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4,
			std::placeholders::_5, std::placeholders::_6, std::placeholders::_7, std::placeholders::_8, std::placeholders::_9,
			std::placeholders::_10, std::placeholders::_11, std::placeholders::_12);
	}

	~TClsMemFnDelegate_12Param(){
		unbind();
	}

	DELEGATE_BODY
};

template<typename RetType, typename... ArgcTypes>
class TClsMemFnDelegate_13Param {
public:
	template<typename CLASSNAME>
	void BindRaw(CLASSNAME* MyClass, typename DelegateClassMemFunWithParamsType<CLASSNAME, RetType, ArgcTypes...>::Type FuncPtr) {
		_funcptr = std::bind(FuncPtr, MyClass, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4,
			std::placeholders::_5, std::placeholders::_6, std::placeholders::_7, std::placeholders::_8, std::placeholders::_9,
			std::placeholders::_10, std::placeholders::_11, std::placeholders::_12, std::placeholders::_13);
	}

	~TClsMemFnDelegate_13Param(){
		unbind();
	}

	DELEGATE_BODY
};

template<typename RetType, typename... ArgcTypes>
class TClsMemFnDelegate_14Param {
public:
	template<typename CLASSNAME>
	void BindRaw(CLASSNAME* MyClass, typename DelegateClassMemFunWithParamsType<CLASSNAME, RetType, ArgcTypes...>::Type FuncPtr) {
		_funcptr = std::bind(FuncPtr, MyClass, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4,
			std::placeholders::_5, std::placeholders::_6, std::placeholders::_7, std::placeholders::_8, std::placeholders::_9,
			std::placeholders::_10, std::placeholders::_11, std::placeholders::_12, std::placeholders::_13, std::placeholders::_14);
	}

	~TClsMemFnDelegate_14Param(){
		unbind();
	}

	DELEGATE_BODY
};

template<typename RetType, typename... ArgcTypes>
class TClsMemFnDelegate_15Param {
public:
	template<typename CLASSNAME>
	void BindRaw(CLASSNAME* MyClass, typename DelegateClassMemFunWithParamsType<CLASSNAME, RetType, ArgcTypes...>::Type FuncPtr) {
		_funcptr = std::bind(FuncPtr, MyClass, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4,
			std::placeholders::_5, std::placeholders::_6, std::placeholders::_7, std::placeholders::_8, std::placeholders::_9,
			std::placeholders::_10, std::placeholders::_11, std::placeholders::_12, std::placeholders::_13, std::placeholders::_14,
			std::placeholders::_15);
	}

	~TClsMemFnDelegate_15Param(){
		unbind();
	}

	DELEGATE_BODY
};

template<typename RetType, typename... ArgcTypes>
class TClsMemFnDelegate_16Param {
public:
	template<typename CLASSNAME>
	void BindRaw(CLASSNAME* MyClass, typename DelegateClassMemFunWithParamsType<CLASSNAME, RetType, ArgcTypes...>::Type FuncPtr) {
		_funcptr = std::bind(FuncPtr, MyClass, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4,
			std::placeholders::_5, std::placeholders::_6, std::placeholders::_7, std::placeholders::_8, std::placeholders::_9,
			std::placeholders::_10, std::placeholders::_11, std::placeholders::_12, std::placeholders::_13, std::placeholders::_14,
			std::placeholders::_15, std::placeholders::_16);
	}

	~TClsMemFnDelegate_16Param(){
		unbind();
	}

	DELEGATE_BODY
};

template<typename RetType, typename... ArgcTypes>
class TClsMemFnDelegate_17Param {
public:
	template<typename CLASSNAME>
	void BindRaw(CLASSNAME* MyClass, typename DelegateClassMemFunWithParamsType<CLASSNAME, RetType, ArgcTypes...>::Type FuncPtr) {
		_funcptr = std::bind(FuncPtr, MyClass, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4,
			std::placeholders::_5, std::placeholders::_6, std::placeholders::_7, std::placeholders::_8, std::placeholders::_9,
			std::placeholders::_10, std::placeholders::_11, std::placeholders::_12, std::placeholders::_13, std::placeholders::_14,
			std::placeholders::_15, std::placeholders::_16, std::placeholders::_17);
	}

	~TClsMemFnDelegate_17Param(){
		unbind();
	}

	DELEGATE_BODY
};

template<typename RetType, typename... ArgcTypes>
class TClsMemFnDelegate_18Param {
public:
	template<typename CLASSNAME>
	void BindRaw(CLASSNAME* MyClass, typename DelegateClassMemFunWithParamsType<CLASSNAME, RetType, ArgcTypes...>::Type FuncPtr) {
		_funcptr = std::bind(FuncPtr, MyClass, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4,
			std::placeholders::_5, std::placeholders::_6, std::placeholders::_7, std::placeholders::_8, std::placeholders::_9,
			std::placeholders::_10, std::placeholders::_11, std::placeholders::_12, std::placeholders::_13, std::placeholders::_14,
			std::placeholders::_15, std::placeholders::_16, std::placeholders::_17, std::placeholders::_18);
	}

	~TClsMemFnDelegate_18Param(){
		unbind();
	}

	DELEGATE_BODY
};

template<typename RetType, typename... ArgcTypes>
class TClsMemFnDelegate_19Param {
public:
	template<typename CLASSNAME>
	void BindRaw(CLASSNAME* MyClass, typename DelegateClassMemFunWithParamsType<CLASSNAME, RetType, ArgcTypes...>::Type FuncPtr) {
		_funcptr = std::bind(FuncPtr, MyClass, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4,
			std::placeholders::_5, std::placeholders::_6, std::placeholders::_7, std::placeholders::_8, std::placeholders::_9,
			std::placeholders::_10, std::placeholders::_11, std::placeholders::_12, std::placeholders::_13, std::placeholders::_14,
			std::placeholders::_15, std::placeholders::_16, std::placeholders::_17, std::placeholders::_18, std::placeholders::_19);
	}

	~TClsMemFnDelegate_19Param(){
		unbind();
	}

	DELEGATE_BODY
};

template<typename RetType, typename... ArgcTypes>
class TClsMemFnDelegate_20Param {
public:
	template<typename CLASSNAME>
	void BindRaw(CLASSNAME* MyClass, typename DelegateClassMemFunWithParamsType<CLASSNAME, RetType, ArgcTypes...>::Type FuncPtr) {
		_funcptr = std::bind(FuncPtr, MyClass, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4,
			std::placeholders::_5, std::placeholders::_6, std::placeholders::_7, std::placeholders::_8, std::placeholders::_9,
			std::placeholders::_10, std::placeholders::_11, std::placeholders::_12, std::placeholders::_13, std::placeholders::_14,
			std::placeholders::_15, std::placeholders::_16, std::placeholders::_17, std::placeholders::_18, std::placeholders::_19,
			std::placeholders::_20);
	}

	~TClsMemFnDelegate_20Param(){
		unbind();
	}

	DELEGATE_BODY
};



/////////////////
/////////////////
/// bind static function
/////////////
/////////////

template<typename RetType>
struct DelegateRawFnNoParamsType
{
	using Type = RetType();
};

template<typename RetType, typename... ArgcTypes>
struct DelegateRawFnWithParamsType
{
	using Type = RetType(ArgcTypes...);
};

template<typename RetType>
class TRawFnDelegate_0Param {
public:
	void BindRaw(typename DelegateRawFnNoParamsType<RetType>::Type FuncPtr) {
		_funcptr = std::bind(FuncPtr);
	}

	~TRawFnDelegate_0Param(){
		unbind();
	}

	bool isbind() const{
		return !(_funcptr == nullptr);
	}

	void unbind(){
		if(isbind()){
			std::function<RetType()> tmp;
			_funcptr.swap(tmp);
		}
	}

	RetType call() {
		return _funcptr();
		//return true;
	}

	RetType operator()() {
		return _funcptr();
	}

private:
	std::function<RetType()> _funcptr;
};

template<typename RetType, typename... ArgcTypes>
class TRawFnDelegate_1Param {
public:
	void BindRaw(typename DelegateRawFnWithParamsType<RetType, ArgcTypes...>::Type FuncPtr) {
		_funcptr = std::bind(FuncPtr, std::placeholders::_1);
	}

	~TRawFnDelegate_1Param(){
		unbind();
	}

	bool isbind() const{
		return !(_funcptr == nullptr);
	}

	void unbind(){
		if(isbind()){
			std::function<RetType(ArgcTypes...)> tmp;
			_funcptr.swap(tmp);
		}
	}

	RetType call(ArgcTypes... arg) {
		return _funcptr(arg...);
	}

	RetType operator()(ArgcTypes... arg) {
		return _funcptr(arg...);
	}

protected:
	std::function<RetType(ArgcTypes...)> _funcptr;
};

template<typename RetType, typename... ArgcTypes>
class TRawFnDelegate_2Param {
public:
	void BindRaw(typename DelegateRawFnWithParamsType<RetType, ArgcTypes...>::Type FuncPtr) {
		_funcptr = std::bind(FuncPtr, std::placeholders::_1, std::placeholders::_2);
	}

	~TRawFnDelegate_2Param(){
		unbind();
	}

	DELEGATE_BODY
};

template<typename RetType, typename... ArgcTypes>
class TRawFnDelegate_3Param {
public:
	void BindRaw(typename DelegateRawFnWithParamsType<RetType, ArgcTypes...>::Type FuncPtr) {
		_funcptr = std::bind(FuncPtr, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
	}

	~TRawFnDelegate_3Param(){
		unbind();
	}

	DELEGATE_BODY
};

template<typename RetType, typename... ArgcTypes>
class TRawFnDelegate_4Param {
public:
	void BindRaw(typename DelegateRawFnWithParamsType<RetType, ArgcTypes...>::Type FuncPtr) {
		_funcptr = std::bind(FuncPtr, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
	}

	~TRawFnDelegate_4Param(){
		unbind();
	}

	DELEGATE_BODY
};

template<typename RetType, typename... ArgcTypes>
class TRawFnDelegate_5Param {
public:
	void BindRaw(typename DelegateRawFnWithParamsType<RetType, ArgcTypes...>::Type FuncPtr) {
		_funcptr = std::bind(FuncPtr, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4,
							std::placeholders::_5);
	}

	~TRawFnDelegate_5Param(){
		unbind();
	}

	DELEGATE_BODY
};

template<typename RetType, typename... ArgcTypes>
class TRawFnDelegate_6Param {
public:
	void BindRaw(typename DelegateRawFnWithParamsType<RetType, ArgcTypes...>::Type FuncPtr) {
		_funcptr = std::bind(FuncPtr, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4,
							std::placeholders::_5, std::placeholders::_6);
	}

	~TRawFnDelegate_6Param(){
		unbind();
	}


	DELEGATE_BODY
};

template<typename RetType, typename... ArgcTypes>
class TRawFnDelegate_7Param {
public:
	void BindRaw(typename DelegateRawFnWithParamsType<RetType, ArgcTypes...>::Type FuncPtr) {
		_funcptr = std::bind(FuncPtr, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4,
							std::placeholders::_5, std::placeholders::_6, std::placeholders::_7);
	}

	~TRawFnDelegate_7Param(){
		unbind();
	}

	DELEGATE_BODY
};

template<typename RetType, typename... ArgcTypes>
class TRawFnDelegate_8Param {
public:
	void BindRaw(typename DelegateRawFnWithParamsType<RetType, ArgcTypes...>::Type FuncPtr) {
		_funcptr = std::bind(FuncPtr, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4,
							std::placeholders::_5, std::placeholders::_6, std::placeholders::_7, std::placeholders::_8);
	}

	~TRawFnDelegate_8Param(){
		unbind();
	}

	DELEGATE_BODY
};

template<typename RetType, typename... ArgcTypes>
class TRawFnDelegate_9Param {
public:
	void BindRaw(typename DelegateRawFnWithParamsType<RetType, ArgcTypes...>::Type FuncPtr) {
		_funcptr = std::bind(FuncPtr, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4,
							std::placeholders::_5, std::placeholders::_6, std::placeholders::_7, std::placeholders::_8,
							std::placeholders::_9);
	}

	~TRawFnDelegate_9Param(){
		unbind();
	}

	DELEGATE_BODY
};

template<typename RetType, typename... ArgcTypes>
class TRawFnDelegate_10Param {
public:
	void BindRaw(typename DelegateRawFnWithParamsType<RetType, ArgcTypes...>::Type FuncPtr) {
		_funcptr = std::bind(FuncPtr, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4,
							std::placeholders::_5, std::placeholders::_6, std::placeholders::_7, std::placeholders::_8,
							std::placeholders::_9, std::placeholders::_10);
	}

	~TRawFnDelegate_10Param(){
		unbind();
	}

	DELEGATE_BODY
};

template<typename RetType, typename... ArgcTypes>
class TRawFnDelegate_11Param {
public:
	void BindRaw(typename DelegateRawFnWithParamsType<RetType, ArgcTypes...>::Type FuncPtr) {
		_funcptr = std::bind(FuncPtr, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4,
							std::placeholders::_5, std::placeholders::_6, std::placeholders::_7, std::placeholders::_8,
							std::placeholders::_9, std::placeholders::_10, std::placeholders::_11);
	}

	~TRawFnDelegate_11Param(){
		unbind();
	}

	DELEGATE_BODY
};

template<typename RetType, typename... ArgcTypes>
class TRawFnDelegate_12Param {
public:
	void BindRaw(typename DelegateRawFnWithParamsType<RetType, ArgcTypes...>::Type FuncPtr) {
		_funcptr = std::bind(FuncPtr, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4,
							std::placeholders::_5, std::placeholders::_6, std::placeholders::_7, std::placeholders::_8,
							std::placeholders::_9, std::placeholders::_10, std::placeholders::_11, std::placeholders::_12);
	}

	~TRawFnDelegate_12Param(){
		unbind();
	}


	DELEGATE_BODY
};

template<typename RetType, typename... ArgcTypes>
class TRawFnDelegate_13Param {
public:
	void BindRaw(typename DelegateRawFnWithParamsType<RetType, ArgcTypes...>::Type FuncPtr) {
		_funcptr = std::bind(FuncPtr, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4,
							std::placeholders::_5, std::placeholders::_6, std::placeholders::_7, std::placeholders::_8,
							std::placeholders::_9, std::placeholders::_10, std::placeholders::_11, std::placeholders::_12,
							std::placeholders::_13);
	}

	~TRawFnDelegate_13Param(){
		unbind();
	}

	DELEGATE_BODY
};

template<typename RetType, typename... ArgcTypes>
class TRawFnDelegate_14Param {
public:
	void BindRaw(typename DelegateRawFnWithParamsType<RetType, ArgcTypes...>::Type FuncPtr) {
		_funcptr = std::bind(FuncPtr, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4,
							std::placeholders::_5, std::placeholders::_6, std::placeholders::_7, std::placeholders::_8,
							std::placeholders::_9, std::placeholders::_10, std::placeholders::_11, std::placeholders::_12,
							std::placeholders::_13, std::placeholders::_14);
	}

	~TRawFnDelegate_14Param(){
		unbind();
	}

	DELEGATE_BODY
};

template<typename RetType, typename... ArgcTypes>
class TRawFnDelegate_15Param {
public:
	void BindRaw(typename DelegateRawFnWithParamsType<RetType, ArgcTypes...>::Type FuncPtr) {
		_funcptr = std::bind(FuncPtr, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4,
							std::placeholders::_5, std::placeholders::_6, std::placeholders::_7, std::placeholders::_8,
							std::placeholders::_9, std::placeholders::_10, std::placeholders::_11, std::placeholders::_12,
							std::placeholders::_13, std::placeholders::_14, std::placeholders::_15);
	}

	~TRawFnDelegate_15Param(){
		unbind();
	}

	DELEGATE_BODY
};

template<typename RetType, typename... ArgcTypes>
class TRawFnDelegate_16Param {
public:
	void BindRaw(typename DelegateRawFnWithParamsType<RetType, ArgcTypes...>::Type FuncPtr) {
		_funcptr = std::bind(FuncPtr, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4,
							std::placeholders::_5, std::placeholders::_6, std::placeholders::_7, std::placeholders::_8,
							std::placeholders::_9, std::placeholders::_10, std::placeholders::_11, std::placeholders::_12,
							std::placeholders::_13, std::placeholders::_14, std::placeholders::_15, std::placeholders::_16);
	}

	~TRawFnDelegate_16Param(){
		unbind();
	}

	DELEGATE_BODY
};

template<typename RetType, typename... ArgcTypes>
class TRawFnDelegate_17Param {
public:
	void BindRaw(typename DelegateRawFnWithParamsType<RetType, ArgcTypes...>::Type FuncPtr) {
		_funcptr = std::bind(FuncPtr, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4,
							std::placeholders::_5, std::placeholders::_6, std::placeholders::_7, std::placeholders::_8,
							std::placeholders::_9, std::placeholders::_10, std::placeholders::_11, std::placeholders::_12,
							std::placeholders::_13, std::placeholders::_14, std::placeholders::_15, std::placeholders::_16,
							std::placeholders::_17);
	}

	~TRawFnDelegate_17Param(){
		unbind();
	}

	DELEGATE_BODY
};

template<typename RetType, typename... ArgcTypes>
class TRawFnDelegate_18Param {
public:
	void BindRaw(typename DelegateRawFnWithParamsType<RetType, ArgcTypes...>::Type FuncPtr) {
		_funcptr = std::bind(FuncPtr, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4,
							std::placeholders::_5, std::placeholders::_6, std::placeholders::_7, std::placeholders::_8,
							std::placeholders::_9, std::placeholders::_10, std::placeholders::_11, std::placeholders::_12,
							std::placeholders::_13, std::placeholders::_14, std::placeholders::_15, std::placeholders::_16,
							std::placeholders::_17, std::placeholders::_18);
	}

	~TRawFnDelegate_18Param(){
		unbind();
	}

	DELEGATE_BODY
};

template<typename RetType, typename... ArgcTypes>
class TRawFnDelegate_19Param {
public:
	void BindRaw(typename DelegateRawFnWithParamsType<RetType, ArgcTypes...>::Type FuncPtr) {
		_funcptr = std::bind(FuncPtr, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4,
							std::placeholders::_5, std::placeholders::_6, std::placeholders::_7, std::placeholders::_8,
							std::placeholders::_9, std::placeholders::_10, std::placeholders::_11, std::placeholders::_12,
							std::placeholders::_13, std::placeholders::_14, std::placeholders::_15, std::placeholders::_16,
							std::placeholders::_17, std::placeholders::_18, std::placeholders::_19);
	}

	~TRawFnDelegate_19Param(){
		unbind();
	}

	DELEGATE_BODY
};

template<typename RetType, typename... ArgcTypes>
class TRawFnDelegate_20Param {
public:
	void BindRaw(typename DelegateRawFnWithParamsType<RetType, ArgcTypes...>::Type FuncPtr) {
		_funcptr = std::bind(FuncPtr, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4,
							std::placeholders::_5, std::placeholders::_6, std::placeholders::_7, std::placeholders::_8,
							std::placeholders::_9, std::placeholders::_10, std::placeholders::_11, std::placeholders::_12,
							std::placeholders::_13, std::placeholders::_14, std::placeholders::_15, std::placeholders::_16,
							std::placeholders::_17, std::placeholders::_18, std::placeholders::_19, std::placeholders::_20);
	}

	~TRawFnDelegate_20Param(){
		unbind();
	}

	DELEGATE_BODY
};