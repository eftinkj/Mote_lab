#ifndef _TIMER_H_
#define _TIMER_H_


class timer {

	
	public:
		void start();
		void stop();
		virtual uint8_t handler() = 0;
  protected:
	uint32_t	duration;
	int8_t		tidx;
};


#endif // _TIMER_H_