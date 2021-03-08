
`timescale 1ns / 10ps

module AL4S3B_FPGA_Top (
    
    // io_pad(s) from constraint file
    io_pad
);


// MODULE Parameters =====================================================================
// NONE


// MODULE Internal Parameters ============================================================
// NONE


// MODULE PORT Declarations and Data Types ===============================================

// io_pad(s)
inout   wire    [31:0]   io_pad ;

// MODULE INTERNAL Signals ===============================================================

// FPGA Global Signals
wire            CLK_IP          ; // Selected FPGA Clock
wire            RST_IP          ; // Selected FPGA Reset

wire            Sys_Clk0        ; // a.k.a C16
wire            Sys_Clk0_Rst    ;

wire            Sys_Clk1        ; // a.k.a C21
wire            Sys_Clk1_Rst    ;

// Wishbone Bus Signals
wire    [16:0]  WBs_ADR         ; // Wishbone Address Bus
wire            WBs_CYC         ; // Wishbone Client Cycle Strobe (i.e. Chip Select)
wire     [3:0]  WBs_BYTE_STB    ; // Wishbone Byte Enables
wire            WBs_WE          ; // Wishbone Write Enable Strobe
wire            WBs_RD          ; // Wishbone Read Enable Strobe
wire            WBs_STB         ; // Wishbone Transfer Strobe
wire    [31:0]  WBs_WR_DAT      ; // Wishbone Write Data Bus
wire            WB_CLK          ; // Wishbone Clock
wire            WB_RST          ; // Wishbone FPGA Reset
wire    [31:0]  WBs_RD_DAT      ; // Wishbone Read Data Bus
wire            WBs_ACK         ; // Wishbone Client Acknowledge

wire            WB_RST_FPGA     ; // Wishbone FPGA Reset [to FPGA_IP]

// Misc
wire    [15:0]  Device_ID       ; // Provide DEVICE_ID output [to S3B Cell Macro]

// MODULE LOGIC ==========================================================================

// if Wishbone Slave interface is being used in the FPGA IP, then:
// 1. use Sys_Clk0 as the clock input for the WBs block
// 2. use Sys_Clk0 as the clocking for other logic in the FPGA IP
// We could use Sys_Clk1 for the IP clocking too, here we use same for both WB and IP logic.

// reset the FPGA IP on either the AHB domain or clock domain reset signals.
gclkbuff u_gclkbuff_reset ( .A(Sys_Clk0_Rst | WB_RST) , .Z(WB_RST_FPGA) );
// Sys_Clk0 provides clock to the WBs interface block
gclkbuff u_gclkbuff_clock ( .A(Sys_Clk0             ) , .Z(WB_CLK       ) );

// Sys_Clk0_Rst provides a reset signal for the other FPGA IP logic
assign RST_IP = Sys_Clk0_Rst;
// Sys_Clk0 provides a clock signal for the other FPGA IP logic
assign CLK_IP = Sys_Clk0;

assign Device_ID = 16'hC002; // PWM CONTROLLER ID = 0x002


// Instantiate (sub)Modules ==============================================================

// Composite "FPGA_IP"
AL4S3B_FPGA_IP 
    u_AL4S3B_FPGA_IP 
    (

        // CLOCK/RESET
        .CLK_IP_i                   ( CLK_IP                    ),
        .RST_IP_i                   ( RST_IP                    ),

        // AHB-To_FPGA Bridge I/F
        .WBs_ADR                    ( WBs_ADR                   ), // input  [16:0] | Address Bus                to   FPGA
        .WBs_CYC                    ( WBs_CYC                   ), // input         | Cycle Chip Select          to   FPGA
        .WBs_BYTE_STB               ( WBs_BYTE_STB              ), // input   [3:0] | Byte Select                to   FPGA
        .WBs_WE                     ( WBs_WE                    ), // input         | Write Enable               to   FPGA
        .WBs_RD                     ( WBs_RD                    ), // input         | Read  Enable               to   FPGA
        .WBs_STB                    ( WBs_STB                   ), // input         | Strobe Signal              to   FPGA
        .WBs_WR_DAT                 ( WBs_WR_DAT                ), // input  [31:0] | Write Data Bus             to   FPGA
        .WB_CLK                     ( WB_CLK                    ), // output        | FPGA Clock               from FPGA
        .WB_RST                     ( WB_RST_FPGA               ), // input         | FPGA Reset               to   FPGA
        .WBs_RD_DAT                 ( WBs_RD_DAT                ), // output [31:0] | Read Data Bus              from FPGA
        .WBs_ACK                    ( WBs_ACK                   ), // output        | Transfer Cycle Acknowledge from FPGA

        // io_pad(s)
        .io_pad                    ( io_pad                     ), // inout  [31:0] | IO PADs
    );


// Verilog model of QLAL4S3B
qlal4s3b_cell_macro 
    u_qlal4s3b_cell_macro 
    (
        // AHB-To-FPGA Bridge
        .WBs_ADR                   ( WBs_ADR                        ), // output [16:0] | Address Bus                   to   FPGA
        .WBs_CYC                   ( WBs_CYC                        ), // output        | Cycle Chip Select             to   FPGA
        .WBs_BYTE_STB              ( WBs_BYTE_STB                   ), // output  [3:0] | Byte Select                   to   FPGA
        .WBs_WE                    ( WBs_WE                         ), // output        | Write Enable                  to   FPGA
        .WBs_RD                    ( WBs_RD                         ), // output        | Read  Enable                  to   FPGA
        .WBs_STB                   ( WBs_STB                        ), // output        | Strobe Signal                 to   FPGA
        .WBs_WR_DAT                ( WBs_WR_DAT                     ), // output [31:0] | Write Data Bus                to   FPGA
        .WB_CLK                    ( WB_CLK                         ), // input         | FPGA Clock                    from FPGA
        .WB_RST                    ( WB_RST                         ), // output        | FPGA Reset                    to   FPGA
        .WBs_RD_DAT                ( WBs_RD_DAT                     ), // input  [31:0] | Read Data Bus                 from FPGA
        .WBs_ACK                   ( WBs_ACK                        ), // input         | Transfer Cycle Acknowledge    from FPGA

        // SDMA Signals
        .SDMA_Req                  ( {3'b000, 1'b0}                 ), // input   [3:0]
        .SDMA_Sreq                 ( 4'b0000                        ), // input   [3:0]
        .SDMA_Done                 (                                ), // output  [3:0]
        .SDMA_Active               (                                ), // output  [3:0]

        // FB Interrupts
        .FB_msg_out                ( {1'b0, 1'b0, 1'b0, 1'b0}       ), // input   [3:0]
        .FB_Int_Clr                ( 8'h0                           ), // input   [7:0]
        .FB_Start                  (                                ), // output
        .FB_Busy                   ( 1'b0                           ), // input

        // FB Clocks
        .Sys_Clk0                  ( Sys_Clk0                       ), // output
        .Sys_Clk0_Rst              ( Sys_Clk0_Rst                   ), // output
        .Sys_Clk1                  ( Sys_Clk1                       ), // output
        .Sys_Clk1_Rst              ( Sys_Clk1_Rst                   ), // output

        // Packet FIFO
        .Sys_PKfb_Clk              (  1'b0                          ), // input
        .Sys_PKfb_Rst              (                                ), // output
        .FB_PKfbData               ( 32'h0                          ), // input  [31:0]
        .FB_PKfbPush               (  4'h0                          ), // input   [3:0]
        .FB_PKfbSOF                (  1'b0                          ), // input
        .FB_PKfbEOF                (  1'b0                          ), // input
        .FB_PKfbOverflow           (                                ), // output

        // Sensor Interface
        .Sensor_Int                (                                ), // output  [7:0]
        .TimeStamp                 (                                ), // output [23:0]

        // SPI Master APB Bus
        .Sys_Pclk                  (                                ), // output
        .Sys_Pclk_Rst              (                                ), // output      <-- Fixed to add "_Rst"
        .Sys_PSel                  (  1'b0                          ), // input
        .SPIm_Paddr                ( 16'h0                          ), // input  [15:0]
        .SPIm_PEnable              (  1'b0                          ), // input
        .SPIm_PWrite               (  1'b0                          ), // input
        .SPIm_PWdata               ( 32'h0                          ), // input  [31:0]
        .SPIm_Prdata               (                                ), // output [31:0]
        .SPIm_PReady               (                                ), // output
        .SPIm_PSlvErr              (                                ), // output

        // Misc
        .Device_ID                 ( Device_ID                      ), // input  [15:0]

        // FBIO Signals
        .FBIO_In                   (                                ), // output [13:0] <-- Do Not make any connections; Use Constraint manager in SpDE to sFBIO
        .FBIO_In_En                (                                ), // input  [13:0] <-- Do Not make any connections; Use Constraint manager in SpDE to sFBIO
        .FBIO_Out                  (                                ), // input  [13:0] <-- Do Not make any connections; Use Constraint manager in SpDE to sFBIO
        .FBIO_Out_En               (                                ), // input  [13:0] <-- Do Not make any connections; Use Constraint manager in SpDE to sFBIO

        // ???
        .SFBIO                     (                                ), // inout  [13:0]
        .Device_ID_6S              ( 1'b0                           ), // input
        .Device_ID_4S              ( 1'b0                           ), // input
        .SPIm_PWdata_26S           ( 1'b0                           ), // input
        .SPIm_PWdata_24S           ( 1'b0                           ), // input
        .SPIm_PWdata_14S           ( 1'b0                           ), // input
        .SPIm_PWdata_11S           ( 1'b0                           ), // input
        .SPIm_PWdata_0S            ( 1'b0                           ), // input
        .SPIm_Paddr_8S             ( 1'b0                           ), // input
        .SPIm_Paddr_6S             ( 1'b0                           ), // input
        .FB_PKfbPush_1S            ( 1'b0                           ), // input
        .FB_PKfbData_31S           ( 1'b0                           ), // input
        .FB_PKfbData_21S           ( 1'b0                           ), // input
        .FB_PKfbData_19S           ( 1'b0                           ), // input
        .FB_PKfbData_9S            ( 1'b0                           ), // input
        .FB_PKfbData_6S            ( 1'b0                           ), // input
        .Sys_PKfb_ClkS             ( 1'b0                           ), // input
        .FB_BusyS                  ( 1'b0                           ), // input
        .WB_CLKS                   ( 1'b0                           )  // input

    );

//pragma attribute u_qlal4s3b_cell_macro        preserve_cell true
//pragma attribute u_AL4S3B_FPGA_IP             preserve_cell true

endmodule
