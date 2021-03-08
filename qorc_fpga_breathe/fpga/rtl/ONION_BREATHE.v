// ROUGH NOTES and ideas
// adapted from : https://github.com/defano/digital-design

// adapt the pwm technique to vary the duty-cycle in a pattern of increase-peak-decrease
// to achieve a "breathing" effect.
// approx 60Hz is good for a change in the BREATHE_o brightness for visual appeal to the human eye.

// Theory of operation:
//
// LED brightness can be achieved by rapidly turning on and off the LED; the
// longer we leave the LED on, the brighter it will appear. So long as we
// "flash" the LED at a high rate, this will be imperceptable to the human
// eye. This circuit uses a 256 clock-cycle period in which *a varying fraction*
// of that period the LED is on. 
//
// To create a breathing effect, we slowly modulate the brightness level, first
// increasing it until it reaches 100%, then slowly decreasing it.
//
// |MIN| | <---------- INHALE ----------> | MAX | <---------- EXHALE ----------> | |MIN|
// | <----------------------------------- COUNTER -----------------------------------> |
// | <--- 256 ---> || <--- 256 ---> || <--- 256 ---> || <--- 256 ---> || <--- 256 ---> |
// | ON |   OFF    ||  ON   |  OFF  ||    ON     |OFF||  ON   |  OFF  || ON |   OFF    |
// | inhale period, increasing on period  |      | exhale period, decreasing on period |
//
// step-counter -> will be used to mark the start of a step-period (1/60 sec)
// at each step mark, the step-period-counter start running and run for 256 clock cycles.
// at each step, the duty-cycle will be incremented from zero to 255, rollover to zero 
// and upto 255 again.
// each time duty-cycle reaches 256, we toggle between inhale and exhale mode.

// step-counter 
//    -> trigger step-period-counter -> duty-counter running
//    -> trigger duty-cycle-increment
//    -> && duty-cycle reaches max -> toggle inhale mode
//    
// at each clk -> check mode, duty-cycle, duty-counter and set the BREATHE_o on or off.
// if (inhale), BREATHE_o is ON as long as (duty-counter < duty-cycle (duty-cycle is increasing)
// if (exhale), BREATHE_o is ON as long as duty-counter > duty-cycle (duty-cycle is decreasing)

// next_ctr -> triggers duty cycle change, happens every "period" cycles (24-bits)
// at each next_ctr, duty cycle changes by 1, and we need 0 - 0xff - 0 for inhale-exhale
// so, duty_cycle sets = 256x2 = 512.

// if period cycles = 0x10000 (0x10000 cycles at each brightness level in inhale+exhale)
// then one inhale-exhale = 0x10000 * 512 = 65536 * 512 = 33 554 432 clocks or 0x2000000 clocks.
// with 8-bit pwm, 12MHz input clock, 0x10000 cycle period per duty-cycle will give 
// an approximately human breathing cycle appearance ~3seconds.


// FUTURE REIMPLEMENTATION NOTES =========================================================
// 256 clock cycle is a single unit -> PWM with PWM_RESOLUTION_BITS = 8 -> single PWM cycle
// duty_cycle_tick -> goes from 0 - 255 and repeats back from 0 (because 8 bits)

// each PWM cycle, we increment a counter so that we have a pwm_cycle_tick
// every N-PWM cycles, we change the duty_cycle from 0 - 255 - 0 in steps of 1
// it doesn't make sense to have too big N, so we keep max N = 256, min N = 2, and N is always a power of 2 for simple implementation
// pwm_cycle_tick[0] will toggle every cycle, so every 2 (or 2^N cycles with higher bits of pwm_cycle_tick) - we can kickoff change in duty_cycle

// so, duty_cyle[7:0] (PWM_RESOLUTION_BITS-1:0) we increment as a counter, at every pwm_cycle_tick[N] (N=0to7)

// this much alone would give us a breathing effect.

// now, the challenge would be to exactly give one inhale-exhale cycle (or multiples of).
// this can be left to the top level module who uses us, to stop us at appropriate clock cycles.
// so how many clock cycles for one inhale - exhale cycle?
// pwm-cycle = 256 clock cycles
// (N=0) every 2x pwm-cycle, we have duty_cycle move 1 step so 512 clock cycles for each duty-cycle
// duty-cycle goes 0 - 255 - 0 , so 512 steps per inhale-exhale cycle
// so 512x512 clock cycles per inhale-exhale cycle. = 262144 clock cycles
// basically, we need calculation (N-pwm cycles x 512) == 1 inhale-exhale period == 1 second for example.
// 2x256 x 512 == 1 second, so clock_freq = 262144 262.144kHz, so for

// FUTURE REIMPLEMENTATION NOTES =========================================================


module ONION_BREATHE (
    period,
    clk,
    reset,
    BREATHE_o,
);


// MODULE Parameters =====================================================================
// NONE


// MODULE Internal Parameters ============================================================
// NONE


// MODULE PORT Declarations and Data Types ===============================================
input       wire    [23:0]  period              ; // how many cycles for each brightness step?
input       wire            clk                 ;
input       wire            reset               ;
output      wire            BREATHE_o           ;


// MODULE INTERNAL Signals ===============================================================

reg [23:0] next_ctr;                    // Counter to create a change in brightness
reg [7:0] duty_ctr;                     // Counter to create a 256 clock cycle period
reg [7:0] duty_cycle;                   // Number of ticks in duty_ctr that LED is on/off

reg inhale;                             // When set, duty cycle is increasing
reg output_state;                       // LED is on when set, off when cleared

// MODULE LOGIC ==========================================================================

assign BREATHE_o = output_state;    // Drive LED with output_state


// Counter to advance the duty cycle (i.e., increase or decrease the LED
// brightness)
always@ (posedge clk or negedge reset)
if (!reset)
begin
    next_ctr <= 0;
end
else
begin
    if(next_ctr == period)
    begin
        next_ctr <= 0;
    end
    else
    begin        
        next_ctr <= next_ctr + 1;
    end
    
end

// Duty cycle is number of clock cycles the BREATHE_o will be off (when inhale = 1'b0)
// or on (when inhale = 1'b1) per each 256 clock cycle period. The greater the
// number of cycles the BREATHE_o is energized per period, the brighter it will appear.
always@ (posedge clk or negedge reset)
if (!reset)
    duty_cycle <= 8'd0;
else if(next_ctr == period)
    duty_cycle <= duty_cycle + 8'd1;


// Counts the 256 clocks per duty cycle period.
always@ (posedge clk or negedge reset)
if (!reset)
    duty_ctr <= 8'd0;
else
    duty_ctr <= duty_ctr + 8'd1;


// Indicates whether duty cycle specifies number of clock cycles LEDs are on,
// or number of cycles LEDs are off. This enables us to use an incrementing
// counter to both increase and decrease brightness over time
always@ (posedge clk or negedge reset)
if (!reset)
    inhale <= 1'b1;
else if ((next_ctr == period) && &duty_cycle) // counter period AND duty cycle saturated, at peak.
begin
    inhale <= ~inhale;
end

// Drive the LEDs on or off depending on where in the 256 clock period cycle
// we are.
always@ (posedge clk or negedge reset)
if (!reset)
    output_state <= 1'b0;
else if (inhale)
    output_state <= (duty_ctr < duty_cycle);
else
    output_state <= (duty_ctr > duty_cycle);


endmodule