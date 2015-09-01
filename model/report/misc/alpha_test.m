function [alpha, theta] = alpha_test(  )
%ALPHA_TEST Summary of this function goes here
%   Detailed explanation goes here

    alpha_0 = -0.68;
    lx = 0.076;
    Dx = 0.0665;
    Hx = 0.047;
    h = 0.0156;
    b = 0.040;
    r = 0.02;

    alpha = 0;
    theta = 0;
    num = 3.14/0.01;
    for i = 1:num
        val = i*0.01;
        alpha(i) = val;
        theta(i) = calcolo_theta(Hx, lx, r, h, Dx, b, val, alpha_0);
    end
    figure();
    plot (alpha, theta);
    xlabel('alpha');
    ylabel('tetha');
end
