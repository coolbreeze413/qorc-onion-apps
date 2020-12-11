
module pwmfpga(
    output wire redled,
    output wire greenled,
    output wire blueled
    );
    
    wire clk;

    qlal4s3b_cell_macro u_qlal4s3b_cell_macro (
        .Sys_Clk0 (clk),
    );

    reg [7:0] red_val;
    reg [7:0] green_val;
    reg [7:0] blue_val;

    reg red_en;
    reg green_en;
    reg blue_en;

    reg [7:0] intensity;


    parameter stop_count = 65536000;

    reg [31:0] count;
    reg [2:0] rgb;
    initial count <= 0;
    initial rgb <= 3'b111;

    pwm r_pwm (
        .clk(clk),
        .duty_cycle(red_val),
        .reset(red_en),
        .led(redled)
    );

    pwm g_pwm (
        .clk(clk),
        .duty_cycle(green_val),
        .reset(green_en),
        .led(greenled)
    );

    pwm b_pwm (
        .clk(clk),
        .duty_cycle(blue_val),
        .reset(blue_en),
        .led(blueled)
    );

    always @(*) begin
        red_en <= 1;
        green_en <= 1;
        blue_en <= 1;
    end


    always @(posedge clk) begin
        if (count == stop_count) begin
            count <= 0;            
            intensity <= intensity + 32;
            toggle <= ~toggle;
            if(toggle) begin
                red_val <= intensity;
                green_val <= 8'd0;
                blue_val <= intensity;
            end else begin
                red_val <= intensity;
                green_val <= intensity;
                blue_val <= 8'd0;
            end
            
        end else begin
            count <= count + 1;
        end
    end

/*
    assign redled = rgb[0];
    assign greenled = rgb[1];
    assign blueled = rgb[2];
*/

endmodule

