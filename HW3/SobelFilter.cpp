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

	for (int y = 0; y<512; y++) {
		int median_count = 0;
		for (int x = 0; x<512; x++) {
			if(x == 0){
				for(int z = 0;z < 9; z++){
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
					median[z] = (rgb.range(7,0) + rgb.range(15,8) + rgb.range(23, 16))/3;
                    cout << y << "," << x << "=" << median[z] << endl;
				}
        for(int i = 8; i > 0; i--){
          for(int j = 0; j < i-1; j++){
            if(median[j] > median[j+1]){
              int tmp = median[j];
              median[j] = median[j+1];
              median[j+1] = tmp;
            }
          }
        }
				int temp = median[4];
				median_bitmap[y][x] = temp;
			} else {
				for(int z = 0;z < 3; z++){
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
					median[median_count % 9] = (rgb.range(7,0) + rgb.range(15,8) + rgb.range(23, 16))/3;
                              cout << y << "," << x << "=" << median[z] << endl;
					median_count++;
				}
        for(int i = 8; i > 0; i--){
          for(int j = 0; j < i-1; j++){
            if(median[j] > median[j+1]){
              int tmp = median[j];
              median[j] = median[j+1];
              median[j+1] = tmp;
            }
          }
        }
				int temp = median[4];
				median_bitmap[y][x] = temp;
        cout << y << "," << x << "=" << temp << endl;
			}
		}
	}		

	for (int y = 0; y < 512; y++) {
		for (int x = 0; x < 512; x++) {
			int sum = 0;
			for (int v = -1; v <= 1; v++) {
				for (int u = -1; u <= 1; u++) {
					int yy = y + v;
					int xx = x + u;
					if (yy >= 0 && yy < 512 && xx >= 0 && xx < 512) {
						sum += median_bitmap[yy][xx] * mask[v+1][u+1];
					}   
				}
			}
			mean_bitmap[y][x] = (sum / 10);

			#ifndef NATIVE_SYSTEMC
			{
				HLS_DEFINE_PROTOCOL("output");
				o_result.put(mean_bitmap[y][x]);
				wait();
			}
			#else
			o_result.write(mean_bitmap[y][x]);
			#endif
		}
	}

}
