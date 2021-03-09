
`timescale 1ns / 10ps

// each BREATHE output will have its own register for settings.
// BREATHE_x_CONFIG
//      BREATHE_x_CLK_CYCLES_PER_STEP       - [23:0]    (how many clk cycles per brightness step)
//      BREATHE_x_RESERVED                  - [30:24]   future-use (number of breaths to take...)
//      BREATHE_x_EN                        - [31]      1 to enable, 0 to disable
// so, for each IO, we will have 1 register BREATHE_x_CONFIG (addr = 0,4,8...)

module AL4S3B_FPGA_ONION_BREATHECTRL ( 
    
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

    // BREATHE clk
    BREATHE_clk,
            
    // BREATHE signals
    BREATHE_o
);


// MODULE Parameters =====================================================================

// This is the value that is returned when a non-implemented register is read.
parameter   DEFAULT_REG_VALUE                       = 32'hDEF_FAB_AC;   // no such register

parameter   BREATHE_PWM_RESOLUTION_BITS             = 8;                // future-use

parameter   DEFAULT_BREATHE_CLK_CYCLES_PER_STEP     = 24'hAAAA;         // 1-sec-inhale,1-sec-exhale


// MODULE Internal Parameters ============================================================
// Allow for up to 256 registers in this module
localparam  ADDRWIDTH                   =  10;

// register offsets.
localparam  REG_ADDR_BREATHE_0_CONFIG   =  10'h000        ;
localparam  REG_ADDR_BREATHE_1_CONFIG   =  10'h004        ;
localparam  REG_ADDR_BREATHE_2_CONFIG   =  10'h008        ;

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

// PWM clock
input       wire                BREATHE_clk         ;
// PWM - 2:0 IOs only.
output      wire    [31:0]      BREATHE_o           ;


// MODULE INTERNAL Signals ===============================================================

reg     [31:0]  BREATHE_0_CONFIG            ;
reg     [31:0]  BREATHE_1_CONFIG            ;
reg     [31:0]  BREATHE_2_CONFIG            ;


wire            REG_WE_BREATHE_0_CONFIG     ;
wire            REG_WE_BREATHE_1_CONFIG     ;
wire            REG_WE_BREATHE_2_CONFIG     ;
wire            WBs_ACK_o_nxt               ;


// MODULE LOGIC ==========================================================================

// define WRITE ENABLE logic:
assign REG_WE_BREATHE_0_CONFIG = ( WBs_ADR_i[ADDRWIDTH-1:2] == REG_ADDR_BREATHE_0_CONFIG[ADDRWIDTH-1:2] ) && 
                                                            WBs_CYC_i && 
                                                            WBs_STB_i && 
                                                            WBs_WE_i && 
                                                            (~WBs_ACK_o);

assign REG_WE_BREATHE_1_CONFIG = ( WBs_ADR_i[ADDRWIDTH-1:2] == REG_ADDR_BREATHE_1_CONFIG[ADDRWIDTH-1:2] ) && 
                                                            WBs_CYC_i && 
                                                            WBs_STB_i && 
                                                            WBs_WE_i && 
                                                            (~WBs_ACK_o);

assign REG_WE_BREATHE_2_CONFIG = ( WBs_ADR_i[ADDRWIDTH-1:2] == REG_ADDR_BREATHE_2_CONFIG[ADDRWIDTH-1:2] ) && 
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
        BREATHE_0_CONFIG                    <= 32'hAAAA    ;
        BREATHE_1_CONFIG                    <= 32'hAAAA    ;
        BREATHE_2_CONFIG                    <= 32'hAAAA    ;
    end  
    else
    begin

        if (REG_WE_BREATHE_0_CONFIG)
        begin
            if (WBs_BYTE_STB_i[0])
                BREATHE_0_CONFIG[7:0]       <= WBs_DAT_i[7:0]   ;
            if (WBs_BYTE_STB_i[1])
                BREATHE_0_CONFIG[15:8]      <= WBs_DAT_i[15:8]  ;
            if (WBs_BYTE_STB_i[2])
                BREATHE_0_CONFIG[23:16]     <= WBs_DAT_i[23:16] ;
            if (WBs_BYTE_STB_i[3])
                BREATHE_0_CONFIG[31:24]     <= WBs_DAT_i[31:24] ;
        end
        
        if (REG_WE_BREATHE_1_CONFIG)
        begin
            if (WBs_BYTE_STB_i[0])
                BREATHE_1_CONFIG[7:0]       <= WBs_DAT_i[7:0]   ;
            if (WBs_BYTE_STB_i[1])
                BREATHE_1_CONFIG[15:8]      <= WBs_DAT_i[15:8]  ;
            if (WBs_BYTE_STB_i[2])
                BREATHE_1_CONFIG[23:16]     <= WBs_DAT_i[23:16] ;
            if (WBs_BYTE_STB_i[3])
                BREATHE_1_CONFIG[31:24]     <= WBs_DAT_i[31:24] ;
        end

        if (REG_WE_BREATHE_2_CONFIG)
        begin
            if (WBs_BYTE_STB_i[0])
                BREATHE_2_CONFIG[7:0]       <= WBs_DAT_i[7:0]   ;
            if (WBs_BYTE_STB_i[1])
                BREATHE_2_CONFIG[15:8]      <= WBs_DAT_i[15:8]  ;
            if (WBs_BYTE_STB_i[2])
                BREATHE_2_CONFIG[23:16]     <= WBs_DAT_i[23:16] ;
            if (WBs_BYTE_STB_i[3])
                BREATHE_2_CONFIG[31:24]     <= WBs_DAT_i[31:24] ;
        end

    WBs_ACK_o                               <=  WBs_ACK_o_nxt  ;
    end  
end


//define READ logic for the registers
always @(*)
begin
    case(WBs_ADR_i[ADDRWIDTH-1:2])
        REG_ADDR_BREATHE_0_CONFIG    [ADDRWIDTH-1:2]    : WBs_DAT_o <= BREATHE_0_CONFIG     ;
        REG_ADDR_BREATHE_1_CONFIG    [ADDRWIDTH-1:2]    : WBs_DAT_o <= BREATHE_1_CONFIG     ;
        REG_ADDR_BREATHE_2_CONFIG    [ADDRWIDTH-1:2]    : WBs_DAT_o <= BREATHE_2_CONFIG     ;
        default                                         : WBs_DAT_o <= DEFAULT_REG_VALUE    ;
    endcase
end


// Instantiate (sub)Modules ==============================================================

ONION_BREATHE 
    // #(
    //     .BREATHE_PWM_RESOLUTION_BITS    ( BREATHE_PWM_RESOLUTION_BITS          )
    // )
    r_ONION_BREATHE
    (
    .period             ( BREATHE_0_CONFIG[23:0]                    ),
    .clk                ( BREATHE_clk                               ),
    .reset              ( BREATHE_0_CONFIG[31]                      ),
    .BREATHE_o          ( BREATHE_o[22]                             ),
    );

ONION_BREATHE 
    // #(
    //     .BREATHE_RESOLUTION_BITS    ( BREATHE_RESOLUTION_BITS          )
    // )
    g_ONION_BREATHE
    (
    .period             ( BREATHE_1_CONFIG[23:0]                    ),
    .clk                ( BREATHE_clk                               ),
    .reset              ( BREATHE_1_CONFIG[31]                      ),
    .BREATHE_o          ( BREATHE_o[21]                             ),
    );

ONION_BREATHE 
    // #(
    //     .BREATHE_RESOLUTION_BITS    ( BREATHE_RESOLUTION_BITS          )
    // )
    b_ONION_BREATHE
    (
    .period             ( BREATHE_2_CONFIG[23:0]                    ),
    .clk                ( BREATHE_clk                               ),
    .reset              ( BREATHE_2_CONFIG[31]                      ),
    .BREATHE_o          ( BREATHE_o[18]                             ),
    );

endmodule
