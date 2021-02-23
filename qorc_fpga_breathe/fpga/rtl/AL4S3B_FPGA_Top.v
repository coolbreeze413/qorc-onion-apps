
module AL4S3B_FPGA_Top(
    
    // LEDs from constraint file
    red_led,
    green_led,
    blue_led
);

// MODULE Parameters =====================================================================

parameter stop_count = 33554432; // 33554432 == 0x2000000


// MODULE Internal Parameters ============================================================


// MODULE PORT Declarations and Data Types ===============================================

output      wire            red_led              ;
output      wire            green_led            ;
output      wire            blue_led             ;

// MODULE INTERNAL Signals ===============================================================

wire Sys_Clk0;
wire Sys_Clk0_Rst;
wire Sys_Clk1;
wire Sys_Clk1_Rst;
reg [31:0] count;
reg [2:0] rgb;
initial count <= 0;     // does not synthesize!
initial rgb <= 3'b000;  // does not synthesize!



// MODULE LOGIC ==========================================================================

always @(posedge Sys_Clk0 or posedge Sys_Clk0_Rst) 
begin
    if(Sys_Clk0_Rst)
    begin // handle reset
        rgb <= 3'b000;
        count <= 0;
    end // handle reset
    else
        if (count == stop_count) 
        begin
            count <= 0;
            case (rgb)
                3'b000  : rgb <= 3'b100;    // goto blue
                3'b100  : rgb <= 3'b010;    // goto green
                3'b010  : rgb <= 3'b001;    // goto red
                3'b001  : rgb <= 3'b011;    // goto r+g = yellow
                3'b011  : rgb <= 3'b101;    // goto r+b = magenta
                3'b101  : rgb <= 3'b110;    // goto g+b = cyan
                3'b110  : rgb <= 3'b100;    // goto blue
                default : rgb <= 3'b111;    // goto OFF (black)
            endcase
        end 
        else 
        begin
            count <= count + 1;
        end
end


// Instantiate (sub)Modules ==============================================================

// we need only the clocks/resets from the cell macro
qlal4s3b_cell_macro u_qlal4s3b_cell_macro (
    // FB Clocks
    .Sys_Clk0                  ( Sys_Clk0                       ), // output
    .Sys_Clk0_Rst              ( Sys_Clk0_Rst                   ), // output
    .Sys_Clk1                  ( Sys_Clk1                       ), // output
    .Sys_Clk1_Rst              ( Sys_Clk1_Rst                   ), // output
);

ONION_BREATHE r_breathe (
    .clk(Sys_Clk0),
    .reset(rgb[0]),
    .BREATHE_o(red_led)
);

ONION_BREATHE g_breathe (
    .clk(Sys_Clk0),
    .reset(rgb[1]),
    .BREATHE_o(green_led)
);

ONION_BREATHE b_breathe (
    .clk(Sys_Clk0),
    .reset(rgb[2]),
    .BREATHE_o(blue_led)
);


endmodule
