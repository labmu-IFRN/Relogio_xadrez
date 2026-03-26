#include <virtuabotixRTC.h>
#include <Bounce2.h>
#include <LiquidCrystal.h>
#include <controle_display_lcd.h>

int tempo_brancas = 0;
int tempo_pretas  = 0;

int i = 0; // Variável para verificar a quantidade de tempo que o botão de pause esta segurado
int j; // Variável para controle de ajustes de tempo

#define pin_botao_brancas A5
#define pin_botao_pause A4
#define pin_botao_almentar_tempo A3
#define pin_botao_diminuir_tempo A2
#define pin_botao_pretas A1

// Inicializa o objeto RTC nos pinos correspondentes: CLK, DAT, RST
virtuabotixRTC myRTC(7, 8, 9);
Controle_LCD controle_lcd(0, 1, 2, 3, 4, 5);

Bounce botao_pretas = Bounce();
Bounce botao_brancas = Bounce();
Bounce botao_pause = Bounce();
Bounce botao_almentar_tempo = Bounce();
Bounce botao_diminuir_tempo = Bounce();


bool verificar_botao_pause_segurado();
void jogar();
void configurar();

void setup() {
  controle_lcd.iniciar();
  
  pinMode(pin_botao_pretas, INPUT_PULLUP);
  pinMode(pin_botao_brancas, INPUT_PULLUP);
  pinMode(pin_botao_pause, INPUT_PULLUP);
  pinMode(pin_botao_almentar_tempo, INPUT_PULLUP);
  pinMode(pin_botao_diminuir_tempo, INPUT_PULLUP);

  botao_pretas.attach(pin_botao_pretas);
  botao_brancas.attach(pin_botao_brancas);
  botao_pause.attach(pin_botao_pause);
  botao_almentar_tempo.attach(pin_botao_almentar_tempo);
  botao_diminuir_tempo.attach(pin_botao_diminuir_tempo);

  // Definir itervalo de debaunce em milissegundos
  botao_pretas.interval(100);
  botao_brancas.interval(100);
  botao_pause.interval(100);
  botao_almentar_tempo.interval(100);
  botao_diminuir_tempo.interval(100);

  Serial.begin(9600);

  // Configura a data e hora: (Segundo, Minuto, Hora, Dia da semana, Dia, Mês, Ano)
  // Exemplo: 29 de dezembro de 2024, domingo, 08:05:15
  myRTC.setDS1302Time(10, 29, 15, 1, 13, 1, 2025);
}

void loop() {
  controle_lcd.exibir_tempo(123, 123);
  myRTC.updateTime();

  botao_pretas.update();
  botao_brancas.update();
  botao_pause.update();
  botao_almentar_tempo.update();
  botao_diminuir_tempo.update();

  if (botao_pause.fell() && tempo_pretas >= 1 && tempo_brancas >=1){
    jogar();

  }

  if (verificar_botao_pause_segurado()){
    configurar();
  }
  
}




bool verificar_botao_pause_segurado(){
  if (botao_pause.fell()){
    i++;
  }

  if (i > 0 && botao_pause.rose()){
    i = 0;
  }

  if (i == 20){
    i = 0;
    return true;

  }else{
    return false;
  }
}


void jogar(){
  bool brancas_jogam = false;
  bool pretas_jogam = false;

  while(tempo_brancas > 0 && tempo_pretas > 0 && verificar_botao_pause_segurado() == false){
    int tempo_gasto = 0;
    int segundos_iniciais = myRTC.seconds;
    myRTC.updateTime();

    botao_pretas.update();
    botao_brancas.update();
    botao_pause.update();
    
    if (brancas_jogam == true){
      if (myRTC.seconds >= segundos_iniciais) {
        tempo_gasto = myRTC.seconds - segundos_iniciais;
      } else {
        tempo_gasto = (60 - segundos_iniciais) + myRTC.seconds;
      }

      tempo_brancas = tempo_brancas - tempo_gasto;
      controle_lcd.exibir_tempo(tempo_brancas, tempo_pretas);
    }

    if (pretas_jogam == true){
      if (myRTC.seconds >= segundos_iniciais) {
        tempo_gasto = myRTC.seconds - segundos_iniciais;
      } else {
        tempo_gasto = (60 - segundos_iniciais) + myRTC.seconds;
      }

      tempo_pretas = tempo_brancas - tempo_gasto;
      controle_lcd.exibir_tempo(tempo_brancas, tempo_pretas);
    }

    if (botao_brancas.fell()){
      pretas_jogam = true;
      brancas_jogam = false;
    }else if(botao_pretas.fell()){
      brancas_jogam = true;
      pretas_jogam = false;
    }else if(botao_pause.fell()){
      brancas_jogam = false;
      pretas_jogam = false;
    }

  }
}


void configurar(){
  j = 0;
  bool desligar = false;
  bool ligar = true;

  unsigned long pausa = 0;

  while(verificar_botao_pause_segurado() == false){
    botao_pause.update();
    botao_almentar_tempo.update();
    botao_diminuir_tempo.update();

    if (millis() - pausa > 200 && desligar == true){
      controle_lcd.apagar_numero(j);

      ligar = true;
      desligar = false;
    }

    if (millis() - pausa > 200 && ligar == true){
      controle_lcd.exibir_tempo(tempo_brancas, tempo_pretas);
      
      ligar = false;
      desligar = true;
    }

    if (botao_pause.fell()){
      j++;

      if (j > 7){
        j = 0;
      }
    }

    if (botao_almentar_tempo.fell()){
      switch(j){
        case 0:
          tempo_brancas = tempo_brancas + 600;
          break;

        case 1:
          tempo_brancas = tempo_brancas + 60;
          break;

        case 2:
          tempo_brancas = tempo_brancas + 10;
          break;

        case 3:
          tempo_brancas = tempo_brancas + 1;
          break;

        case 4:
          tempo_pretas = tempo_pretas + 600;
          break;

        case 5:
          tempo_pretas = tempo_pretas + 60;
          break;

        case 6:
          tempo_pretas = tempo_pretas + 10;
          break;

        case 7:
          tempo_pretas = tempo_pretas + 1;
          break;
      }
    }

    if (botao_diminuir_tempo.fell()){
      switch(j){
        case 0:
          tempo_brancas = tempo_brancas - 600;
          break;

        case 1:
          tempo_brancas = tempo_brancas - 60;
          break;

        case 2:
          tempo_brancas = tempo_brancas - 10;
          break;

        case 3:
          tempo_brancas = tempo_brancas - 1;
          break;

        case 4:
          tempo_pretas = tempo_pretas - 600;
          break;

        case 5:
          tempo_pretas = tempo_pretas - 60;
          break;

        case 6:
          tempo_pretas = tempo_pretas - 10;
          break;

        case 7:
          tempo_pretas = tempo_pretas - 1;
          break;
      }

      if (tempo_brancas < 0){
        tempo_brancas = 0;
      }else if(tempo_pretas < 0){
        tempo_pretas = 0;
      }
    }

  }
  
}