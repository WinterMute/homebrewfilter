#ifdef VWII
#define HaltGeckoThread()      	false
#define ResumeGeckoThread()	false
#define InitGeckoThread()	false
#define ShutdownGeckoThread()	false
#else
	void HaltGeckoThread();
	void ResumeGeckoThread();
	void InitGeckoThread();
	void ShutdownGeckoThread();
#endif