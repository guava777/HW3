#include "ImageGradient.h"

#ifndef NATIVE_SYSTEMC
#include "stratus_hls.h"
#endif

ImageGradient::ImageGradient( sc_module_name n ): sc_module( n )
{
	SC_THREAD( do_filter );
	sensitive << i_clk.pos();
	dont_initialize();
	reset_signal_is(i_rst, false);

#ifndef NATIVE_SYSTEMC
	i_grey.clk_rst(i_clk, i_rst);
  	o_result.clk_rst(i_clk, i_rst);
#endif
}

// sobel mask
const int mask[3][3] = {{1, 1, 1}, {1, 2, 1}, {1, 1, 1}};

void ImageGradient::do_filter() {
	{
#ifndef NATIVE_SYSTEMC
		HLS_DEFINE_PROTOCOL("main_reset");
		i_grey.reset();
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
				y = y + 1;
				x=0;
				median_count=0;
        			wait();
			}else{
				if(x == 0){
					for(unsigned int i = 0; i < 9; i++){
						#ifndef NATIVE_SYSTEMC
						median[i] = i_grey.get();
						#else
						median[i] = i_grey.read();
						#endif
						wait();
					}
				} else {
					if(median_count == 0){
						for(unsigned int i = 0; i < 3; i++){
							#ifndef NATIVE_SYSTEMC
							median[i] = i_grey.get();
							#else
							median[i] = i_grey.read();
							#endif
						}	
						median_count = 1;                     
						wait();
					} else if (median_count == 1){
						for(unsigned int i = 3; i < 6; i++){
							#ifndef NATIVE_SYSTEMC
							median[i] = i_grey.get();
							#else
							median[i] = i_grey.read();
							#endif
						}	
						median_count = 2;
						wait();					
					} else if(median_count == 2){
						for(unsigned int i = 6; i < 9; i++){
							#ifndef NATIVE_SYSTEMC
							median[i] = i_grey.get();
							#else
							median[i] = i_grey.read();
							#endif
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
		if(y>20) {
			sum = 0;
			if(x_m==512){
				y_m++;
				x_m=0;
        			wait();
			}
			for (sc_uint<4> v = 0; v < 3; v++) {
				for (sc_uint<4> u = 0; u < 3; u++) {
					int yy = y_m + v - 1;
					int xx = x_m + u - 1;
					if (yy >= 0 && yy < 512 && xx >= 0 && xx < 512) {
						sum += median_bitmap[yy][xx] * mask[v][u];
                   				wait();
					}   
				}
			}
			mean = (sum / 10);
			#ifndef NATIVE_SYSTEMC
			o_result.put(mean);
			#else
			o_result.write(mean);
			#endif
			x_m++;
      			wait();
		}
	}
}
