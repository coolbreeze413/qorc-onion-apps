module breathe (
    input clk,
    input reg [31:0] 60Hz_counter_max,
    input reset,
    output led,
    output state
  );

  // lifted and modified from : https://github.com/defano/digital-design

  // adapt the pwm technique to vary the duty-cycle in a pattern of increase-peak-decrease
  // to achieve a "breathing" effect.
  // approx 60Hz is good for a change in the led brightness for visual appeal to the human eye.
  //

  // we need to have ~60 steps - 30 steps from zero to max, and 30 steps from max to zero
  // so, time_needed_per_step = 1/60 seconds
  // have a counter which triggers a change every 1/60 second, and counter max depends on the input clock.
  // counter_val_max/time_needed_per_step = clk_freq
  // counter_val_max = clk_freq * time_needed_per_step
  // counter_val_max = clk_freq/60
  // if clk_freq == 12MHz, counter_val_max = 12000000/60, round to next lower power of 2.
  // for 12MHz, set to 16 bits (right?)


  // Theory of operation:
  //
  // LED brightness can be achieved by rapidly turning on and off the LED; the
  // longer we leave the LED on, the brighter it will appear. So long as we
  // "flash" the LED at a high rate, this will be imperceptable to the human
  // eye. This circuit uses a 128 clock-cycle period in which a varying fraction
  // of that period the LED is on. 
  //
  // To create a breathing effect, we slowly modulate the brightness level, first
  // increasing it until it reaches 100%, then slowly decreasing it.
  //
  // |MIN| | <---------- INHALE ----------> | MAX | <---------- EXHALE ----------> | |MIN|
  // | <----------------------------------- COUNTER -----------------------------------> |
  // | <--- 128 ---> || <--- 128 ---> || <--- 128 ---> || <--- 128 ---> || <--- 128 ---> |
  // | ON |   OFF    ||  ON   |  OFF  ||    ON     |OFF||  ON   |  OFF  || ON |   OFF    |
  // | inhale period, increasing on period  |      | exhale period, decreasing on period |
  //
  // step-counter -> will be used to mark the start of a step-period (1/60 sec)
  // at each step mark, the step-period-counter start running and run for 128 clock cycles.
  // at each step, the duty-cycle will be incremented from zero to 128, rollover to zero and upto 128 again.
  // each time duty-cycle reaches 128, we toggle between inhale and exhale mode.

  // step-counter 
  //    -> trigger step-period-counter -> duty-counter running
  //    -> trigger duty-cycle-increment
  //    -> && duty-cycle reaches max -> toggle inhale mode
  //    
  // at each clk -> check mode, duty-cycle, duty-counter and set the led on or off.
  // if (inhale), led is ON as long as (duty-counter < duty-cycle (duty-cycle is increasing)
  // if (exhale), led is ON as long as duty-counter > duty-cycle (duty-cycle is decreasing)


 
  // each step is 128 clocks - this is the step period, and duty_ctr tracks this
  

  reg [15:0] next_ctr;            // Counter to create a ~60Hz change in brightness
  reg [7:0] duty_ctr;             // Counter to create a 128 clock cycle period
  reg [7:0] duty_cycle;           // Number of ticks in duty_ctr that LED is on/off

  reg inhale;                     // When set, duty cycle is increasing
  reg led_state;                  // LED is on when set, off when cleared

  assign led = led_state;    // Drive all eight LEDs with led_state
  assign next = &next_ctr;        // Advance duty cycle when next_ctr saturates

  // Counter to advance the duty cycle (i.e., increase or decrease the LED
  // brightness) approx 60 times per second
  always@ (posedge clk or negedge reset)
    if (!reset)
      next_ctr <= 16'd0;
    else
      next_ctr <= next_ctr + 16'd1;

  // Duty cycle is number of clock cycles the led will be off (when inhale = 1'b0)
  // or on (when inhale = 1'b1) per each 128 clock cycle period. The greater the
  // number of cycles the led is energized per period, the brighter it will appear.
  always@ (posedge clk or negedge reset)
    if (!reset)
      duty_cycle <= 8'd0;
    else if (next)
      duty_cycle <= duty_cycle + 8'd1;

  // Counts the 128 clocks per duty cycle period.
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
      inhale <= 1'b0;
    else if (next && &duty_cycle)
      inhale <= ~inhale;

  // Drive the LEDs on or off depending on where in the 128 clock period cycle
  // we are.
  always@ (posedge clk or negedge reset)
    if (!reset)
      led_state <= 1'b0;
    else if (inhale)
      led_state <= (duty_ctr < duty_cycle);
    else
      led_state <= (duty_ctr > duty_cycle);

endmodule