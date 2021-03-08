
module AL4S3B_FPGA_Top(
    
    // LEDs from constraint file
    red_led,
    green_led,
    blue_led
);

// MODULE Parameters =====================================================================

// depending on the PWM RESOLUTION we use, we calculate how many brightness steps per breathe:
// 512 = num-steps-per-breathe-cycle (inhale+exhale) for 8-bit pwm (0 - 0xFF - 0)
// BREATHE_STEPS_PER_CYCLE = 2*(1 << PWM_RESOLUTION_BITS) = 1<<PWM_RESOLUTION_BITS+1
// PWM_RESOLUTION_BITS = 8, then, BREATHE_STEPS_PER_CYCLE = 512
parameter BREATHE_PWM_RESOLUTION_BITS = 8;
parameter BREATHE_STEPS_PER_CYCLE = 1 << (BREATHE_PWM_RESOLUTION_BITS + 1);

// how many clock cycles do we want to spend at each brightness level? BREATHE_CLK_CYCLES_PER_STEP
// lower the number, faster the breathing. 0xAAAA approx 1 sec inhale, 1 sec exhale.
parameter BREATHE_CLK_CYCLES_PER_STEP = 24'hAAAA;

// how many clock cycles does it take then, to complete one breathe cycle?
parameter BREATHE_CLK_CYCLES_PER_BREATHE_CYCLE = BREATHE_CLK_CYCLES_PER_STEP * BREATHE_STEPS_PER_CYCLE;
// total-clock-cycles-per-breathe-cycle = BREATHE_STEP_CLK_CYCLES * BREATHE_STEPS_PER_CYCLE
// always use STOP_COUNT as a multiple of total-clock-cycles-per-breathe-cycle to get a smooth
// breathing transition!
// if cycle_period=0x10000, stop_count = 0x10000 x 512 = 33554432 == 0x2000000

// after how many breathe cycles do we want our state machine to transition?
parameter STOP_COUNT = 2*BREATHE_CLK_CYCLES_PER_BREATHE_CYCLE;


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
reg [23:0] breathe_period;
initial count <= 0;     // does not synthesize!
initial rgb <= 3'b000;  // does not synthesize!
initial breathe_period <= BREATHE_CLK_CYCLES_PER_STEP;


// MODULE LOGIC ==========================================================================

always @(posedge Sys_Clk0 or posedge Sys_Clk0_Rst) 
begin
    if(Sys_Clk0_Rst)
    begin // handle reset
        rgb <= 3'b000;
        count <= 0;
    end // handle reset
    else
    begin // not reset
        // as an alternative to reset, transition stuff at first clock edge (workaround!)
        if(rgb == 3'b000)
        begin
            count <= 0;
            rgb <= 3'b100; // goto blue
        end
        if (count == STOP_COUNT) 
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
    end // not reset
end

// provide the configuration of how many clocks to spend at each brightness step to the BREATHE module.
always @(*)
begin
    breathe_period <= BREATHE_CLK_CYCLES_PER_STEP;
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
    .period(breathe_period),
    .clk(Sys_Clk0),
    .reset(rgb[0]),
    .BREATHE_o(red_led)
);

ONION_BREATHE g_breathe (
    .period(breathe_period),
    .clk(Sys_Clk0),
    .reset(rgb[1]),
    .BREATHE_o(green_led)
);

ONION_BREATHE b_breathe (
    .period(breathe_period),
    .clk(Sys_Clk0),
    .reset(rgb[2]),
    .BREATHE_o(blue_led)
);


endmodule
