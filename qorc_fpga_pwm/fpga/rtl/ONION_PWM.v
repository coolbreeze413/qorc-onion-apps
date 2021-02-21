// simple pwm control with configurable resolution
// default resolution is 8 bits - 256 levels

// we operate in a set of 2^resolution-bits clock cycles, and in every set, we set the output
// to "duty_cycle" number of clock cycles to achieve output intensity variation.
// ideally, the clk input should be fast enough so that a set of 2^resolution-bits clock cycles <= 1/60 second
// this will ensure that the variation of output-on and output-off is fast enough to be imperceptible
// to the human eye (targeting led brightness control)

// distributed pwm would be even better, however, here we implement simple on-off pwm.

module ONION_PWM (
    duty_cycle,
    clk,
    reset,
    PWM_o
);

// MODULE Parameters =====================================================================

parameter PWM_RESOLUTION_BITS = 8;


// MODULE Internal Parameters ============================================================
// NONE


// MODULE PORT Declarations and Data Types ===============================================

input       wire    [PWM_RESOLUTION_BITS-1:0]       duty_cycle          ;
input       wire                                    clk                 ;
input       wire                                    reset               ;
output      wire                                    PWM_o               ;



// MODULE INTERNAL Signals ===============================================================
reg [PWM_RESOLUTION_BITS-1:0]   clk_counter;

reg                             PWM_o_state;


// MODULE LOGIC ==========================================================================

// counts 2^resolution-bits clock cycles and starts again
always@ (posedge clk or negedge reset)
    if (!reset)
        clk_counter <= 0;
    else
        clk_counter <= clk_counter + 1;


// drive the LED on or off depending on where we are in the 2^resolution-bits clock cycle period
always@ (posedge clk or negedge reset)
    if (!reset)
        PWM_o_state <= 1'b0;
    else
        PWM_o_state <= (clk_counter <= duty_cycle);


assign PWM_o = PWM_o_state;


endmodule