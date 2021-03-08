
`timescale 1ns / 10ps

module AL4S3B_FPGA_ONION_GPIOCTRL ( 
    
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


// MODULE Parameters =====================================================================

// This is the value that is returned when a non-implemented register is read.
parameter   DEFAULT_REG_VALUE   = 32'hDEF_FAB_AC;


// MODULE Internal Parameters ============================================================
// Allow for up to 256 registers in this module
localparam  ADDRWIDTH   =  10;

// register offsets.
localparam  REG_ADDR_GPIO_IN    =  10'h000        ;
localparam  REG_ADDR_GPIO_OUT   =  10'h004        ;
localparam  REG_ADDR_GPIO_OE    =  10'h008        ;


// MODULE PORT Declarations and Data Types ===============================================

// AHB-To_FPGA Bridge I/F
input       wire    [16:0]      WBs_ADR_i           ;  // Address Bus                   to   FPGA
input       wire                WBs_CYC_i           ;  // Cycle Chip Select             to   FPGA 
input       wire    [3:0]       WBs_BYTE_STB_i      ;  // Byte Select                   to   FPGA
input       wire                WBs_WE_i            ;  // Write Enable                  to   FPGA
input       wire                WBs_STB_i           ;  // Strobe Signal                 to   FPGA
input       wire    [31:0]      WBs_DAT_i           ;  // Write Data Bus                to   FPGA
input       wire                WBs_CLK_i           ;  // FPGA Clock                    from FPGA
input       wire                WBs_RST_i           ;  // FPGA Reset                    to FPGA
output      wire    [31:0]      WBs_DAT_o           ;  // Read Data Bus                 from FPGA
output      wire                WBs_ACK_o           ;  // Transfer Cycle Acknowledge    from FPGA

// GPIO
inout       wire    [31:0]      GPIO_io             ;


// MODULE INTERNAL Signals ===============================================================

wire    [31:0]  GPIO_in         ;
reg     [31:0]  GPIO_out        ;
reg     [31:0]  GPIO_OE         ;

wire            REG_WE_GPIO_out ;
wire            REG_WE_GPIO_OE  ;
wire            WBs_ACK_o_nxt   ;


// MODULE LOGIC ==========================================================================

// define WRITE ENABLE logic:
assign REG_WE_GPIO_out = ( WBs_ADR_i[ADDRWIDTH-1:2] == REG_ADDR_GPIO_OUT[ADDRWIDTH-1:2] ) && 
                                                       WBs_CYC_i && 
                                                       WBs_STB_i && 
                                                       WBs_WE_i && 
                                                       (~WBs_ACK_o);
assign REG_WE_GPIO_OE  = ( WBs_ADR_i[ADDRWIDTH-1:2] == REG_ADDR_GPIO_OE[ADDRWIDTH-1:2] ) && 
                                                       WBs_CYC_i && 
                                                       WBs_STB_i && 
                                                       WBs_WE_i && 
                                                       (~WBs_ACK_o);

// define the ACK back to the host for registers
assign WBs_ACK_o_nxt  =  (WBs_CYC_i) && 
                         WBs_STB_i && 
                         (~WBs_ACK_o);


// define WRITE logic for the registers
always @( posedge WBs_CLK_i or posedge WBs_RST_i)
begin
    if (WBs_RST_i)
    begin
        GPIO_out          <= 32'b0      ;
        GPIO_OE           <= 32'b0      ;
        WBs_ACK_o    	  <= 1'b0       ;
    end  
    else
    begin

        // GPIO Out Register
        if (REG_WE_GPIO_out)
        begin
            if (WBs_BYTE_STB_i[0])
                GPIO_out[7:0]       <= WBs_DAT_i[7:0]   ;
            if (WBs_BYTE_STB_i[1])
                GPIO_out[15:8]      <= WBs_DAT_i[15:8]  ;
            if (WBs_BYTE_STB_i[2])
                GPIO_out[23:16]     <= WBs_DAT_i[23:16] ;
            if (WBs_BYTE_STB_i[3])
                GPIO_out[31:24]     <= WBs_DAT_i[31:24] ;
        end
        
        // GPIO OE Register
        if (REG_WE_GPIO_OE)
        begin
            if (WBs_BYTE_STB_i[0])
                GPIO_OE[7:0]        <= WBs_DAT_i[7:0]   ;
            if (WBs_BYTE_STB_i[1])
                GPIO_OE[15:8]       <= WBs_DAT_i[15:8]  ;
            if (WBs_BYTE_STB_i[2])
                GPIO_OE[23:16]      <= WBs_DAT_i[23:16] ;
            if (WBs_BYTE_STB_i[3])
                GPIO_OE[31:24]      <= WBs_DAT_i[31:24] ;
        end

        WBs_ACK_o                   <=  WBs_ACK_o_nxt   ;
    end  
end


// define READ logic for the registers
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
