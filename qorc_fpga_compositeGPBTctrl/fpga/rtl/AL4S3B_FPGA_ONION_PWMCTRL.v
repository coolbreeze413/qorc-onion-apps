
`timescale 1ns / 10ps

// each PWM output will have its own register for settings.
// PWM_x_CONFIG
//      PWM_x_DUTY_CYCLE    - [15:0]    (max, actual depends on PWM_RESOLUTION_BITS)
//      PWM_x_RESERVED      - [30:16]   future
//      PWM_x_EN            - [31]      1 to enable, 0 to disable
// so, for each IO, we will have 1 register PWM_x_CONFIG (addr = 0,4,8...)

module AL4S3B_FPGA_ONION_PWMCTRL ( 
    
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

    // PWM clk
    PWM_clk,
            
    // PWM signals
    PWM_o
);


// MODULE Parameters =====================================================================

// This is the value that is returned when a non-implemented register is read.
parameter   DEFAULT_REG_VALUE   = 32'hDEF_FAB_AC;

parameter PWM_RESOLUTION_BITS   = 8;


// MODULE Internal Parameters ============================================================
// Allow for up to 256 registers in this module
localparam  ADDRWIDTH   =  10;

// register offsets.
localparam  REG_ADDR_PWM_0_CONFIG    =  10'h000        ;
localparam  REG_ADDR_PWM_1_CONFIG    =  10'h004        ;
localparam  REG_ADDR_PWM_2_CONFIG    =  10'h008        ;

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
input       wire                PWM_clk             ;
// PWM - 2:0 IOs only.
output      wire    [31:0]      PWM_o               ;


// MODULE INTERNAL Signals ===============================================================

reg     [31:0]  PWM_0_CONFIG            ;
reg     [31:0]  PWM_1_CONFIG            ;
reg     [31:0]  PWM_2_CONFIG            ;


wire            REG_WE_PWM_0_CONFIG     ;
wire            REG_WE_PWM_1_CONFIG     ;
wire            REG_WE_PWM_2_CONFIG     ;
wire            WBs_ACK_o_nxt           ;


// MODULE LOGIC ==========================================================================

// define WRITE ENABLE logic:
assign REG_WE_PWM_0_CONFIG = ( WBs_ADR_i[ADDRWIDTH-1:2] == REG_ADDR_PWM_0_CONFIG[ADDRWIDTH-1:2] ) && 
                                                            WBs_CYC_i && 
                                                            WBs_STB_i && 
                                                            WBs_WE_i && 
                                                            (~WBs_ACK_o);

assign REG_WE_PWM_1_CONFIG = ( WBs_ADR_i[ADDRWIDTH-1:2] == REG_ADDR_PWM_1_CONFIG[ADDRWIDTH-1:2] ) && 
                                                            WBs_CYC_i && 
                                                            WBs_STB_i && 
                                                            WBs_WE_i && 
                                                            (~WBs_ACK_o);

assign REG_WE_PWM_2_CONFIG = ( WBs_ADR_i[ADDRWIDTH-1:2] == REG_ADDR_PWM_2_CONFIG[ADDRWIDTH-1:2] ) && 
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
        PWM_0_CONFIG                    <= 32'b0    ;
        PWM_1_CONFIG                    <= 32'b0    ;
        PWM_2_CONFIG                    <= 32'b0    ;
    end  
    else
    begin

        if (REG_WE_PWM_0_CONFIG)
        begin
            if (WBs_BYTE_STB_i[0])
                PWM_0_CONFIG[7:0]       <= WBs_DAT_i[7:0]   ;
            if (WBs_BYTE_STB_i[1])
                PWM_0_CONFIG[15:8]      <= WBs_DAT_i[15:8]  ;
            if (WBs_BYTE_STB_i[2])
                PWM_0_CONFIG[23:16]     <= WBs_DAT_i[23:16] ;
            if (WBs_BYTE_STB_i[3])
                PWM_0_CONFIG[31:24]     <= WBs_DAT_i[31:24] ;
        end
        
        if (REG_WE_PWM_1_CONFIG)
        begin
            if (WBs_BYTE_STB_i[0])
                PWM_1_CONFIG[7:0]           <= WBs_DAT_i[7:0]   ;
            if (WBs_BYTE_STB_i[1])
                PWM_1_CONFIG[15:8]          <= WBs_DAT_i[15:8]  ;
            if (WBs_BYTE_STB_i[2])
                PWM_1_CONFIG[23:16]         <= WBs_DAT_i[23:16] ;
            if (WBs_BYTE_STB_i[3])
                PWM_1_CONFIG[31:24]         <= WBs_DAT_i[31:24] ;
        end

        if (REG_WE_PWM_2_CONFIG)
        begin
            if (WBs_BYTE_STB_i[0])
                PWM_2_CONFIG[7:0]           <= WBs_DAT_i[7:0]   ;
            if (WBs_BYTE_STB_i[1])
                PWM_2_CONFIG[15:8]          <= WBs_DAT_i[15:8]  ;
            if (WBs_BYTE_STB_i[2])
                PWM_2_CONFIG[23:16]         <= WBs_DAT_i[23:16] ;
            if (WBs_BYTE_STB_i[3])
                PWM_2_CONFIG[31:24]         <= WBs_DAT_i[31:24] ;
        end

    WBs_ACK_o                               <=  WBs_ACK_o_nxt  ;
    end  
end


//define READ logic for the registers
always @(*)
begin
    case(WBs_ADR_i[ADDRWIDTH-1:2])
        REG_ADDR_PWM_0_CONFIG    [ADDRWIDTH-1:2]    : WBs_DAT_o <= PWM_0_CONFIG         ;
        REG_ADDR_PWM_1_CONFIG    [ADDRWIDTH-1:2]    : WBs_DAT_o <= PWM_1_CONFIG         ;
        REG_ADDR_PWM_2_CONFIG    [ADDRWIDTH-1:2]    : WBs_DAT_o <= PWM_2_CONFIG         ;
        default                                     : WBs_DAT_o <= DEFAULT_REG_VALUE    ;
    endcase
end


// Instantiate (sub)Modules ==============================================================

ONION_PWM 
    #(
        .PWM_RESOLUTION_BITS    ( PWM_RESOLUTION_BITS          )
    )
    r_ONION_PWM
    (
    .duty_cycle         ( PWM_0_CONFIG[PWM_RESOLUTION_BITS-1:0] ),
    .clk                ( PWM_clk                               ),
    .reset              ( PWM_0_CONFIG[31]                      ),
    .PWM_o              ( PWM_o[22]                             ),
    );

ONION_PWM 
    #(
        .PWM_RESOLUTION_BITS    ( PWM_RESOLUTION_BITS          )
    )
    g_ONION_PWM
    (
    .duty_cycle         ( PWM_1_CONFIG[PWM_RESOLUTION_BITS-1:0] ),
    .clk                ( PWM_clk                               ),
    .reset              ( PWM_1_CONFIG[31]                      ),
    .PWM_o              ( PWM_o[21]                             ),
    );

ONION_PWM 
    #(
        .PWM_RESOLUTION_BITS    ( PWM_RESOLUTION_BITS          )
    )
    b_ONION_PWM
    (
    .duty_cycle         ( PWM_2_CONFIG[PWM_RESOLUTION_BITS-1:0] ),
    .clk                ( PWM_clk                               ),
    .reset              ( PWM_2_CONFIG[31]                      ),
    .PWM_o              ( PWM_o[18]                             ),
    );

endmodule
