#include <mbed.h>
#include <threadLvgl.h>
#include <threadLedsNeo.h>

#include <cstdio>

// Creation des timers
Timer tGame; // duree de la partie en cours
Timer tBuzz1; // duree de buzzage quand le joueur 1 perd le controle
Timer tDisable1; // duree pendant laquelle le joueur 1 perd le controle
Timer tBuzz2; // duree de buzzage quand le joueur 2 perd le controle
Timer tDisable2; // duree pendant laquelle le joueur 2 perd le controle
Timer tBuzzEnd; // duree de buzzage quand une partie se finit

// Recuperation des valeurs analogiques
AnalogIn val0(A0);
AnalogIn val1(A1);

PwmOut val2(A2); // signal PWM du buzzer

ThreadLvgl threadLvgl(30);
ThreadLedsNeo threadLeds(D12, 5);

// Objets LVGL
lv_obj_t * label ; // label permettant d'ecrire le texte a la creation des boutons
lv_obj_t * label1 ; // label affichant le score du joueur 1
lv_obj_t * label2 ; // label affichant le score du joueur 2
lv_obj_t * labelTime ; // label affichant le compte a rebours de la partie
lv_obj_t * labelData ; // lable affichant qui a gagne

lv_obj_t * btn1; // bouton d'action du joueur 1
lv_obj_t * btn2; // bouton d'action du joueur 2
lv_obj_t * btnStart; // bouton commencant la partie

//Variables static
static bool inGame = 0; // booleen indiquant si une partie est en cours
static bool pl1Disabled = 0; // booleen indiquant si le joueur 1 perd le controle
static bool pl2Disabled = 0; // booleen indiquant si le joueur 2 perd le controle
static bool buzzing1 = 0; // booleen indiquant si il y a un buzz apres la perte de controle du joueur 1
static bool buzzing2 = 0; // booleen indiquant si il y a un buzz apres la perte de controle du joueur 2
static bool buzzingEnd = 0; // booleen indiquant si il y a un buzz apres une fin de partie

static int seconds = 0; // duree de la partie en cours
static int mSeconds = 0; // duree de la partie en cours

static int ledChosen1 = 0; // Led choisie par le joueur 1
static int ledChosen2 = 0; // Led choisie par le joueur 2

static int objective1 = rand()%5; // Led que le joueur 1 doit selectionner
static int objective2 = rand()%5; // Led que le joueur 2 doit selectionner

static int scorepPl1 = 0; // Score du joueur 1
static int scorepPl2 = 0; // Score du joueur 2

void lv_label_score1(void) // label affichant le score du joueur 1
{
    label1 = lv_label_create(lv_scr_act());
    lv_label_set_long_mode(label1, LV_LABEL_LONG_WRAP);     /*Break the long lines*/
    lv_label_set_text(label1, " "); // on affiche rien a l'initialisation
    lv_obj_set_width(label1, 80); // taille du label
    lv_obj_set_style_text_align(label1, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(label1, LV_ALIGN_CENTER, -180, 105); // position du label
}

void lv_label_score2(void) // label affichant le score du joueur 2
{
    label2 = lv_label_create(lv_scr_act());
    lv_label_set_long_mode(label2, LV_LABEL_LONG_WRAP);
    lv_label_set_text(label2, " ");
    lv_obj_set_width(label2, 80);
    lv_obj_set_style_text_align(label2, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(label2, LV_ALIGN_CENTER, 172, 105);
}

void lv_labelTime(void) // label affichant le compte a rebours de la partie
{
    labelTime = lv_label_create(lv_scr_act());
    lv_label_set_long_mode(labelTime, LV_LABEL_LONG_WRAP);
    lv_label_set_text(labelTime, " ");
    lv_obj_set_width(labelTime, 80); 
    lv_obj_set_style_text_align(labelTime, LV_TEXT_ALIGN_LEFT, 0);
    lv_obj_align(labelTime, LV_ALIGN_CENTER, 112, 105);
}

void lv_labelData(void) // lable affichant qui a gagne
{
    labelData = lv_label_create(lv_scr_act());
    lv_label_set_long_mode(labelData, LV_LABEL_LONG_WRAP);
    lv_label_set_text(labelData, " ");
    lv_obj_set_width(labelData, 300);
    lv_obj_set_style_text_align(labelData, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(labelData, LV_ALIGN_CENTER, 0, 75);
}

static void event_handler1(lv_event_t * e) // gestion de l'event si le joueur 1 fait une action
{
    int n = 0;
    lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_PRESSED) { // si le bouton d'action a ete presse
        if(ledChosen1==(objective1+2)) // si le joueur 1 a choisi la bonne Led
        {
            for(n=0;n<5;n++)
            {
                threadLeds.setLed(n, 0,255,0);
            } // bref flash des Leds du joueur 1 en vert
            scorepPl1++; // un point en plus

            int exObjective1 = objective1;
            do {
                objective1 = rand()%5;
            }
            while (objective1==exObjective1); //nouvelle Led que le joueur 1 doit selectionner
        }
        else
        {
            tDisable1.start();
            lv_obj_clear_flag(btn1,LV_OBJ_FLAG_CLICKABLE);
            pl1Disabled = 1; // le joueur 1 perd le controle

            tBuzz1.start();
            val2.pulsewidth(0.2f);
            buzzing1 = 1; // le buzzer buzz
        }
    }
}

static void event_handler2(lv_event_t * e) // gestion de l'event si le joueur 2 fait une action, meme fonctionnement
{
    int n = 0;
    lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_PRESSED) {
        if(ledChosen2==10-objective2)
        {
            for(n=0;n<5;n++)
            {
                threadLeds.setLed(n, 0,255,0);
            }
            scorepPl2++;

            int exObjective2 = objective2;
            do {
                objective2 = rand()%5;
            }
            while (objective2==exObjective2);  
        }
        else
        {
            tDisable2.start();
            lv_obj_clear_flag(btn2,LV_OBJ_FLAG_CLICKABLE);
            pl2Disabled = 1;

            tBuzz2.start();
            val2.pulsewidth(0.2f);
            buzzing2 = 1;
        }
    }
}

static void event_handler3(lv_event_t * e) // gestion de l'event si on veut commencer une partie
{
    lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_PRESSED) {
        scorepPl1 = 0;
        scorepPl2 = 0; // score renitialise
        lv_label_set_text(labelData, " "); // changement du texte affichant le vainqueur

        tGame.start(); // Commencement du timer compte a rebours de la partie
        if(!inGame)
        {
            //Activation des boutons d'action des joueurs
            lv_obj_add_flag(btn1,LV_OBJ_FLAG_CLICKABLE);
            lv_obj_add_flag(btn2,LV_OBJ_FLAG_CLICKABLE);
        }
        lv_obj_clear_flag(btnStart,LV_OBJ_FLAG_CLICKABLE); // desactivation du bouton pour demarrer une partie quand il y en a une en cours
        inGame = 1; // une partie est en cours
    }
}

