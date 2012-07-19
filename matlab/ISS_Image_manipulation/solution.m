function [] = solution()
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here

iptsetpref('UseIPPL',false);

I=imread('xblask00.bmp');

suradnice = size(I);
sx = suradnice(1);
sy = suradnice(2);

% 1. zaostrenie obrazu
mat=[-0.5 -0.5 -0.5; -0.5 5.0 -0.5; -0.5 -0.5 -0.5];
step1 = imfilter(I,mat);
imwrite(step1, 'step1.bmp');
step1_ref = imread('step1_ref.bmp');
step1_odchylka = sum(sum(abs(double(step1) - double(step1_ref))));

% 2. otocenie obrazu
for (x=1:sx)
	for (y=0:(sy-1))
		step2(x,y+1) = step1(x,sy-y);
	end;
end;
imwrite(step2, 'step2.bmp');
step2_ref = imread('step2_ref.bmp');
step2_odchylka = sum(sum(abs(double(step2) - double(step2_ref))));

% 3. medianovy filter
step3 = medfilt2(step2, [5 5]);
imwrite(step3, 'step3.bmp');
step3_ref = imread('step3_ref.bmp');
step3_odchylka = sum(sum(abs(double(step3) - double(step3_ref))));

% 4. rozostrenie
mat=[1 1 1 1 1; 1 3 3 3 1; 1 3 9 3 1; 1 3 3 3 1; 1 1 1 1 1] / 49;
step4 = imfilter(step3,mat);
imwrite(step4, 'step4.bmp');
step4_ref = imread('step4_ref.bmp');
step4_odchylka = sum(sum(abs(double(step4) - double(step4_ref))));

% 5. chyba
for (x=1:sx)
	for (y=0:(sy-1))
		step4_rev(x,y+1) = double(step4(x,sy-y)); % otocenie
	end;
end;

odchylka=0;

step4_rev = double(step4_rev);
I_double = double(I);

for (x=1:sx)
	for (y=1:sy)
		odchylka=odchylka+double(abs(I_double(x,y)-step4_rev(x,y)));
	end;
end;
odchylka=odchylka/sx/sy;
odchylka

% 6. histogram
minimalna = min(min(im2double(step4)));
maximalna = max(max(im2double(step4)));
step5 = imadjust(step4, [minimalna maximalna], [0 1]);
imwrite(step5, 'step5.bmp');
step5_ref = imread('step5_ref.bmp');
step5_odchylka = sum(sum(abs(double(step5) - double(step5_ref))));

% 7. smerodatna odchylka a stredna hodnota
mean_no_hist = mean2(im2double(step4))*255
std_no_hist = std2(im2double(step4))*255
mean_hist = mean2(im2double(step5))*255
std_hist = std2(im2double(step5))*255

% 8. kvantizacia
N = 2;
a = 0;
b = 255;
step6 = zeros(sx,sy);
step6 = round(((2^N)-1)*(double(step5)-a)/(b-a))*(b-a)/((2^N)-1) + a;
step6 = uint8(step6);
imwrite(step6, 'step6.bmp');
step6_ref = imread('step6_ref.bmp');
step6_odchylka = sum(sum(abs(double(step6) - double(step6_ref))));
