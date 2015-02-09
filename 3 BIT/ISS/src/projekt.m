%ISS Projekt 2014
%Autor: Filip Gulán (xgulan00@stud.fit.vutbr.cz)

%zabranenie nespravneho porovnavania
iptsetpref('UseIPPL',false);
%Nacitanie obrazu
I=imread('xgulan00.bmp');

%1. Zaostrenie obrazu
h1=[-0.5 -0.5 -0.5; -0.5 5.0 -0.5; -0.5 -0.5 -0.5];
I1=imfilter(I, h1);
imshow(I1);
imwrite(I1, 'step1.bmp');

%2. Otocenie obrazu
I2=fliplr(I1);
imshow(I2);
imwrite(I2, 'step2.bmp');

%3. Medianovy filter
I3=medfilt2(I2, [5 5]);
imshow(I3);
imwrite(I3, 'step3.bmp');

%4. Rozmazanie obrazu
h2=[1 1 1 1 1; 1 3 3 3 1; 1 3 9 3 1; 1 3 3 3 1; 1 1 1 1 1]/49;
I4=imfilter(I3, h2);
imshow(I4);
imwrite(I4, 'step4.bmp');

%5. Chyba v obraze
noise=0;
I5=double(fliplr(I4));
TEMPI5=double(I);
for (x=1:512)
  for (y=1:512)
    noise=noise+abs(TEMPI5(x,y)-I5(x,y));
  end;
end;
chyba=noise/512/512

%6. Roztiahnutie histogramu
maximal=max(max(double(I4)));
minimal=min(min(double(I4)));
I6=uint8(imadjust(I4, [minimal/255 maximal/255],[0 1]));
imshow(I6);
imwrite(I6, 'step5.bmp');

%7. Stredna hodnota a smerodajna odchylka
mean_no_hist=mean2(double(I4))
std_no_hist=std2(double(I4))
mean_hist=mean2(double(I6))
std_hist=std2(double(I6))

%8. Kvantizacia obrazu
x=size(I6);
I8=zeros(x(1),x(2));
TEMPI=double(I6);
for k=1:x(1)
  for l=1:x(2)
    I8(k,l)=round(((2^2)-1)*(TEMPI(k,l))/(255))*(255)/((2^2)-1);
  end
end
I8=uint8(I8);
imshow(I8);
imwrite(I8, 'step6.bmp');

%Porovnanie
%Ixt=imread('referenc/step6.bmp');
%Ix=imread('step6.bmp');
%subplot(121); imshow(Ix); subplot(122); imshow(Ixt);
%sum(sum(abs(double(Ix) - double(Ixt))))