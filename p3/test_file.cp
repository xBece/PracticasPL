main {
	var
		int ve;
		float vf;
		char vc;
		bool vl;
		list int pe, pe2;
		list float pf, pf2;
		list char pc, pc2;
		list bool pl;
	endvar

	proc procedimientoA (int a1, float a2, char a3) {
		var
			int x1, x2;
		endvar

		proc procedimientoB (char b1, bool b2) {
			var
				float xf, x2, aux1, aux2 ;
			endvar
			
			proc procedimientoC (bool c1, mut int c2, mut float x1) {
				
			}

			procedimientoC (true, 10, xf);
			procedimientoC(false,1, aux1);
			procedimientoC(true,23, aux2);
			x2= xf*(aux1-aux2)/10.0;
			
			while (x2*aux1-xf<10.0)
				x2= x2*xf ;
		}

		proc procedimientoD (mut float d1) {
			var
				char dato ;
				int valor ;
			endvar
			
			char procedimientoE (char e1, char e2, mut char out) {
				write "introduzca dos caracteres: ";
				read e1, e2;
				if (e1=='a')
					out = e1 ;
				else if (e1=='b')
					out = e2 ;
				else
					out = ' ';
			}
			write "introduzca un valor entero: ";
			read valor ;
			if (d1 > 0.0) {
				var
					int dato ;
				endvar

				dato= 2 ;
				dato= valor*20/dato ;
			} 
			else {
				valor= valor * 100 ;
				d1= d1/1000.0 ;
			}
		}


		pe = [1, 2*a1, a1];
		insert(pe, 0, 10);
		insert(pe, 0, pe[0]);
		insert(pf, #pf, 10.0);
		insert(pc, 0, 'A')
		
		if (pe[0] == 20) {
			ve= pe[#pe-1];
			remove(pe, #pe-1);
		} 
		else {
			pe= pe * pe2 ;
			pe= pe2 - pe <- 10 * (20/2000) ;
		}
	}
}