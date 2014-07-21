// LectorInclinometro.cpp : main project file.

#include "stdafx.h"
#include <conio.h>
#include <iostream>

using namespace System;
using namespace System::IO::Ports;
using namespace System::IO;


#define BAUD_RATE 9600
#define PORT_NAME "COM3"
#define FILTRO_LENGTH 30

int main(array<System::String ^> ^args)
{
	String^ id_exp; // Identificador del nombre de la carpeta
	String^ ruta; // Ruta de guardado de los datos
	String^ directorio;
	




	// Pedir nombre del experimento

	Console::WriteLine(L"Introduzca el ID del experimento (Ej: \"exp1\")");
	id_exp = Console::ReadLine();
	Console::WriteLine(L"[INFO]: ID {0}",id_exp);
	directorio = "c:\\Inclinometer_measures\\";
	directorio = directorio->Concat(directorio,id_exp);
	ruta = directorio->Concat(directorio,"\\inclinometer_data.txt");
	Console::WriteLine("[INFO]: La ruta de guardado es: {0}\n",ruta);

	// Crear carpeta con el nombre del experimento
	// Crear archivo y abrirlo para la adquisición de posiciones

	if(!Directory::Exists(directorio))DirectoryInfo^ di = Directory::CreateDirectory( directorio );
	StreamWriter^ file_datos = File::CreateText( ruta ); 

	// Conexión con arduino y creación de variables de adquisición

	Console::WriteLine(L"[INFO]: Comenzando la comunicacion");

	SerialPort^ arduino;
	arduino = gcnew SerialPort(PORT_NAME,BAUD_RATE);
	arduino->Open();
	Console::WriteLine(L"[INFO]: Conectado");
	String^ lectura;
	String^ x_string;
	String^ y_string;
	String^ rot_string;
	String^ lectura2;
	double x, y, rot, x_aux, y_aux, rot_aux, diffx, diffy;
	int separator;
	int iterador = 0;
	bool bucle_condicion = true;
	String^ menu_seleccion;


	// Bucle datos

			// Adquisición de 3 muestras basura de conexión
			
			for(int i=0; i<4; i++) lectura =  arduino->ReadLine();
			
			Console::WriteLine(L"[INFO]: Ultima lectura.... {0}",lectura);



	Console::WriteLine("Pulse intro para captar la primera posición");
	Console::ReadLine();

	do{
		
		x = 0;
		y = 0;
		rot = 0;
		x_aux = 0;
		y_aux = 0;
		

			//do{
			//	for(int i=0; i<5; i++)lectura =  arduino->ReadLine();

			//	diffx = x_aux;
			//	diffy = y_aux;


			//	separator = lectura->IndexOf('\\');
			//	x_string = lectura->Substring(0, separator);
			//	lectura = lectura->Remove(0, separator+1);
			//	x_aux = Double::Parse(x_string);

			//	diffx = abs(diffx-x_aux);

			//	separator = lectura->IndexOf('\\');
			//	y_string = lectura->Substring(0, separator);
			//	lectura = lectura->Remove(0, separator+1);
			//	y_aux = Double::Parse(y_string);

			//	diffy = abs(diffy-y_aux);

			//	rot_string = lectura;
			//	rot_aux = Double::Parse(rot_string);



			//}while(diffx > 1 || diffy > 1);

		// Adquirir FILTRO_LENGTH muestras y mediar
		for(int i=0; i<FILTRO_LENGTH; i++){


			arduino->DiscardInBuffer();
			lectura =  arduino->ReadLine();
			separator = lectura->IndexOf('\\');	
			Console::WriteLine(lectura);

			if(separator<0){

				Console::WriteLine("[*************] PRIMER FALLO");
				i--;

			}else{

				lectura2 = lectura->Remove(0, separator+1);
				
				if(lectura2->IndexOf('\\')<0){
					Console::WriteLine(lectura2);

					Console::WriteLine("[*************]  SEGUNDO FALLO");
					i--;
				}else{

					x_string = lectura->Substring(0, separator);
					lectura = lectura->Remove(0, separator+1);
					x_aux = Double::Parse(x_string);

					separator = lectura->IndexOf('\\');


					y_string = lectura->Substring(0, separator);
					lectura = lectura->Remove(0, separator+1);
					y_aux = Double::Parse(y_string);

					rot_string = lectura;
					rot_aux = Double::Parse(rot_string);

					x = x + x_aux*0.025;
					y = y + y_aux*0.025;
					rot = rot + rot_aux*0.025;


					Console::WriteLine("[INFO]:[{3}]::[{0}, {1} , {2} ]",x_aux,y_aux,rot_aux,i);
				}
			}
		}

		x = x /FILTRO_LENGTH;
		y = y / FILTRO_LENGTH;
		rot = rot / FILTRO_LENGTH;

		// Guardarlas en un archivo numeradas con un iterador

		Console::WriteLine("[INFO]:[{3}]::[{0}, {1} , {2} ]",x,y,rot,iterador);
		file_datos->WriteLine("{3} {0:E} {1:E} {2:E}",x,y,rot,iterador);



		iterador ++;

		// Preguntar si nueva adquisición / Finalizar adquisición
		Console::WriteLine(L"Otra medida ---> INTRO    Terminar? ---> PULSE 1 + INTRO");
		menu_seleccion = Console::ReadLine();
		if(menu_seleccion->Equals("1")) bucle_condicion = false;


	}while(bucle_condicion);





	// Finalizar adquisición

		// Cerrar comunicación
		arduino->Close();

		// Cerrar archivo
		file_datos->Close();

		// Informar de la cantidad adquirida.

		Console::WriteLine("\n[INFO]:Se han tomado {0} valores.",iterador);

	Console::WriteLine("\n[INFO]:Terminado, pulse ENTER para salir.");	
	Console::ReadLine();
    return 0;
}
