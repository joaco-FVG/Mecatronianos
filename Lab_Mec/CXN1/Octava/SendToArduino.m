function [] = SendToArduino(th,Port,connected)
    th1 = th(1);th2 = th(2);th3 = th(3); vel = th(4);ac = th(5);
    
    if connected==0
      disp("not connected")
      Connect(Port);
    endif
    Tx_Data= num2str([th1,th2,th3,90,vel,ac],"%5.0f")
    srl_write(Port,Tx_Data);
    pause(0.1);
endfunction

function Connect(Port)
    Port = serial(Port);
    supportedinterfaces = instrhwinfo().SupportedInterfaces
    if ! isempty(strfind (supportedinterfaces , "serial"))
        disp("Serial: Supported")
    else
        disp("Serial: Unsupported")
    endif
    %Port = serial(input('Ingrese Serial as "COMX" -> '));
    srl_flush(Port)
    pause(1);
    set(Port,'baudrate',9600);
    set(Port,'bytesize',8);
    set(Port,'parity','n');
    set(Port,'stopbits',1);
    set(Port,'timeout',50);
endfunction