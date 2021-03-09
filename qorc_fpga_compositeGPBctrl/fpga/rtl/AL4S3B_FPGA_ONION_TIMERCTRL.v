
`timescale 1ns / 10ps

// each TIMER output will have its own register for settings.
// TIMER_x_CONFIG
//      TIMER_x_PERIOD          - [30:0]    (how many clk cycles)
//      TIMER_x_EN              - [31]      1 to enable, 0 to disable

module AL4S3B_FPGA_ONION_TIMERCTRL (
    
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

    // TIMER clk
    TIMER_clk,
            
    // IO signals (debug)
    TIMER_dbg_o,

    // TIMER signals
    TIMER_o
);


// MODULE Parameters =====================================================================

// This is the value that is returned when a non-implemented register is read.
parameter   DEFAULT_REG_VALUE                     = 32'hDEF_FAB_AC;   // no such register


// MODULE Internal Parameters ============================================================
// Allow for up to 256 registers in this module
localparam  ADDRWIDTH                   =  10;

// register offsets.
localparam  REG_ADDR_TIMER_0_CONFIG   =  10'h000        ;

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

// TIMER clock
input       wire                TIMER_clk           ;

// IO outputs
output      wire    [31:0]      TIMER_dbg_o                ;

// Timer outputs
output      wire    [3:0]       TIMER_o             ;


// MODULE INTERNAL Signals ===============================================================

reg     [31:0]  TIMER_0_CONFIG                      ;


wire            REG_WE_TIMER_0_CONFIG               ;
wire            WBs_ACK_o_nxt                       ;


// MODULE LOGIC ==========================================================================

// define WRITE ENABLE logic:
assign REG_WE_TIMER_0_CONFIG = ( WBs_ADR_i[ADDRWIDTH-1:2] == REG_ADDR_TIMER_0_CONFIG[ADDRWIDTH-1:2] ) && 
                                                            WBs_CYC_i && 
                                                            WBs_STB_i && 
                                                            WBs_WE_i && 
                                                            (~WBs_ACK_o);

// define the ACK back to the host for registers
assign WBs_ACK_o_nxt  =  (WBs_CYC_i) && 
                         (WBs_STB_i) && 
                         (~WBs_ACK_o);


// define WRITE logic for the registers
always @( posedge WBs_CLK_i or posedge WBs_RST_i)
begin
    if (WBs_RST_i)
    begin
        TIMER_0_CONFIG                    <= 32'h0    ;
    end  
    else
    begin

        if (REG_WE_TIMER_0_CONFIG)
        begin
            if (WBs_BYTE_STB_i[0])
                TIMER_0_CONFIG[7:0]       <= WBs_DAT_i[7:0]   ;
            if (WBs_BYTE_STB_i[1])
                TIMER_0_CONFIG[15:8]      <= WBs_DAT_i[15:8]  ;
            if (WBs_BYTE_STB_i[2])
                TIMER_0_CONFIG[23:16]     <= WBs_DAT_i[23:16] ;
            if (WBs_BYTE_STB_i[3])
                TIMER_0_CONFIG[31:24]     <= WBs_DAT_i[31:24] ;
        end
        
    WBs_ACK_o                               <=  WBs_ACK_o_nxt  ;
    end  
end


//define READ logic for the registers
always @(*)
begin
    case(WBs_ADR_i[ADDRWIDTH-1:2])
        REG_ADDR_TIMER_0_CONFIG    [ADDRWIDTH-1:2]    : WBs_DAT_o <= TIMER_0_CONFIG     ;
        default                                         : WBs_DAT_o <= DEFAULT_REG_VALUE    ;
    endcase
end


//assign TIMER_dbg_o[22] = 1'bz;
//assign TIMER_dbg_o[21] = 1'bz;


// Instantiate (sub)Modules ==============================================================

ONION_TIMER 
    u_ONION_TIMER_0
    (
    .period             ( TIMER_0_CONFIG[30:0]                      ),
    .clk                ( TIMER_clk                                 ),
    .reset              ( TIMER_0_CONFIG[31]                        ),
    .TIMER_o            ( TIMER_o[0]                                ),
    .TIMER_dbg_o        ( /*TIMER_dbg_o[18]*/                       ),
    );


endmodule
