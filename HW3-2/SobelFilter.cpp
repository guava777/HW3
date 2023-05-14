#include <cmath>
#ifndef NATIVE_SYSTEMC
#include "stratus_hls.h"
#endif

#include "SobelFilter.h"

SobelFilter::SobelFilter( sc_module_name n ): sc_module( n )
{
	SC_THREAD( do_filter );
	sensitive << i_clk.pos();
	dont_initialize();
	reset_signal_is(i_rst, false);
        
#ifndef NATIVE_SYSTEMC
	i_rgb.clk_rst(i_clk, i_rst);
  	o_result.clk_rst(i_clk, i_rst);
#endif
}

SobelFilter::~SobelFilter() {}

const int mask[3][3] = {{1, 1, 1}, {1, 2, 1}, {1, 1, 1}};

void SobelFilter::do_filter() {
	{
#ifndef NATIVE_SYSTEMC
		HLS_DEFINE_PROTOCOL("main_reset");
		i_rgb.reset();
		o_result.reset();
#endif
wait();
	}
	x=0;
	y=0;
	x_m=0;
	y_m=0;
	median_count = 0;
	while(true){
		if(y<512){
			if(x==512){
				y++;
				x=0;
				median_count=0;
        wait();
			}else{
			if(x == 0){
				for(unsigned int i = 0; i < 9; i++){
						sc_dt::sc_uint<24> rgb;
#ifndef NATIVE_SYSTEMC
						{
							HLS_DEFINE_PROTOCOL("input");
							rgb = i_rgb.get();
							wait();
						}
#else
						rgb = i_rgb.read();
#endif
						median[i] = (rgb.range(7,0) + rgb.range(15,8) + rgb.range(23, 16))/3;
          wait();
				}
			} else {
				if(median_count == 0){
					for(unsigned int i = 0; i < 3; i++){
						sc_dt::sc_uint<24> rgb;
#ifndef NATIVE_SYSTEMC
						{
							HLS_DEFINE_PROTOCOL("input");
							rgb = i_rgb.get();
							wait();
						}
#else
						rgb = i_rgb.read();
#endif
						median[i] = (rgb.range(7,0) + rgb.range(15,8) + rgb.range(23, 16))/3;
					}	
          median_count = 1;                              
				} else if (median_count == 1){
					for(unsigned int i = 3; i < 6; i++){
						sc_dt::sc_uint<24> rgb;
#ifndef NATIVE_SYSTEMC
						{
							HLS_DEFINE_PROTOCOL("input");
							rgb = i_rgb.get();
							wait();
						}
#else
						rgb = i_rgb.read();
#endif
						median[i] = (rgb.range(7,0) + rgb.range(15,8) + rgb.range(23, 16))/3;
					}	
					median_count = 2;
				} else if(median_count == 2){
					for(unsigned int i = 6; i < 9; i++){
						sc_dt::sc_uint<24> rgb;
#ifndef NATIVE_SYSTEMC
						{
							HLS_DEFINE_PROTOCOL("input");
							rgb = i_rgb.get();
							wait();
						}
#else
						rgb = i_rgb.read();
#endif
						median[i] = (rgb.range(7,0) + rgb.range(15,8) + rgb.range(23, 16))/3;
					}	
					median_count = 0;	
          wait();
				}
			}          
      for(sc_uint<4> i = 8; i > 0; i--){
        for(sc_uint<4> j = 0; j < i-1; j++){
          if(median[j] > median[j+1]){
            sc_uint<8> tmp = median[j];
            median[j] = median[j+1];
            median[j+1] = tmp;
          }
        }
      }  
			median_bitmap[y][x] = median[4];
			x++;
      wait();
      }
		}
   //mean filter
		if(y>2) {
			sum = 0;
			if(x_m==512){
				y_m++;
				x_m=0;
			}
			for (sc_uint<4> v = 0; v < 3; v++) {
				for (sc_uint<4> u = 0; u < 3; u++) {
					int yy = y_m + v - 1;
					int xx = x_m + u - 1;
					if (yy >= 0 && yy < 512 && xx >= 0 && xx < 512) {
						sum += median_bitmap[yy][xx] * mask[v][u];                  
					}   
				}
			}
			mean = (sum / 10);
#ifndef NATIVE_SYSTEMC
{
  HLS_DEFINE_PROTOCOL("output");
  o_result.put(mean);
  wait();
}
#else
			o_result.write(mean);
#endif
			x_m++;
      
		}
	}
}
