module pwm (
    input wire [7:0] duty_cycle,
    input wire clk,
    input wire reset,
    output led
  );

  // simple pwm control with 256 levels (8 bits)
  // we operate in a set of 256 clock cycles, and in every set, we set the output
  // to "level" number of clock cycles to achieve output intensity variation.
  // ideally, the clk input should be fast enough so that a set of 256 clock cycles <= 1/60 second
  // this will ensure that the variation of output-on and output-off is fast enough to be imperceptible
  // to the human eye (targeting led brightness control)
  
  reg [7:0] clk_counter;            // counter for a 256 clock cycle period
  //reg [7:0] duty_cycle;             // number of clock cycles in the clk_counter period the output is on.

  reg led_state;                    // LED is on when set, off when cleared

  assign led = led_state;           // Drive all eight LEDs with led_state


  // counts 256 clock cycles and starts again
  always@ (posedge clk or negedge reset)
    if (!reset)
      clk_counter <= 8'd0;
    else
      clk_counter <= clk_counter + 8'd1;


  // drive the LED on or off depending on where we are in the 256 clock cycle period
  always@ (posedge clk or negedge reset)
    if (!reset)
      led_state <= 1'b0;
    else
      led_state <= (clk_counter <= duty_cycle);

endmodule