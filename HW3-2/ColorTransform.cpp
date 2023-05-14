#ifndef NATIVE_SYSTEMC
#include "stratus_hls.h"
#endif

#include "ColorTransform.h"

ColorTransform::ColorTransform( sc_module_name n ): sc_module( n )
{
	SC_THREAD( transform_rgb_to_grey );
	sensitive << i_clk.pos();
	dont_initialize();
	reset_signal_is(i_rst, false);

#ifndef NATIVE_SYSTEMC
	i_rgb.clk_rst(i_clk, i_rst);
	o_grey.clk_rst(i_clk, i_rst);
#endif
}

void ColorTransform::transform_rgb_to_grey() {
	{
#ifndef NATIVE_SYSTEMC
		HLS_DEFINE_PROTOCOL("main_reset");
		i_rgb.reset();
		o_grey.reset();
#endif
	}
	while (true) {
 sc_dt::sc_uint<24> rgb;
#ifndef NATIVE_SYSTEMC
{
  HLS_DEFINE_PROTOCOL("input");
  rgb = i_rgb.get();
  cout << rgb << endl;
  wait();
}   
#else
		sc_dt::sc_uint<24> rgb = i_rgb.read();
#endif
		unsigned char grey = (rgb.range(7,0) + rgb.range(15,8) + rgb.range(23, 16))/3;
   cout << grey << endl;
   wait();
#ifndef NATIVE_SYSTEMC
{
HLS_DEFINE_PROTOCOL("output");
		o_grey.put(grey);
   wait();
   }
#else
		o_grey.write(grey);
#endif
	}
}