clc;clear;
%pkg install -forge instrument-control
pkg load instrument-control
%supportedinterfaces = instrhwinfo().SupportedInterfaces
%    if ! isempty(strfind (supportedinterfaces , "serial"))
%        disp("Serial: Supported")
%    else
%        disp("Serial: Unsupported")
%    endif


    % Habria que meter el port como input y que se conecte al puerto que va
    %Port=serial("COM4");
    % port debe ser un string como "COM4"
    %port = input("Ingrese el puerto")
   
    %[Rx_Data,count]=srl_read(Port,8)
Funcionando = true
Port = serial(input('Ingrese Serial as "COMX" -> '));
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
while Funcionando
  
   Puntos = [ 300,300,200,4000,1000
             -300,-300,400,4000,1000
                0, 585,520,4000,1000
              -300,300,400,3000,1000
               585,  0,520,4000,3000]
 
    l_1 = 520 ;
    l_2 = 365;
    l_3 = 220;
    
    for(i = 1:5)
        x = Puntos(i,1);
        y = Puntos(i,2);
        z = Puntos(i,3);
        
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
        pause(5)
    Puntos(i,:) = [th1,th2,th3,Puntos(i,4:5)]
    
      %con='0';
      %SendToArduino(Puntos(1,:),Port,1);
      Tx_Data= num2str([th1,th2,th3,90,Puntos(i,4),Puntos(i,5)],"%5.0f")
      srl_write(Port,Tx_Data);
    
      
      %con='1';
      %SendToArduino(Puntos(i,:),Port,1);
    
    pause(3)
    %input("press enter to continue")
  endfor
endwhile
