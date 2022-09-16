function [Puntos] = Tarea()
  number_of_points = input("Ingrese cantidad de puntos -> ") % Posiblemente podria implementar el codigo que use en rob1
    vel_is_cnt = input('La velocidad maxima sera siempre la misma ? y/n -> ',"y")
    if (vel_is_cnt == "y")
        velmax = input('Ingrese Velocidad maxima de toda la rutina -> ')
    endif
    acel_is_cnt = input('La aceleracion sera siempre la misma ? y/n -> ',"y")
    if (acel_is_cnt == "y")
        acel = input('Ingrese la Aceleracion de toda la rutina -> ')
    endif
    Puntos = [0,0,0,0,0]
    for(i = 1:number_of_points)
        Punto = input('Ingrese Punto [X,Y,Z] ')
        if(vel_is_cnt == 'n')
            Punto = [Punto,input('Ingrese Velocidad del punto anterior al nuevo -> ')]
        else
            Punto = [Punto,velmax]
        endif
        if(acel_is_cnt == 'n')
            Punto = [Punto,input('Ingrese Aceleracion del punto anterior al nuevo -> ')]
        else
            Punto = [Punto,acel]
        endif
        Puntos(i,:) = [Punto]
      endfor
      Puntos
endfunction