void lv_btn1(void) // bouton d'action du joueur 1
{
    btn1 = lv_btn_create(lv_scr_act());
    lv_obj_add_event_cb(btn1, event_handler1, LV_EVENT_PRESSED, &btn1); // le bouton est relie a sa gestion d'event codee plus haut
    lv_obj_set_size(btn1, 220, 180); //taille
    lv_obj_align(btn1, LV_ALIGN_CENTER, -120, -40); //position

    label = lv_label_create(btn1); // appel au label utilise pour les textes des boutons
    lv_label_set_text(label, "Action pl1"); // texte sur le bouton
    lv_obj_center(label);
}

void lv_btn2(void) // bouton d'action du joueur 2
{
    btn2 = lv_btn_create(lv_scr_act());
    lv_obj_add_event_cb(btn2, event_handler2, LV_EVENT_PRESSED, &btn2);
    lv_obj_set_size(btn2, 220, 180);
    lv_obj_align(btn2, LV_ALIGN_CENTER, 120, -40);

    label = lv_label_create(btn2);
    lv_label_set_text(label, "Action pl2");
    lv_obj_center(label);
}

void lv_btnStart(void) // bouton commencant la partie
{
    btnStart = lv_btn_create(lv_scr_act());
    lv_obj_add_event_cb(btnStart, event_handler3, LV_EVENT_PRESSED, &btnStart);
    lv_obj_set_size(btnStart, 180, 40);
    lv_obj_align(btnStart, LV_ALIGN_CENTER, -35, 108);

    label = lv_label_create(btnStart);
    lv_label_set_text(label, "Commencer");
    lv_obj_center(label);
}

