#ifndef EVENT_H
#define EVENT_H
#pragma once  

#include <fcntl.h>
#include <atomic>
#include <time.h>
#include <semaphore.h>

class eEvent {
private:
	eEvent(long lpEventAttributes, bool bManualReset, bool bInitialState, const char* lpName) {
		_lpEventAttributes = lpEventAttributes;
		_b_manual_reset = bManualReset;
		_b_initial_state = bInitialState;
		if (lpName != nullptr) _sem_name = lpName;
	}
	virtual ~eEvent()
	{
		if (_p_named_sem != SEM_FAILED)
		{
			if (_sem_name.empty())
			{
				sem_destroy(&_sem);
				_p_named_sem = SEM_FAILED;
			}
			else 
			{
				sem_close(_p_named_sem);
				sem_unlink(_sem_name.c_str());
				_sem_name.clear();
			}
		}
		
	}
    // 获取自系统启动的调单递增的时间
    inline uint64_t GetTimeConvSeconds( timespec* curTime, uint32_t factor ) {
        // CLOCK_MONOTONIC：从系统启动这一刻起开始计时,不受系统时间被用户改变的影响
        clock_gettime( CLOCK_MONOTONIC, curTime );
        return static_cast<uint64_t>(curTime->tv_sec) * factor;
    }

    // 获取自系统启动的调单递增的时间 -- 转换单位为微秒
    uint64_t GetMonnotonicTime() {
        timespec curTime;
        uint64_t result = GetTimeConvSeconds( &curTime, 1000000 );
        result += static_cast<uint32_t>(curTime.tv_nsec) / 1000;
        return result;
    }
	bool Init()
	{
		if (_b_initial_state) _sem_count = 1;
		else _sem_count = 0;
		if (_sem_name.empty())
		{
			if (0 != sem_init(&_sem, 0, _sem_count))
			{
				_p_named_sem = SEM_FAILED;
				return false;
			}
			_p_named_sem = &_sem; 
		}
		else {
			_p_named_sem = sem_open(_sem_name.c_str(), O_CREAT, _lpEventAttributes, _sem_count);
			if (_p_named_sem == SEM_FAILED)
			{
				sem_unlink(_sem_name.c_str());
				_sem_name.clear();
				return false;
			}
		}
		return true;
	}
	timespec sem_get_time_millsecs(long msecs)
	{
		struct timespec ts;
		clock_gettime(CLOCK_REALTIME, &ts);
		long secs = msecs / 1000;
		msecs = msecs % 1000;

		long add = 0;
		msecs = msecs * 1000 * 1000 + ts.tv_nsec;
		add = msecs / (1000 * 1000 * 1000);
		ts.tv_sec += (add + secs);
		ts.tv_nsec = msecs % (1000 * 1000 * 1000);
		return ts;
	}

    // sem_trywait + usleep的方式实现
    // 如果信号量大于0，则减少信号量并立马返回true
    // 如果信号量小于0，则阻塞等待，当阻塞超时时返回false
    unsigned long wait( size_t timeout ) {
        const size_t timeoutUs = timeout * 1000; // 延时时间由毫米转换为微秒
        const size_t maxTimeWait = 10000; // 最大的睡眠的时间为10000微秒，也就是10毫秒

        size_t timeWait = 1; // 睡眠时间，默认为1微秒
        size_t delayUs = 0; // 剩余需要延时睡眠时间

        const uint64_t startUs = GetMonnotonicTime(); // 循环前的开始时间，单位微秒
        uint64_t elapsedUs = 0; // 过期时间，单位微秒

        int ret = 0;

        do {
            // 如果信号量大于0，则减少信号量并立马返回true
            if( sem_trywait( _p_named_sem ) == 0 )
            {
                return WAIT_OBJECT_0;
            }

            // 系统信号则立马返回false
            if( errno != EAGAIN )
            {
                return WAIT_FAILED;
            }

            // delayUs一定是大于等于0的，因为do-while的条件是elapsedUs <= timeoutUs.
            delayUs = timeoutUs - elapsedUs;

            // 睡眠时间取最小的值
            timeWait = std::min( delayUs, timeWait );

            // 进行睡眠 单位是微秒
            ret = usleep( timeWait );
            if( ret != 0 )
            {
                return WAIT_FAILED;
            }

            // 睡眠延时时间双倍自增
            timeWait *= 2;

            // 睡眠延时时间不能超过最大值
            timeWait = std::min( timeWait, maxTimeWait );

            // 计算开始时间到现在的运行时间 单位是微秒
            elapsedUs = GetMonnotonicTime() - startUs;
        } while( elapsedUs <= timeoutUs ); // 如果当前循环的时间超过预设延时时间则退出循环

        // 超时退出，则返回false
        return WAIT_TIMEOUT;
    }
public:
	enum {
		WAIT_OBJECT_0 = 0,
		WAIT_TIMEOUT = 0x00000102L,
		WAIT_FAILED = 0xFFFFFFFF
	};
	/// <summary>
	/// 创建事件
	/// </summary>
	/// <param name="lpEventAttributes">安全属性，0777</param>
	/// <param name="bManualReset">复位方式</param>
	/// <param name="bInitialState">初始状态</param>
	/// <param name="lpName">事件名称</param>
	/// <returns>eEvent*</returns>
	static eEvent* CreateEvent(long lpEventAttributes, bool bManualReset, bool bInitialState, const char* lpName) {
		eEvent* p_event = new eEvent(lpEventAttributes, bManualReset, bInitialState, lpName);
		if (!p_event->Init())
		{
			delete p_event;
			return nullptr;
		}
		return p_event;
	}
	/// <summary>
	/// 关闭事件
	/// </summary>
	/// <param name="p_event">eEvent*</param>
	static void CloseHandle(eEvent* &p_event) {
		if (p_event != nullptr)
			delete p_event;
		p_event = nullptr;
	}

