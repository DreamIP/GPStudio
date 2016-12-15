/*! 
 *  \file      histogram.v
 *  \brief     Software configurable hardware Histogram core
 *  \author    Luca Maggiani
 *
 *  \version   1.0 (Mar13) - first release, it finally works! static 8x8bins
 *  \version   1.1 (24Apr13) - Adapted to Camera_OneFrame v3.0 Architecture 
 *  \version   3.0 (Jan14) - Major revision, completely rewritten,
 *                           Configurable cell width, height, bins
 *                           Reduced memory footprint by a factor of 2
 *                           Store phase revision (bug during phase switch)
 *  \version   3.0.1 (14Jan14) - active_cell signal for odd cell width 
 *  \version   3.0.2 (15Jan14) - Store phase revision with count_store_extended
 *	\version   3.1   (18Feb15) - flow valid interfaces and generic bus size added.
 *
 *  \date      Feb 2015
 *  \pre       ram_cell, ram_subcell and ram_module instances
 *  \param[in]  index_fv, index_dv, index_data, inc_fv, inc_dv, inc_data
 *  \param[out] hist_fv, hist_dv, hist_data
 *
 *  \see       ram_cell
 *  \see       ram_subcell
 *  \see       ram_module
 *
 *  \todo input flows synchronisation! (dv_int and fv_int gen)
 */
 

 /*!   \f$
		\begin{tabular} {|c|c|c|c|}
		\hline
		\textbf{Offset} & \textbf{Name} & \textbf{R/W} & \textbf{Descriptions of Bits} \\
		\hline
		0x00 & Status Control & RW & N\_BIN\_EXP\big[5:2\big], MODE\big[1\big], ONOFF\big[0\big]\\
		\hline
		0x01 & CellControl & RW & cellheight\big[25:16\big], cellwidth\big[9:0\big]\\
		\hline
		0x02 & ImageControl & RW & cellinrow\big[9:0\big] \\
		\hline
		0x03 & StoreCycles & RW  & storecycles\big[15:0\big]\\
		\hline		
	\end{tabular}
	\f$
 */
  
module histogram(

	clk_proc,
	reset_n,

	index_fv,
	index_dv,
	index_data,

	inc_fv,
	inc_dv,
	inc_data,

	hist_fv,
	hist_dv,
	hist_data,
	
	addr_rel_i,
	wr_i,
	datawr_i,
	rd_i,
	
	datard_o
);

/* Flows size */
parameter INC_SIZE = 16;
parameter INDEX_SIZE = 8;
parameter HIST_SIZE = 16;

/* Clock param */
parameter CLK_PROC_FREQ = 50000000;

parameter HISTMEM_WORD = 2048;
localparam HISTMEM_ADDR_WIDTH = $clog2(HISTMEM_WORD);

input clk_proc;
input reset_n; 

input inc_fv;
input inc_dv;
input [(INC_SIZE-1):0] inc_data;

input index_fv;
input index_dv;
input [(INDEX_SIZE-1):0] index_data;

output hist_fv;
output hist_dv;
output [(HIST_SIZE-1):0] hist_data;

//% \{
//% Avalon-MM interface
//%
input  [1:0] addr_rel_i; 
input  wr_i;
input  rd_i;
input  [31:0] datawr_i;
output [31:0] datard_o;
//% \}

/*! \{ Status and Control Register */
reg [31:0] scr, scr_new;
/*! \} */

/*! \{  Avalon-MM Parameters */
reg [31:0] cellcontrol, cellcontrol_new;
reg [31:0] imagecontrol, imagecontrol_new;
reg [31:0] storecycles, storecycles_new;
/*! \} */

/*! \{ Internal registers */
reg [31:0] readdata, readdata_new;
/*  \} */

/*! \{ Internal wires */
wire [9:0] cellheight;
wire [9:0] cellwidth;
wire [9:0] cellinrow;
wire onoff;
wire mode;
wire [3:0] n_bin_exp;

wire dv_int;
wire fv_int;

wire storecompleted_cellA;
wire storecompleted_cellB;

wire data_valid_cellA;
wire data_valid_cellB;

wire [HIST_SIZE-1:0] data_out_cellA;
wire [HIST_SIZE-1:0] data_out_cellB;

wire data_valid_out_cellA;
wire data_valid_out_cellB;

wire loadcompleted_cellA;
wire loadcompleted_cellB;
/*  \} */


/*! \{ Output flow valid fsm */
reg [1:0] hist_fv_fsm, hist_fv_fsm_new;
reg hist_fv_reg;
reg fv_int_reg;

localparam WAIT_A_LOADCOMPL  = 2'd0;
localparam WAIT_A_STORECOMPL = 2'd1;
localparam WAIT_B_LOADCOMPL  = 2'd2;
localparam WAIT_B_STORECOMPL = 2'd3;
/*  \} */

/*! \{ Output flow valid fsm */
reg [1:0] histogram_fsm, histogram_fsm_new;

