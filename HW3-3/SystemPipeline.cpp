#include "SystemPipeline.h"
SystemPipeline::SystemPipeline( sc_module_name n, string input_bmp, string output_bmp ): sc_module( n ), 
	tb("tb"), color_transform("color_transform"), image_gradient("image_gradient"), 
	clk("clk", CLOCK_PERIOD, SC_NS), rst("rst"), _output_bmp(output_bmp)
{
	tb.i_clk(clk);
	tb.o_rst(rst);
	color_transform.i_clk(clk);
	color_transform.i_rst(rst);
	image_gradient.i_clk(clk);
	image_gradient.i_rst(rst);
	tb.o_rgb(rgb);
	color_transform.i_rgb(rgb);
	color_transform.o_grey(grey);
	image_gradient.i_grey(grey);
	image_gradient.o_result(result);
	tb.i_result(result);
	tb.read_bmp(input_bmp);
}

SystemPipeline::~SystemPipeline() {
  	tb.write_bmp(_output_bmp);
}
