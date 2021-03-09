
`timescale 1ns / 10ps

module AL4S3B_FPGA_IP ( 

    // CLOCK/RESET
    CLK_IP_i,
    RST_IP_i,

    // AHB-To_FPGA Bridge I/F
    WBs_ADR,
    WBs_CYC,
    WBs_BYTE_STB,
    WBs_WE,
    WBs_RD,
    WBs_STB,
    WBs_WR_DAT,
    WB_CLK,
    WB_RST,
    WBs_RD_DAT,
    WBs_ACK,

    // io_pad
    io_pad
);


// MODULE Parameters =====================================================================

// aperture width is 17 bits (16:0) - total address bits for the FPGA IP (this is fixed for our case)
// aperture size is 10 bits (9:0) - total address bits for each of the FPGA IP Modules
// aperture size of 10 bits indicates we can have 2^10 (1024) Bytes availablein the IP Modules.
// note that the Wishbone Address is always good ol' BYTE address.
// Ensure to use this accordingly when using with 32-bit register width (which is almost always true)
// So, as we have [9:0] address, the valid 32-bit register address would be [9:2] only.
// Hence, 10 bits indicates we have 1024 BYTEs or 256 32-bit REGISTERs available to use in the IP Module.
// This is sufficient in most cases, if not, APERSIZE needs to be changed.
parameter       APERWIDTH                       = 17            ;
parameter       APERSIZE                        = 10            ;

// base addresses of each (sub)module - note that these should be > aperture size (0x1000 here)
parameter       ONION_PWMCTRL_BASE_ADDRESS      = 17'h02000     ;
parameter       QL_RESERVED_BASE_ADDRESS        = 17'h05000     ;

// define default value returned when accessing unused address space in the FPGA IP
parameter       DEFAULT_READ_VALUE              = 32'hBAD_FAB_AC; // Bad FPGA Access

// parameters specific to QL_RESERVED IP Module
parameter       DEFAULT_CNTR_WIDTH              =  3            ;
parameter       DEFAULT_CNTR_TIMEOUT            =  7            ;

parameter       ADDRWIDTH_QL_RESERVED           = 10            ;
parameter       DATAWIDTH_QL_RESERVED           = 32            ;

parameter       QL_RESERVED_CUST_PROD_ADR       = 10'h1F8       ;
parameter       QL_RESERVED_REVISIONS_ADR       = 10'h1FC       ;

parameter       QL_RESERVED_CUSTOMER_ID         =  8'h01        ;
parameter       QL_RESERVED_PRODUCT_ID          =  8'h00        ;
parameter       QL_RESERVED_MAJOR_REV           = 16'h0001      ; 
parameter       QL_RESERVED_MINOR_REV           = 16'h0000      ;

parameter       QL_RESERVED_DEF_REG_VALUE       = 32'hDEF_FAB_AC; // Distinguish access to undefined area


// MODULE Internal Parameters ============================================================
// NONE


// MODULE PORT Declarations and Data Types ===============================================

// CLOCK/RESET
input       wire                CLK_IP_i;  
input       wire                RST_IP_i; 

// AHB-To_FPGA Bridge I/F
input       wire    [16:0]      WBs_ADR         ; // Wishbone Address Bus
input       wire                WBs_CYC         ; // Wishbone Client Cycle Strobe (i.e. Chip Select)
input       wire     [3:0]      WBs_BYTE_STB    ; // Wishbone Byte Enables
input       wire                WBs_WE          ; // Wishbone Write Enable Strobe
input       wire                WBs_RD          ; // Wishbone Read Enable Strobe
input       wire                WBs_STB         ; // Wishbone Transfer Strobe
input       wire    [31:0]      WBs_WR_DAT      ; // Wishbone Write Data Bus
input       wire                WB_CLK          ; // Wishbone Clock
input       wire                WB_RST          ; // Wishbone FPGA Reset
output      wire    [31:0]      WBs_RD_DAT      ; // Wishbone Read Data Bus
output      wire                WBs_ACK         ; // Wishbone Client Acknowledge

// io_pad
inout       wire    [31:0]      io_pad         ; // io_pad of the EOSS3


// MODULE INTERNAL Signals ===============================================================

wire            WBs_CYC_ONION_PWMCTRL           ;
wire            WBs_CYC_QL_Reserved             ;

wire            WBs_ACK_ONION_PWMCTRL           ;
wire            WBs_ACK_QL_Reserved             ;

wire    [31:0]  WBs_DAT_o_ONION_PWMCTRL         ;
wire    [31:0]  WBs_DAT_o_QL_Reserved           ;

wire    [31:0]  FPGA_IP_PWM_o                   ;

// MODULE LOGIC ==========================================================================                                                                  );

// CYC_O can be used as a "chip-select" using the base addresses, so 
// each module does not need to check this by itself, it only needs to check its WBs_CYCi is high.
// We use the base addresses, which can only be aperture-width:aperture-size.
// 16:10 in our case, as 9:0 is the module's aperture size out of 16:0 aperture width
assign WBs_CYC_QL_Reserved      = (  WBs_ADR[APERWIDTH-1:APERSIZE] == QL_RESERVED_BASE_ADDRESS[APERWIDTH-1:APERSIZE] ) 
                                & (  WBs_CYC                                                                         );

assign WBs_CYC_ONION_PWMCTRL   = (  WBs_ADR[APERWIDTH-1:APERSIZE] == ONION_PWMCTRL_BASE_ADDRESS[APERWIDTH-1:APERSIZE] ) 
                                & (  WBs_CYC                                                                            );


// Combine the ACK's from each IP module
assign WBs_ACK              =   WBs_ACK_ONION_PWMCTRL |
                                WBs_ACK_QL_Reserved;


// Multiplex the Read Data from each IP module
always @(*)
begin
    case(WBs_ADR[APERWIDTH-1:APERSIZE])
        ONION_PWMCTRL_BASE_ADDRESS      [APERWIDTH-1:APERSIZE]: WBs_RD_DAT  <=    WBs_DAT_o_ONION_PWMCTRL   ;
        QL_RESERVED_BASE_ADDRESS        [APERWIDTH-1:APERSIZE]: WBs_RD_DAT  <=    WBs_DAT_o_QL_Reserved     ;
        default:                                                WBs_RD_DAT  <=    DEFAULT_READ_VALUE        ;
    endcase
end

// Multiplex the IO signals between submodules (we have only one here)
always @(*)
begin
    io_pad <= FPGA_IP_PWM_o;
end

// Instantiate (sub)Modules ==============================================================

// PWM CONTROLLER
AL4S3B_FPGA_ONION_PWMCTRL
    u_AL4S3B_FPGA_ONION_PWMCTRL
    (
        // AHB-To_FPGA Bridge I/F
        .WBs_ADR_i          ( WBs_ADR                       ),
        .WBs_CYC_i          ( WBs_CYC_ONION_PWMCTRL        ),
        .WBs_BYTE_STB_i     ( WBs_BYTE_STB                  ),
        .WBs_WE_i           ( WBs_WE                        ),
        .WBs_STB_i          ( WBs_STB                       ),
        .WBs_DAT_i          ( WBs_WR_DAT                    ),
        .WBs_CLK_i          ( WB_CLK                        ),
        .WBs_RST_i          ( WB_RST                        ),
        .WBs_DAT_o          ( WBs_DAT_o_ONION_PWMCTRL       ),
        .WBs_ACK_o          ( WBs_ACK_ONION_PWMCTRL         ),

        // PWM clk
        .PWM_clk            ( CLK_IP_i                      ),

        // PWM signals
        .PWM_o              ( FPGA_IP_PWM_o[31:0]           )
    );

// Reserved Resources Block
// Note: This block should be used in each QL FPGA design
AL4S3B_FPGA_QL_Reserved
    #(
        .ADDRWIDTH                 ( ADDRWIDTH_QL_RESERVED          ),
        .DATAWIDTH                 ( DATAWIDTH_QL_RESERVED          ),

        .QL_RESERVED_CUST_PROD_ADR ( QL_RESERVED_CUST_PROD_ADR      ),
        .QL_RESERVED_REVISIONS_ADR ( QL_RESERVED_REVISIONS_ADR      ),

        .QL_RESERVED_CUSTOMER_ID   ( QL_RESERVED_CUSTOMER_ID        ),
        .QL_RESERVED_PRODUCT_ID    ( QL_RESERVED_PRODUCT_ID         ),
        .QL_RESERVED_MAJOR_REV     ( QL_RESERVED_MAJOR_REV          ),
        .QL_RESERVED_MINOR_REV     ( QL_RESERVED_MINOR_REV          ),
        .QL_RESERVED_DEF_REG_VALUE ( QL_RESERVED_DEF_REG_VALUE      ),

        .DEFAULT_CNTR_WIDTH        ( DEFAULT_CNTR_WIDTH             ),
        .DEFAULT_CNTR_TIMEOUT      ( DEFAULT_CNTR_TIMEOUT           )
    )
    u_AL4S3B_FPGA_QL_Reserved
    (
         // AHB-To_FPGA Bridge I/F
        .WBs_ADR_i                 ( WBs_ADR[ADDRWIDTH_QL_RESERVED+1:2] ),
        .WBs_CYC_QL_Reserved_i     ( WBs_CYC_QL_Reserved                ),
        .WBs_CYC_i                 ( WBs_CYC                            ),
        .WBs_STB_i                 ( WBs_STB                            ),
        .WBs_CLK_i                 ( WB_CLK                             ),
        .WBs_RST_i                 ( WB_RST                             ),
        .WBs_DAT_o                 ( WBs_DAT_o_QL_Reserved              ),
        .WBs_ACK_i                 ( WBs_ACK                            ),
        .WBs_ACK_o                 ( WBs_ACK_QL_Reserved                )
    );

endmodule