int main() {
    //Initialisation
    threadLvgl.lock(); // debut des actions sur le thread LVGL

    // Initialisation des objets LVGL
    lv_btnStart();
    lv_btn1();
    lv_btn2();

    lv_label_score1();
    lv_label_score2();
    lv_labelTime();
    lv_labelData();

    // Initialisation du signal PWM du buzzer
    val2.period_ms(100);
    val2.pulsewidth(0);

    threadLvgl.unlock(); // fin des actions sur le thread LVGL

    int n = 0;

    uint32_t capt1=0;
    uint32_t capt2=0; // Valeurs des deux capteurs de pression

    while (1) {
        // put your main code here, to run repeatedly:
        if(tDisable1.read_ms()>1000) // si la duree pendant laquelle le joueur 1 perd le controle depasse 1000ms
        {
            pl1Disabled = 0; // indiquer que le joueur 1 a repris le controle

            lv_obj_add_flag(btn1,LV_OBJ_FLAG_CLICKABLE); // reactiver le bouton d'action 1

            tDisable1.stop();
            tDisable1.reset(); // desactiver et reset le timer
        }

        if(tDisable2.read_ms()>1000) // si la duree pendant laquelle le joueur 2 perd le controle depasse 1000ms
        {
            pl2Disabled = 0;

            lv_obj_add_flag(btn2,LV_OBJ_FLAG_CLICKABLE);

            tDisable2.stop();
            tDisable2.reset();
        }

        if(tBuzz1.read_ms()>200) // si le buzzage quand le joueur 1 perd le controle depasse 200ms
        {
            val2.pulsewidth(0); // desavctiver le buzzage
            tBuzz1.stop();
            tBuzz1.reset(); // desactiver et reset le timer
            buzzing1 = 0; // insiquer que le buzzage est finit
        }

        if(tBuzz2.read_ms()>200) // si le buzzage quand le joueur 2 perd le controle depasse 200ms
        {
            val2.pulsewidth(0);
            tBuzz2.stop();
            tBuzz2.reset();
            buzzing2 = 0;
        }

        if(tBuzzEnd.read_ms()>1000) // si le buzzage quand une partie se finit depasse 1000ms
        {
            val2.pulsewidth(0);
            tBuzzEnd.stop();
            tBuzzEnd.reset();
            buzzingEnd = 0;
        }
        
        if ((buzzingEnd)||(buzzing1)||(buzzing2))  // si on a desactive le buzzage mais il y avait toujours un autre buzzage en cours
        {
            val2.pulsewidth(0.2f); // reactiver le buzzage
        }

        // Recuperation des valeurs des deux capteurs de pression
        capt1 = val1.read() * 100;
        capt2 = val0.read() * 100;

        threadLvgl.lock(); // debut des actions sur le thread LVGL

        // Partie de la gestion des Leds:
        
        for(n=1;n<=30;n++)
        {
            threadLeds.setLed(n, 0,0,0);
        } // Eteint toutes les Leds

        if(inGame) // si une partie est en cours
        {
            // affichage de la led a atteindre pour chaque joueur en vert
            threadLeds.setLed(1+objective1, 0,255,0);
            threadLeds.setLed(11-objective2, 0,255,0);

            if(!pl1Disabled)
            {
                // le 1er capteur de pression allume progressivement les 5 1eres Leds en bleu
                for(n=1;n<(1+capt1/100.0*5);n++)
                {
                    if(objective1==(n-1)) threadLeds.setLed(n, 0,255,125); // si la Led selectionnee est la Led a atteindre, l'afficher en turquoise
                    else threadLeds.setLed(n, 0,0,255);
                }
                ledChosen1 = n; // Indinquer la Led choisie, c'est a dire la plus haute Led allumee
            }
            else // si le joueur 1 a perdu le controle
            {
                for(n=1;n<6;n++)
                {
                    threadLeds.setLed(n, 255,0,0);
                } // afficher toutes les Leds en rouge
            }

            if(!pl2Disabled) // meme fonctionnement pour le joueur 2
            {
                // le 2eme capteur de pression allume progressivement en sens inverse les 5 Leds d'apres en bleu
                for(n=11;n>(11-capt2/100.0*5);n--)
                {
                    if(objective2==(11-n)) threadLeds.setLed(n, 0,255,125);
                    else threadLeds.setLed(n, 0,0,255);
                }
                ledChosen2 = n;
            }
            else
            {
                for(n=11;n>6;n--)
                {
                    threadLeds.setLed(n, 255,0,0);
                }
            }

            if(tGame.read_ms()>20000) // si la partie est  terminee
            {
                tGame.stop();
                tGame.reset(); // desactiver et reset le timer du compte a rebours

                tBuzzEnd.start();
                val2.pulsewidth(0.2f);
                buzzingEnd = 1; // le buzzer buzz

                inGame = 0; // indiquer que l'on a pas de partie en cours

                lv_obj_add_flag(btnStart,LV_OBJ_FLAG_CLICKABLE); // reactiver le bouton commencant la partie

                // Afficher le resulat de la partie
                if(scorepPl1>scorepPl2) lv_label_set_text_fmt(labelData, "LE JOUEUR 1 A GAGNE !");
                else if(scorepPl1<scorepPl2) lv_label_set_text_fmt(labelData, "LE JOUEUR 2 A GAGNE !");
                else if(scorepPl1==scorepPl2) lv_label_set_text_fmt(labelData, "EGALITE !");
            }
        }
        else // si une partie n'est pas en cours
        {
            // Desactiver les boutons d'action des deux joueurs
            lv_obj_clear_flag(btn1,LV_OBJ_FLAG_CLICKABLE);
            lv_obj_clear_flag(btn2,LV_OBJ_FLAG_CLICKABLE);
        }

       // Affichage du score
       lv_label_set_text_fmt(label1, "Score joueur 1 : %"LV_PRId32, scorepPl1);
       lv_label_set_text_fmt(label2, "Score joueur 2 : %"LV_PRId32, scorepPl2);

       // Calcule du temps a afficher pour le compte a rebours
       mSeconds = (1000 - (tGame.read_ms() % 1000)) - 1 % 1000;
       seconds = 19 - (tGame.read_ms() / 1000);
       
       // Affichage du compte a rebours
       lv_label_set_text_fmt(labelTime, "%02d:%03d", seconds,mSeconds);
       
        threadLvgl.unlock();  // fin des actions sur le thread LVGL
        ThisThread::sleep_for(100ms);
    }
}
