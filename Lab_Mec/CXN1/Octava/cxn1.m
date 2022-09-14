clc;clear;
%pkg install -forge instrument-control
pkg load instrument-control
function [output] = Serial_Check(input)
supportedinterfaces = instrhwinfo().SupportedInterfaces
    if ! isempty(strfind (supportedinterfaces , "serial"))
        disp("Serial: Supported")
    else
        disp("Serial: Unsupported")
    endif
endfunction

    % Habria que meter el port como input y que se conecte al puerto que va
    %Port=serial("COM4");
    % port debe ser un string como "COM4"
    %port = input("Ingrese el puerto")
   
    %[Rx_Data,count]=srl_read(Port,8)

function [] = Serial_Disconnect()
    fclose (serial('COM3'));
endfunction

function [th1,th2,th3]=CI(x,y,z,velmax,acel)
    % Longitudes del CXN-1
    l_1 = 520 ;
    l_2 = 365;
    l_3 = 220;
    % Hay que hacer una comunicacion que reciba de parte de la arduino cuando termino la incursion para poder encadenar 
    % Sets de puntos para recorrer 
     r = sqrt(x**2 + y**2);
    th3 = acos((x**2+y**2+(z-l_1)**2-l_2**2-l_3**2)/(2*l_2*l_3));
    beta = atan2((z-l_1),r);
    alfa = atan2(l_3*sin(th3),(l_2+l_3*cos(th3)));
    th2 = abs(beta) - alfa;
    th1 = atan2(y,x);

    th1 = rad2deg(th1);
    th2 = -rad2deg(th2);
    th3 = -rad2deg(th3);
    
    %th4 = rad2deg(th4);
    Port = serial("COM10")
    pause(1);
    set(Port,'baudrate',9600);
    set(Port,'bytesize',8);
    set(Port,'parity','n');
    set(Port,'stopbits',1);
    set(Port,'timeout',50);
    srl_flush(Port)
    Tx_Data= num2str([th1,th2,th3,90,velmax,acel],"%5.0f")
    srl_write(Port,Tx_Data);
    pause(0.1);
endfunction

function [] = MA(th1,th2,th3,th4,velmax,acel)
  Port = serial("COM10")
    pause(1);
    set(Port,'baudrate',9600);
    set(Port,'bytesize',8);
    set(Port,'parity','n');
    set(Port,'stopbits',1);
    set(Port,'timeout',50);
    srl_flush(Port)
    Tx_Data= num2str([th1,th2,th3,th4,velmax,acel],"%5.0f")
    srl_write(Port,Tx_Data);
    pause(0.1);
endfunction