	/// <summary>
	/// 等待事件
	/// </summary>
	/// <param name="p_event">事件</param>
	/// <param name="ms">超时时间，0 永不超时</param>
	/// <returns>WAIT_OBJECT_0/WAIT_TIMEOUT/WAIT_FAILED</returns>
	static unsigned long WaitForSingleObject(eEvent* p_event, long ms)
	{
		if (p_event == nullptr)
			return WAIT_FAILED;
		if (p_event->_b_initial_state) {
			if (!p_event->_b_manual_reset) {
				p_event->_b_initial_state = false;
				while (sem_getvalue(p_event->_p_named_sem, &p_event->_sem_count) == 0 && p_event->_sem_count > 0)
					sem_wait(p_event->_p_named_sem);
			}
			return WAIT_OBJECT_0;
		}
		if (ms == 0) {
			int n_ret = sem_wait(p_event->_p_named_sem);
			if (n_ret != 0)
				return WAIT_FAILED;
		}
		else {
			// int n_ret = 0;
			// timespec ts = p_event->sem_get_time_millsecs(ms);
			// while ((n_ret = sem_timedwait(p_event->_p_named_sem, &ts)) == -1 && errno == EINTR)
			// 	continue;       /* Restart if interrupted by handler */
			// if (n_ret == -1) {
			// 	if (errno == ETIMEDOUT)
			// 		return WAIT_TIMEOUT;
			// 	else
			// 		return WAIT_FAILED;
			// }
            return p_event->wait(ms);
		}
		if (p_event->_b_manual_reset)
			p_event->_b_initial_state = true;
		return WAIT_OBJECT_0;
	}
	/// <summary>
	/// 触发事件
	/// </summary>
	/// <param name="p_event">事件</param>
	/// <returns>true/false</returns>
	static bool SetEvent(eEvent* p_event)
	{
		if (p_event == nullptr) return false;
		int n_ret = sem_post(p_event->_p_named_sem);
		if (n_ret != 0)
			return false;
		return true;
	}
	/// <summary>
	/// 复位事件
	/// </summary>
	/// <param name="p_event">事件</param>
	/// <returns>true/false</returns>
	static bool ResetEvent(eEvent* p_event)
	{
		if (p_event == nullptr) return false;
		p_event->_b_initial_state = false;
		while (sem_getvalue(p_event->_p_named_sem, &p_event->_sem_count) == 0 && p_event->_sem_count > 0)
			sem_wait(p_event->_p_named_sem);
		return true;
	}
private:
	bool _b_manual_reset;// 复位方式
	std::atomic_bool _b_initial_state;// 初始状态

	sem_t* _p_named_sem = SEM_FAILED;
	sem_t  _sem;
	std::string	_sem_name;
	long _lpEventAttributes;
	int _sem_count = 0;
};

#endif