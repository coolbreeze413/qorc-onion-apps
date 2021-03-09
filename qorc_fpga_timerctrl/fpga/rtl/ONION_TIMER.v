
// NOTES =========================================================
// Extremely Simple Timer (more like a counter)
// NOTES =========================================================


module ONION_TIMER (
    period,
    clk,
    reset,
    TIMER_o,
    TIMER_dbg_o,
);


// MODULE Parameters =====================================================================
// NONE


// MODULE Internal Parameters ============================================================
// NONE


// MODULE PORT Declarations and Data Types ===============================================
input       wire    [30:0]  period              ; // timer period in clock cycles (31-bits)
input       wire            clk                 ; // ref clock for timer
input       wire            reset               ; // reset signal
output      wire            TIMER_o             ; // TIMER "alarm" output
output      wire            TIMER_dbg_o         ; // TIMER debug output - use an IO.


// MODULE INTERNAL Signals ===============================================================

reg [30:0] period_ctr;                  // Counter to create a change in brightness

reg output_state;                       // TIMER output state

// MODULE LOGIC ==========================================================================

assign TIMER_o = output_state;          // Drive TIMER alarm with output_state
assign TIMER_dbg_o = output_state;

always@ (posedge clk or negedge reset)
if (!reset)
begin
    period_ctr <= 0;
end
else
begin
    if(period_ctr == period)
    begin
        // period_ctr <= 0;
        // stay as is, don't reload.
    end
    else
    begin
        period_ctr <= period_ctr + 1;
    end
end


// Drive the LEDs on or off depending on where in the 256 clock period cycle
// we are.
always@ (posedge clk or negedge reset)
if (!reset)
    output_state <= 1'b0;
else
    output_state <= (period_ctr == period);


endmodule