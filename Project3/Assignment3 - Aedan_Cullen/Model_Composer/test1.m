global wrap_in
global row1_in
global row2_in
global row3_in
global row4_in
global col1_in
global col2_in
global col3_in
global col4_in

mat1 = [
    8 4 1 0
    1 48 9 89
    8 4 6 19
    3 0 88 98
];

mat2 = [
    94 1 9 1
    64 58 0 1
    98 18 7 5
    1 2 7 8
];

mat3 = [
    7 7 19 21
    65 0 98 1
    89 51 37 1
    31 45 84 7
];

mat4 = [
    54 1 2 8
    9 8 0 4
    9 4 5 1
    9 8 6 7
];

n = din_matmat([mat1, mat2], [mat3, mat4]);
disp("Done setting matrix inputs; run Simulink now and then press Enter");
pause;
disp("Model Composer results (simulation)");
disp(dout_matmat(n, out));
disp("Model Composer results (hardware co-sim)");
disp(dout_matmat_hw(n, out));
disp("MATLAB software results:");
disp([mat1*mat3, mat2*mat4]);

vec1 = [
    48
    1
    54
    23
];

vec2 = [
    46
    34
    0
    1
];

n = din_matvec([mat1, mat2], [vec1, vec2]);
disp("Done setting vector inputs; run Simulink now and then press Enter");
pause;
disp("Model Composer results (simulation)");
disp(dout_matvec(n, out));
disp("Model Composer results (hardware co-sim)");
disp(dout_matvec_hw(n, out));
disp("MATLAB software results:");
disp([mat1*vec1, mat2*vec2]);
