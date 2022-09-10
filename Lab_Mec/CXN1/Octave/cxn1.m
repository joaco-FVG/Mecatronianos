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

function [output] = Serial_Connect(port)
    % Habria que meter el port como input y que se conecte al puerto que va
    %Port=serial("COM4");
    % port debe ser un string como "COM4"
    Port = serial(port)
    pause(1);
    set(Port,'baudrate',9600);
    set(Port,'bytesize',8);
    set(Port,'parity','n');
    set(Port,'stopbits',1);
    set(Port,'timeout',50);
    srl_flush(Port);
    [Rx_Data,count]=srl_read(Port,8)

function [output] = Serial_Disconnect()
    fclose (Port);
endfunction

function [th0,th1,th2,th3]=CI(x,y,z,velmax,acel)
    % Longitudes del CXN-1
    l_1 = 492 ;
    l_2 = 365;
    l_3 = 310;
    % Hay que hacer una comunicacion que reciba de parte de la arduino cuando termino la incursion para poder encadenar 
    % Sets de puntos para recorrer 
    r = sqrt(x**2 + y**2);
    gama = ((x**2+y**2+(z-l_1)**2-l_2**2-l_3**2)/(2*l_2*l_3));
    th2 = atan((sqrt(1-gama**2))/gama);
    beta = atan((z-l_1)/r);
    alfa = atan((l_3*sin(th2))/(l_2+l_3*cos(th2)));
    th1 = (beta - alfa);
    th0 = atan(y/x);
    th3 = pi/2;
    %th4 = pi/2;
    th2 = th2 - pi/2;
    th1 = rad2deg(th1);
    th2 = rad2deg(th2);
    th3 = rad2deg(th3);
    %th4 = rad2deg(th4);

    
    Tx_Data= num2str([th0,th1,th2,th3,velmax,acel],"%5.0f")
    
    srl_write(Port,Tx_Data);
    pause(0.1);
    
    

endfunction
