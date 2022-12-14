// ==============================================================
// RTL generated by Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2022.1 (64-bit)
// Version: 2022.1
// Copyright (C) Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
// 
// ===========================================================

`timescale 1 ns / 1 ps 

(* CORE_GENERATION_INFO="cordic_cordic,hls_ip_2022_1,{HLS_INPUT_TYPE=cxx,HLS_INPUT_FLOAT=0,HLS_INPUT_FIXED=0,HLS_INPUT_PART=xc7z020-clg484-1,HLS_INPUT_CLOCK=10.000000,HLS_INPUT_ARCH=others,HLS_SYN_CLOCK=6.573000,HLS_SYN_LAT=51,HLS_SYN_TPT=none,HLS_SYN_MEM=0,HLS_SYN_DSP=0,HLS_SYN_FF=160,HLS_SYN_LUT=567,HLS_VERSION=2022_1}" *)

module cordic (
        ap_clk,
        ap_rst,
        ap_start,
        ap_done,
        ap_idle,
        ap_ready,
        select_r,
        CS_I_T,
        IA_I_S,
        IA_I_C,
        CS_O_SIN,
        CS_O_SIN_ap_vld,
        CS_O_COS,
        CS_O_COS_ap_vld,
        IA_O_IAT,
        IA_O_IAT_ap_vld,
        done,
        done_ap_vld
);

parameter    ap_ST_fsm_state1 = 6'd1;
parameter    ap_ST_fsm_state2 = 6'd2;
parameter    ap_ST_fsm_state3 = 6'd4;
parameter    ap_ST_fsm_state4 = 6'd8;
parameter    ap_ST_fsm_state5 = 6'd16;
parameter    ap_ST_fsm_state6 = 6'd32;

input   ap_clk;
input   ap_rst;
input   ap_start;
output   ap_done;
output   ap_idle;
output   ap_ready;
input   select_r;
input  [15:0] CS_I_T;
input  [15:0] IA_I_S;
input  [15:0] IA_I_C;
output  [15:0] CS_O_SIN;
output   CS_O_SIN_ap_vld;
output  [15:0] CS_O_COS;
output   CS_O_COS_ap_vld;
output  [15:0] IA_O_IAT;
output   IA_O_IAT_ap_vld;
output  [15:0] done;
output   done_ap_vld;

reg ap_done;
reg ap_idle;
reg ap_ready;
reg CS_O_SIN_ap_vld;
reg CS_O_COS_ap_vld;
reg IA_O_IAT_ap_vld;
reg done_ap_vld;

(* fsm_encoding = "none" *) reg   [5:0] ap_CS_fsm;
wire    ap_CS_fsm_state1;
wire   [5:0] cordic_phase_V_address0;
reg    cordic_phase_V_ce0;
wire   [13:0] cordic_phase_V_q0;
reg   [3:0] itr_1_reg_424;
wire    ap_CS_fsm_state2;
wire   [0:0] icmp_ln27_fu_215_p2;
reg  signed [15:0] r_V_load_1_reg_437;
wire    ap_CS_fsm_state3;
reg  signed [15:0] r_V_2_load_1_reg_442;
reg   [15:0] t_V_2_load_reg_447;
wire   [17:0] r_V_4_fu_303_p2;
reg   [17:0] r_V_4_reg_452;
wire   [17:0] r_V_5_fu_313_p2;
reg   [17:0] r_V_5_reg_457;
wire   [63:0] zext_ln27_fu_210_p1;
reg  signed [15:0] r_V_fu_72;
wire   [15:0] x_V_2_fu_356_p2;
wire   [15:0] x_V_fu_163_p3;
wire    ap_CS_fsm_state4;
reg  signed [15:0] r_V_2_fu_76;
wire   [15:0] y_V_2_fu_361_p2;
wire   [15:0] y_V_fu_171_p3;
reg   [15:0] t_V_2_fu_80;
wire   [15:0] grp_fu_380_p3;
wire   [15:0] t_V_fu_179_p3;
wire    ap_CS_fsm_state6;
reg   [3:0] itr_fu_84;
wire   [3:0] add_ln27_fu_221_p2;
wire   [0:0] x_V_fu_163_p0;
wire   [0:0] y_V_fu_171_p0;
wire   [0:0] t_V_fu_179_p0;
wire   [0:0] y_V_1_fu_232_p0;
wire   [0:0] x_V_1_fu_240_p0;
wire   [0:0] t_V_4_fu_248_p0;
wire  signed [15:0] tmp_fu_256_p1;
wire   [0:0] tmp_fu_256_p3;
wire   [0:0] tmp_1_fu_272_p3;
wire   [0:0] sigma_2_fu_288_p0;
wire   [1:0] sigma_fu_264_p3;
wire   [1:0] sigma_1_fu_280_p3;
wire  signed [1:0] sigma_2_fu_288_p3;
wire  signed [1:0] r_V_4_fu_303_p0;
wire  signed [17:0] sext_ln1319_fu_299_p1;
wire  signed [1:0] r_V_5_fu_313_p0;
wire  signed [47:0] sext_ln1316_1_fu_327_p1;
wire   [47:0] zext_ln1534_fu_330_p1;
wire   [47:0] r_fu_333_p2;
wire  signed [47:0] sext_ln1316_3_fu_343_p1;
wire   [47:0] r_1_fu_346_p2;
wire   [15:0] y_shift_V_fu_352_p1;
wire   [15:0] x_shift_V_fu_339_p1;
wire   [13:0] grp_fu_380_p0;
wire    ap_CS_fsm_state5;
reg   [5:0] ap_NS_fsm;
reg    ap_ST_fsm_state1_blk;
wire    ap_ST_fsm_state2_blk;
wire    ap_ST_fsm_state3_blk;
wire    ap_ST_fsm_state4_blk;
wire    ap_ST_fsm_state5_blk;
wire    ap_ST_fsm_state6_blk;
wire   [15:0] grp_fu_380_p00;
wire    ap_ce_reg;

// power-on initialization
initial begin
#0 ap_CS_fsm = 6'd1;
end

cordic_cordic_phase_V_ROM_AUTO_1R #(
    .DataWidth( 14 ),
    .AddressRange( 64 ),
    .AddressWidth( 6 ))
cordic_phase_V_U(
    .clk(ap_clk),
    .reset(ap_rst),
    .address0(cordic_phase_V_address0),
    .ce0(cordic_phase_V_ce0),
    .q0(cordic_phase_V_q0)
);

cordic_mul_2s_16s_18_1_1 #(
    .ID( 1 ),
    .NUM_STAGE( 1 ),
    .din0_WIDTH( 2 ),
    .din1_WIDTH( 16 ),
    .dout_WIDTH( 18 ))
mul_2s_16s_18_1_1_U1(
    .din0(r_V_4_fu_303_p0),
    .din1(r_V_fu_72),
    .dout(r_V_4_fu_303_p2)
);

cordic_mul_2s_16s_18_1_1 #(
    .ID( 1 ),
    .NUM_STAGE( 1 ),
    .din0_WIDTH( 2 ),
    .din1_WIDTH( 16 ),
    .dout_WIDTH( 18 ))
mul_2s_16s_18_1_1_U2(
    .din0(r_V_5_fu_313_p0),
    .din1(r_V_2_fu_76),
    .dout(r_V_5_fu_313_p2)
);

cordic_mac_mulsub_14ns_2s_16ns_16_4_1 #(
    .ID( 1 ),
    .NUM_STAGE( 4 ),
    .din0_WIDTH( 14 ),
    .din1_WIDTH( 2 ),
    .din2_WIDTH( 16 ),
    .dout_WIDTH( 16 ))
mac_mulsub_14ns_2s_16ns_16_4_1_U3(
    .clk(ap_clk),
    .reset(ap_rst),
    .din0(grp_fu_380_p0),
    .din1(sigma_2_fu_288_p3),
    .din2(t_V_2_load_reg_447),
    .ce(1'b1),
    .dout(grp_fu_380_p3)
);

always @ (posedge ap_clk) begin
    if (ap_rst == 1'b1) begin
        ap_CS_fsm <= ap_ST_fsm_state1;
    end else begin
        ap_CS_fsm <= ap_NS_fsm;
    end
end

always @ (posedge ap_clk) begin
    if (((1'b1 == ap_CS_fsm_state1) & (ap_start == 1'b1))) begin
        itr_fu_84 <= 4'd0;
    end else if (((icmp_ln27_fu_215_p2 == 1'd0) & (1'b1 == ap_CS_fsm_state2))) begin
        itr_fu_84 <= add_ln27_fu_221_p2;
    end
end

always @ (posedge ap_clk) begin
    if (((1'b1 == ap_CS_fsm_state1) & (ap_start == 1'b1))) begin
        r_V_2_fu_76 <= y_V_fu_171_p3;
    end else if ((1'b1 == ap_CS_fsm_state4)) begin
        r_V_2_fu_76 <= y_V_2_fu_361_p2;
    end
end

always @ (posedge ap_clk) begin
    if (((1'b1 == ap_CS_fsm_state1) & (ap_start == 1'b1))) begin
        r_V_fu_72 <= x_V_fu_163_p3;
    end else if ((1'b1 == ap_CS_fsm_state4)) begin
        r_V_fu_72 <= x_V_2_fu_356_p2;
    end
end

always @ (posedge ap_clk) begin
    if (((1'b1 == ap_CS_fsm_state1) & (ap_start == 1'b1))) begin
        t_V_2_fu_80 <= t_V_fu_179_p3;
    end else if ((1'b1 == ap_CS_fsm_state6)) begin
        t_V_2_fu_80 <= grp_fu_380_p3;
    end
end

always @ (posedge ap_clk) begin
    if ((1'b1 == ap_CS_fsm_state2)) begin
        itr_1_reg_424 <= itr_fu_84;
    end
end

always @ (posedge ap_clk) begin
    if ((1'b1 == ap_CS_fsm_state3)) begin
        r_V_2_load_1_reg_442 <= r_V_2_fu_76;
        r_V_4_reg_452 <= r_V_4_fu_303_p2;
        r_V_5_reg_457 <= r_V_5_fu_313_p2;
        r_V_load_1_reg_437 <= r_V_fu_72;
        t_V_2_load_reg_447 <= t_V_2_fu_80;
    end
end

always @ (*) begin
    if (((icmp_ln27_fu_215_p2 == 1'd1) & (1'b1 == ap_CS_fsm_state2))) begin
        CS_O_COS_ap_vld = 1'b1;
    end else begin
        CS_O_COS_ap_vld = 1'b0;
    end
end

always @ (*) begin
    if (((icmp_ln27_fu_215_p2 == 1'd1) & (1'b1 == ap_CS_fsm_state2))) begin
        CS_O_SIN_ap_vld = 1'b1;
    end else begin
        CS_O_SIN_ap_vld = 1'b0;
    end
end

always @ (*) begin
    if (((icmp_ln27_fu_215_p2 == 1'd1) & (1'b1 == ap_CS_fsm_state2))) begin
        IA_O_IAT_ap_vld = 1'b1;
    end else begin
        IA_O_IAT_ap_vld = 1'b0;
    end
end

always @ (*) begin
    if ((ap_start == 1'b0)) begin
        ap_ST_fsm_state1_blk = 1'b1;
    end else begin
        ap_ST_fsm_state1_blk = 1'b0;
    end
end

assign ap_ST_fsm_state2_blk = 1'b0;

assign ap_ST_fsm_state3_blk = 1'b0;

assign ap_ST_fsm_state4_blk = 1'b0;

assign ap_ST_fsm_state5_blk = 1'b0;

assign ap_ST_fsm_state6_blk = 1'b0;

always @ (*) begin
    if (((icmp_ln27_fu_215_p2 == 1'd1) & (1'b1 == ap_CS_fsm_state2))) begin
        ap_done = 1'b1;
    end else begin
        ap_done = 1'b0;
    end
end

always @ (*) begin
    if (((1'b1 == ap_CS_fsm_state1) & (ap_start == 1'b0))) begin
        ap_idle = 1'b1;
    end else begin
        ap_idle = 1'b0;
    end
end

always @ (*) begin
    if (((icmp_ln27_fu_215_p2 == 1'd1) & (1'b1 == ap_CS_fsm_state2))) begin
        ap_ready = 1'b1;
    end else begin
        ap_ready = 1'b0;
    end
end

always @ (*) begin
    if ((1'b1 == ap_CS_fsm_state2)) begin
        cordic_phase_V_ce0 = 1'b1;
    end else begin
        cordic_phase_V_ce0 = 1'b0;
    end
end

always @ (*) begin
    if (((icmp_ln27_fu_215_p2 == 1'd1) & (1'b1 == ap_CS_fsm_state2))) begin
        done_ap_vld = 1'b1;
    end else begin
        done_ap_vld = 1'b0;
    end
end

always @ (*) begin
    case (ap_CS_fsm)
        ap_ST_fsm_state1 : begin
            if (((1'b1 == ap_CS_fsm_state1) & (ap_start == 1'b1))) begin
                ap_NS_fsm = ap_ST_fsm_state2;
            end else begin
                ap_NS_fsm = ap_ST_fsm_state1;
            end
        end
        ap_ST_fsm_state2 : begin
            if (((icmp_ln27_fu_215_p2 == 1'd1) & (1'b1 == ap_CS_fsm_state2))) begin
                ap_NS_fsm = ap_ST_fsm_state1;
            end else begin
                ap_NS_fsm = ap_ST_fsm_state3;
            end
        end
        ap_ST_fsm_state3 : begin
            ap_NS_fsm = ap_ST_fsm_state4;
        end
        ap_ST_fsm_state4 : begin
            ap_NS_fsm = ap_ST_fsm_state5;
        end
        ap_ST_fsm_state5 : begin
            ap_NS_fsm = ap_ST_fsm_state6;
        end
        ap_ST_fsm_state6 : begin
            ap_NS_fsm = ap_ST_fsm_state2;
        end
        default : begin
            ap_NS_fsm = 'bx;
        end
    endcase
end

assign CS_O_COS = ((x_V_1_fu_240_p0[0:0] == 1'b1) ? 16'd0 : r_V_fu_72);

assign CS_O_SIN = ((y_V_1_fu_232_p0[0:0] == 1'b1) ? 16'd0 : r_V_2_fu_76);

assign IA_O_IAT = ((t_V_4_fu_248_p0[0:0] == 1'b1) ? t_V_2_fu_80 : 16'd0);

assign add_ln27_fu_221_p2 = (itr_fu_84 + 4'd1);

assign ap_CS_fsm_state1 = ap_CS_fsm[32'd0];

assign ap_CS_fsm_state2 = ap_CS_fsm[32'd1];

assign ap_CS_fsm_state3 = ap_CS_fsm[32'd2];

assign ap_CS_fsm_state4 = ap_CS_fsm[32'd3];

assign ap_CS_fsm_state5 = ap_CS_fsm[32'd4];

assign ap_CS_fsm_state6 = ap_CS_fsm[32'd5];

assign cordic_phase_V_address0 = zext_ln27_fu_210_p1;

assign done = 16'd1;

assign grp_fu_380_p0 = grp_fu_380_p00;

assign grp_fu_380_p00 = cordic_phase_V_q0;

assign icmp_ln27_fu_215_p2 = ((itr_fu_84 == 4'd10) ? 1'b1 : 1'b0);

assign r_1_fu_346_p2 = $signed(sext_ln1316_3_fu_343_p1) >>> zext_ln1534_fu_330_p1;

assign r_V_4_fu_303_p0 = sext_ln1319_fu_299_p1;

assign r_V_5_fu_313_p0 = sext_ln1319_fu_299_p1;

assign r_fu_333_p2 = $signed(sext_ln1316_1_fu_327_p1) >>> zext_ln1534_fu_330_p1;

assign sext_ln1316_1_fu_327_p1 = $signed(r_V_4_reg_452);

assign sext_ln1316_3_fu_343_p1 = $signed(r_V_5_reg_457);

assign sext_ln1319_fu_299_p1 = sigma_2_fu_288_p3;

assign sigma_1_fu_280_p3 = ((tmp_1_fu_272_p3[0:0] == 1'b1) ? 2'd3 : 2'd1);

assign sigma_2_fu_288_p0 = select_r;

assign sigma_2_fu_288_p3 = ((sigma_2_fu_288_p0[0:0] == 1'b1) ? sigma_fu_264_p3 : sigma_1_fu_280_p3);

assign sigma_fu_264_p3 = ((tmp_fu_256_p3[0:0] == 1'b1) ? 2'd1 : 2'd3);

assign t_V_4_fu_248_p0 = select_r;

assign t_V_fu_179_p0 = select_r;

assign t_V_fu_179_p3 = ((t_V_fu_179_p0[0:0] == 1'b1) ? 16'd0 : CS_I_T);

assign tmp_1_fu_272_p3 = t_V_2_fu_80[32'd15];

assign tmp_fu_256_p1 = r_V_2_fu_76;

assign tmp_fu_256_p3 = tmp_fu_256_p1[32'd15];

assign x_V_1_fu_240_p0 = select_r;

assign x_V_2_fu_356_p2 = ($signed(r_V_load_1_reg_437) - $signed(y_shift_V_fu_352_p1));

assign x_V_fu_163_p0 = select_r;

assign x_V_fu_163_p3 = ((x_V_fu_163_p0[0:0] == 1'b1) ? IA_I_C : 16'd9950);

assign x_shift_V_fu_339_p1 = r_fu_333_p2[15:0];

assign y_V_1_fu_232_p0 = select_r;

assign y_V_2_fu_361_p2 = ($signed(x_shift_V_fu_339_p1) + $signed(r_V_2_load_1_reg_442));

assign y_V_fu_171_p0 = select_r;

assign y_V_fu_171_p3 = ((y_V_fu_171_p0[0:0] == 1'b1) ? IA_I_S : 16'd0);

assign y_shift_V_fu_352_p1 = r_1_fu_346_p2[15:0];

assign zext_ln1534_fu_330_p1 = itr_1_reg_424;

assign zext_ln27_fu_210_p1 = itr_fu_84;

endmodule //cordic
