int main() {
   if(pouY < altura_pou){
       collision = true;
    }
    
    if(pou_passou_do_piso()) {
        checkCollision = true;
    }

    if(checkCollision) {
            // se o pou estiver passando pelo retangulo na descida;
            if(pou_passando_na_descida()) {
                if(pou_no_piso_em_X()) {
                    collision = true;
                    checkCollision = false;
                    altura_pou = piso_Y[ind_piso_Y_menor];
                    ind_piso_Y_menor++;
                    altura_tela += P_DISTANCE - 0.1;
                }

            }
            if(pouY < piso_Y[ind_piso_Y_menor]) {
                checkCollision = false;
        }
    }
    
}