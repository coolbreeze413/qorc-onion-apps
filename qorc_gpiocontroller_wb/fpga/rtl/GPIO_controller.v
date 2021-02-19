
`timescale 1ns / 10ps

module GPIO_controller ( 
                // AHB-To_FPGA Bridge I/F
                WBs_ADR_i,
                WBs_CYC_i,
                WBs_BYTE_STB_i,
                WBs_WE_i,
                WBs_STB_i,
                WBs_DAT_i,
                WBs_CLK_i,
                WBs_RST_i,
                WBs_DAT_o,
                WBs_ACK_o,
						
				// GPIO signals
				GPIO_io
);


//------Port Parameters----------------

    //  Address offsets shown below are byte offsets, so the 2 LSB's (on the right) should be 0's.

// This parameter can/should be over-ridden when this module is instantiated,
//  since it must be unique across all other modules in the FPGA.
parameter   MODULE_OFFSET       = 17'h0_1000;

// This is the value that is returned when a non-implemented register is read.
parameter   DEFAULT_REG_VALUE   = 32'hFAB_DEF_AC;


//------Port Signals-------------------

// AHB-To_FPGA Bridge I/F
input   [16:0]  WBs_ADR_i           ;  // Address Bus                to   FPGA
input           WBs_CYC_i           ;  // Cycle Chip Select          to   FPGA 
input   [3:0]   WBs_BYTE_STB_i      ;  // Byte Select                to   FPGA
input           WBs_WE_i            ;  // Write Enable               to   FPGA
input           WBs_STB_i           ;  // Strobe Signal              to   FPGA
input   [31:0]  WBs_DAT_i           ;  // Write Data Bus             to   FPGA
input           WBs_CLK_i           ;  // FPGA Clock               from FPGA
input           WBs_RST_i           ;  // FPGA Reset               to   FPGA
output  [31:0]  WBs_DAT_o           ;  // Read Data Bus              from FPGA
output          WBs_ACK_o           ;  // Transfer Cycle Acknowledge from FPGA


// GPIO
inout   [31:0]  GPIO_io             ;


// FPGA Global Signals
wire            WBs_CLK_i           ;  // Wishbone FPGA Clock
wire            WBs_RST_i           ;  // Wishbone FPGA Reset

// Wishbone Bus Signals
wire    [16:0]  WBs_ADR_i           ;  // Wishbone Address Bus
wire            WBs_CYC_i           ;  // Wishbone Client Cycle  Strobe (i.e. Chip Select) 
wire    [3:0]   WBs_BYTE_STB_i      ;  // Wishbone Byte   Enables
wire            WBs_WE_i            ;  // Wishbone Write  Enable Strobe
wire            WBs_STB_i           ;  // Wishbone Transfer      Strobe
wire    [31:0]  WBs_DAT_i           ;  // Wishbone Write  Data Bus
 
reg     [31:0]  WBs_DAT_o           ;  // Wishbone Read   Data Bus


reg             WBs_ACK_o           ;  // Wishbone Client Acknowledge


// GPIO
wire    [31:0]  GPIO_io             ;


//------Internal Parameters---------------

// Allow for up to 256 registers in this module
localparam  ADDRWIDTH   =  8;

// register offsets.
    //  Address offsets shown below are byte offsets, so the 2 LSB's (on the right) should be 0's.
localparam  REG_ADDR_GPIO_IN    =  8'h00        ; 
localparam  REG_ADDR_GPIO_OUT   =  8'h04        ; 
localparam  REG_ADDR_GPIO_OE    =  8'h08        ; 


//------Internal Signals---------------
wire    [31:0]  GPIO_in         ;
reg     [31:0]  GPIO_out        ;
reg     [31:0]  GPIO_OE         ;

wire            module_decode   ;
wire            REG_WE_GPIO_out ;
wire            REG_WE_GPIO_OE  ;
wire            WBs_ACK_o_nxt   ;


assign module_decode = (WBs_ADR_i[16:ADDRWIDTH] == MODULE_OFFSET[16:ADDRWIDTH]);

assign REG_WE_GPIO_out = ( WBs_ADR_i[ADDRWIDTH-1:2] == REG_ADDR_GPIO_OUT[ADDRWIDTH-1:2] ) && module_decode && WBs_CYC_i && WBs_STB_i && WBs_WE_i && (~WBs_ACK_o);
assign REG_WE_GPIO_OE  = ( WBs_ADR_i[ADDRWIDTH-1:2] == REG_ADDR_GPIO_OE[ADDRWIDTH-1:2]  ) && module_decode && WBs_CYC_i && WBs_STB_i && WBs_WE_i && (~WBs_ACK_o);

// Define the Acknowledge back to the host for registers
assign WBs_ACK_o_nxt  =  module_decode && (WBs_CYC_i) && WBs_STB_i && (~WBs_ACK_o);


// write logic for the registers
always @( posedge WBs_CLK_i or posedge WBs_RST_i)
begin
    if (WBs_RST_i)
    begin
		GPIO_out          <= 31'b0    ;
		GPIO_OE           <= 31'b0    ;

		WBs_ACK_o    	  <= 1'b0    ;
    end  
    else
    begin
			
        // GPIO Out Register 
        if (REG_WE_GPIO_out) begin
            if (WBs_BYTE_STB_i[0])
                GPIO_out[7:0]     <= WBs_DAT_i[7:0]   ;
            if (WBs_BYTE_STB_i[1])
                GPIO_out[15:8]    <= WBs_DAT_i[15:8]  ;
            if (WBs_BYTE_STB_i[2])
                GPIO_out[23:16]   <= WBs_DAT_i[23:16] ;
            if (WBs_BYTE_STB_i[3])
                GPIO_out[31:24]   <= WBs_DAT_i[31:24] ;
		end
			
        // GPIO OE Register 
        if (REG_WE_GPIO_OE) begin
            if (WBs_BYTE_STB_i[0])
			    GPIO_OE[7:0]        <= WBs_DAT_i[7:0]   ;
            if (WBs_BYTE_STB_i[1])
			    GPIO_OE[15:8]       <= WBs_DAT_i[15:8]  ;
            if (WBs_BYTE_STB_i[2])
			    GPIO_OE[23:16]      <= WBs_DAT_i[23:16] ;
            if (WBs_BYTE_STB_i[3])
			    GPIO_OE[31:24]      <= WBs_DAT_i[31:24] ;
		end

        WBs_ACK_o                   <=  WBs_ACK_o_nxt  ;
    end  
end


// read logic for the registers
always @(*)
 begin
    case(WBs_ADR_i[ADDRWIDTH-1:2])
        REG_ADDR_GPIO_IN    [ADDRWIDTH-1:2]  : WBs_DAT_o <= GPIO_in             ;
        REG_ADDR_GPIO_OUT   [ADDRWIDTH-1:2]  : WBs_DAT_o <= GPIO_out            ;
        REG_ADDR_GPIO_OE    [ADDRWIDTH-1:2]  : WBs_DAT_o <= GPIO_OE             ;
	    default                              : WBs_DAT_o <= DEFAULT_REG_VALUE   ;
	endcase
end


// GPIO pins
assign GPIO_in = GPIO_io;

genvar i;
generate
    for (i=0; i<32; i= i+1) begin
        assign GPIO_io[i] = GPIO_OE[i] ? GPIO_out[i] : 1'bz;
    end
endgenerate


endmodule

