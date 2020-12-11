
module breathefpga(
    output wire redled,
    output wire greenled,
    output wire blueled
    );
    
    wire clk;

    qlal4s3b_cell_macro u_qlal4s3b_cell_macro (
        .Sys_Clk0 (clk),
    );

    parameter stop_count = 65536000;

    reg [31:0] count;
    reg [2:0] rgb;
    initial count <= 0;
    initial rgb <= 3'b111;

    breathe breathe (
        .clk(clk),
        .reset(rgb[0]),
        .led(redled)
    );

    breathe g_breathe (
        .clk(clk),
        .reset(rgb[1]),
        .led(greenled)
    );

    breathe b_breathe (
        .clk(clk),
        .reset(rgb[2]),
        .led(blueled)
    );


    always @(posedge clk) begin
        if (count == stop_count) begin
            count <= 0;
            case (rgb)
                3'b111  : rgb <= 3'b011;
                3'b011  : rgb <= 3'b101;
                3'b101  : rgb <= 3'b110;
                3'b110  : rgb <= 3'b111;
                default : rgb <= 3'b111;
            endcase
            
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

