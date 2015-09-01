function [ theta ] = calcolo_theta( H, l, r, h, D, b, alpha, alpha_0 )
%CALCOLO_THETA Summary of this function goes here
%   Detailed explanation goes here

    cos_alpha = cos(alpha_0)-alpha*sin(alpha_0);
    sin_alpha = alpha*cos(alpha_0)+sin(alpha_0);

    num = H*H+l*l+r*r+h*h+D*D-b*b;
    num = num+2*D*r*cos_alpha-2*H*r*sin_alpha;
    num = num+2*h*r*sin_alpha-2*H*h-2*l*D-2*l*r*cos_alpha;

    den = 2*l*r*sin_alpha+2*h*D+2*r*h*cos_alpha-2*H*l;

    theta = num/den;


end

