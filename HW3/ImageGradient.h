#ifndef IMAGE_GRADIENT_H_
#define IMAGE_GRADIENT_H_
#include <systemc>
using namespace sc_core;

#ifndef NATIVE_SYSTEMC
#include <cynw_p2p.h>
#endif

#include "filter_def.h"

class ImageGradient: public sc_module
{
public:
	sc_in_clk i_clk;
	sc_in < bool >  i_rst;
#ifndef NATIVE_SYSTEMC
	cynw_p2p< sc_dt::sc_uint<8> >::in i_grey;
	cynw_p2p< sc_dt::sc_uint<8> >::out o_result;
#else
	sc_fifo_in< sc_dt::sc_uint<8> > i_grey;
	sc_fifo_out< sc_dt::sc_uint<8> > o_result;
#endif

	SC_HAS_PROCESS( ImageGradient );
	ImageGradient( sc_module_name n );
private:
	void do_filter();
	sc_uint<10> x,y,x_m,y_m;
	sc_uint<3> median_count;
	sc_uint<12> sum;
	sc_uint<8> mean;
	sc_uint<8> median[9];
	sc_uint<8> median_bitmap[512][512];
};
#endif