localparam START_STATE  = 2'd0;
localparam WAIT_A_STATE = 2'd1;
localparam WAIT_B_STATE = 2'd2;
/*  \} */

/*! \{ Output flow */
reg dv_out_s;
reg [HIST_SIZE-1:0] data_out;
/*  \} */


/* ################################### */
/* Input flows synchronisation */

assign dv_int = inc_dv & index_dv & onoff;
assign fv_int = inc_fv & index_fv & onoff;



/* ################################### */	
/* Input data management */

always@(posedge clk_proc or negedge reset_n)
	if (reset_n == 0)
		histogram_fsm <= START_STATE;
	else
		histogram_fsm <= histogram_fsm_new;
		
always@(*)
	case (histogram_fsm)
	START_STATE:
		if (onoff == 1'b0)
			histogram_fsm_new = START_STATE;
		else
			histogram_fsm_new = WAIT_A_STATE;
	WAIT_A_STATE:
		if (onoff == 1'b0)
			histogram_fsm_new = START_STATE;
		else
			if (loadcompleted_cellA)
				histogram_fsm_new = WAIT_B_STATE;
			else
				histogram_fsm_new = WAIT_A_STATE;
	WAIT_B_STATE:
		if (onoff == 1'b0)
			histogram_fsm_new = START_STATE;
		else
			if (loadcompleted_cellB)
				histogram_fsm_new = WAIT_A_STATE;
			else
				histogram_fsm_new = WAIT_B_STATE;
	default:
		histogram_fsm_new = START_STATE;
	endcase	
	
	
always@(posedge clk_proc or negedge reset_n)
	if (reset_n == 1'b0)
		dv_out_s <= 1'b0;
	else
		dv_out_s <= (data_valid_out_cellA | data_valid_out_cellB);

always@(posedge clk_proc or negedge reset_n)
	if (reset_n == 1'b0)
		data_out <= 16'd0;
	else
		if (histogram_fsm == WAIT_A_STATE)
			data_out <= data_out_cellB;
		else if (histogram_fsm == WAIT_B_STATE)
			data_out <= data_out_cellA;
		else
			data_out <= 16'b0;

//% \brief Data valid to cells
//% \todo  Right now it works with a synchronous static exception, 
//%        I hope to find a cleaner solution (more elegant at least)
//% \{
assign data_valid_cellA = dv_int & ((histogram_fsm == WAIT_A_STATE)| loadcompleted_cellB) &~loadcompleted_cellA;
assign data_valid_cellB = dv_int & ((histogram_fsm == WAIT_B_STATE)| loadcompleted_cellA) &~loadcompleted_cellB;
//% \}


/* ################################### */
/*  hist_fv management (output flow) */

always@(posedge clk_proc or negedge reset_n)
	if (reset_n == 0)
		hist_fv_fsm <= 0;
	else 
		hist_fv_fsm <= hist_fv_fsm_new;

always@(*)
	case (hist_fv_fsm)
	WAIT_A_LOADCOMPL:
		if (onoff == 0)
			hist_fv_fsm_new = WAIT_A_LOADCOMPL;
		else
			if (loadcompleted_cellA)
				hist_fv_fsm_new = WAIT_A_STORECOMPL;
			else
				hist_fv_fsm_new = WAIT_A_LOADCOMPL;
	WAIT_A_STORECOMPL:
		if (onoff == 0)
			hist_fv_fsm_new = WAIT_A_LOADCOMPL;
		else
			if (storecompleted_cellA)
				hist_fv_fsm_new = WAIT_B_LOADCOMPL;
			else
				hist_fv_fsm_new = WAIT_A_STORECOMPL;
	WAIT_B_LOADCOMPL:
		if (onoff == 0)
			hist_fv_fsm_new = WAIT_A_LOADCOMPL;
		else
			if (loadcompleted_cellB)
				hist_fv_fsm_new = WAIT_B_STORECOMPL;
			else
				hist_fv_fsm_new = WAIT_B_LOADCOMPL;
		
	WAIT_B_STORECOMPL:
		if (onoff == 0)
			hist_fv_fsm_new = WAIT_A_LOADCOMPL;
		else
			if (storecompleted_cellB)
				hist_fv_fsm_new = WAIT_A_LOADCOMPL;
			else
				hist_fv_fsm_new = WAIT_B_STORECOMPL;
	default:
		hist_fv_fsm_new = WAIT_A_LOADCOMPL;
	endcase	

always@(posedge clk_proc or negedge reset_n)
	if (reset_n == 0)
		fv_int_reg <= 0;
	else 
		fv_int_reg <= fv_int;
		
always@(posedge clk_proc or negedge reset_n)
	if (reset_n == 0)
		hist_fv_reg <= 0;
	else 
		hist_fv_reg <= fv_int_reg || (hist_fv_fsm == WAIT_A_STORECOMPL) || (hist_fv_fsm == WAIT_B_STORECOMPL);		
		
/* ###################	*/	

//% \brief output flow assignmenent
//% \{		
assign hist_fv = hist_fv_reg;
assign hist_dv = dv_out_s;
assign hist_data = data_out;
//% \}

//% \brief ram_cellA instantiation
//% \{
ram_cell 
	#(  .HISTMEM_WORD(HISTMEM_WORD),
		.HISTOGRAM_WIDTH(HIST_SIZE)) ram_cellA(
	.clk(clk_proc),
	.reset_n(reset_n),
		
	.data_in(inc_data),
	.dir_max(index_data),
	.data_valid_in(data_valid_cellA),
	
	.data_out(data_out_cellA),
	.data_valid_out(data_valid_out_cellA),
	
	.cellwidth(cellwidth),
	.cellinrow(cellinrow),
	.cellheight(cellheight),
	.storecycles(storecycles[15:0]),
	
	.onoff(onoff),
	.mode(mode),
	.n_bin_exp(n_bin_exp),
	
	.newcell_signal(),
	.newline_signal(),
	.loadcompleted(loadcompleted_cellA),
	.storecompleted(storecompleted_cellA),
	
	.count_store_int(),
	.address_a_int()
);
//% \}

//% \brief ram_cellA instantiation
//% \{	
ram_cell 
	#(  .HISTMEM_WORD(HISTMEM_WORD),
		.HISTOGRAM_WIDTH(HIST_SIZE)) ram_cellB(
	.clk(clk_proc),
	.reset_n(reset_n),
		
	.data_in(inc_data),
	.dir_max(index_data),
	.data_valid_in(data_valid_cellB),
	
	.data_out(data_out_cellB),
	.data_valid_out(data_valid_out_cellB),
	
	.cellwidth(cellwidth),
	.cellinrow(cellinrow),
	.cellheight(cellheight),
	.storecycles(storecycles[15:0]),
	
	.onoff(onoff),
	.mode(mode),
	.n_bin_exp(n_bin_exp),
	
	.newcell_signal(),
	.newline_signal(),
	.loadcompleted(loadcompleted_cellB),
	.storecompleted(storecompleted_cellB),
	
	.count_store_int(),
	.address_a_int()
);
//% \}


/* -------------- Avalon-MM Interface -------------- 

	SCR				-	R/W
	CELLCONTROL		-	R/W
	IMAGECONTROL	-	R/W
	STORECYCLES		-	R/W

*/

assign cellheight = cellcontrol[25:16];
assign cellwidth = cellcontrol[9:0];
assign cellinrow = imagecontrol[9:0];
assign onoff = scr[0];
assign mode  = scr[1];
assign n_bin_exp = scr[5:2];

//%
//% Avalon-MM registers write
//% \code
always @ (*)
	if (wr_i)
		case(addr_rel_i)
		2'd0: 
			begin 
				scr_new = datawr_i;
				cellcontrol_new = cellcontrol;
				imagecontrol_new = imagecontrol;
				storecycles_new = storecycles;
			end
		2'd1: 
			begin 
				scr_new = scr;
				cellcontrol_new = datawr_i;
				imagecontrol_new = imagecontrol;
				storecycles_new = storecycles;
			end
		2'd2:
			begin 
				scr_new = scr;
				cellcontrol_new = cellcontrol;
				imagecontrol_new = datawr_i;
				storecycles_new = storecycles;
			end
		2'd3:
			begin 
				scr_new = scr;
				cellcontrol_new = cellcontrol;
				imagecontrol_new = imagecontrol;
				storecycles_new = datawr_i;
			end
			
		default:
			begin 
				scr_new = scr;
				cellcontrol_new = cellcontrol;
				imagecontrol_new = imagecontrol;
				storecycles_new = storecycles;
			end			
		endcase
	else /* write disabled */
		begin 
			scr_new = scr;
			cellcontrol_new = cellcontrol;
			imagecontrol_new = imagecontrol;
			storecycles_new = storecycles;
		end				
//% \endcode
	
	
/*! Read phase 
*/	
always @ (*)
	if (rd_i)
		case(addr_rel_i)
			2'd0:		readdata_new = scr;
			2'd1:		readdata_new = cellcontrol;
			2'd2:		readdata_new = imagecontrol;
			2'd3:		readdata_new = storecycles;			
			default:	readdata_new = 32'd0;
		endcase
	else 
		readdata_new = readdata;

/* Internal register update */
always @ (posedge clk_proc or negedge reset_n)
	if (reset_n == 1'b0)
		begin
			scr					<= 32'd0;	
			cellcontrol[31:16]	<= 16'd7;
			cellcontrol[15:0]  	<= 16'd7;
			imagecontrol		<= 32'd39;
			storecycles			<= 32'd640;
			readdata			<= 32'd0;
		end
	else 
		begin
			scr 			<= scr_new;
			cellcontrol 	<= cellcontrol_new;
			imagecontrol 	<= imagecontrol_new;
			storecycles 	<= storecycles_new;
			readdata		<= readdata_new;
		end
		
assign datard_o = readdata;
	
endmodule
