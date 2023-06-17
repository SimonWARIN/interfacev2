#include <mbed.h>
#include <threadLvgl.h>
#include <threadLedsNeo.h>

#include <cstdio>

Timer tBuzz1;
Timer tDisable1;
Timer tBuzz2;
Timer tDisable2;

AnalogIn val0(A0);
AnalogIn val1(A1);
PwmOut val2(A2);

ThreadLvgl threadLvgl(30);
ThreadLedsNeo threadLeds(D12, 5);

//lv_obj_t * bar1 ;
//lv_obj_t * bar2 ;
lv_obj_t * label ;
lv_obj_t * label1 ;
lv_obj_t * label2 ;
lv_obj_t * btn1;
lv_obj_t * btn2;

static int input1 = 0;
static int input2 = 0;

static int ledChosen1 = 0;
static int ledChosen2 = 0;

static int objective1 = rand()%5;
static int objective2 = rand()%5;

static int pl1Disabled = 0;
static int pl2Disabled = 0;

static int scorepPl1 = 0;
static int scorepPl2 = 0;

/*
void lv_bar1(void)
{
    static lv_style_t style_indic;

    lv_style_init(&style_indic);
    lv_style_set_bg_opa(&style_indic, LV_OPA_COVER);
    lv_style_set_bg_color(&style_indic, lv_palette_main(LV_PALETTE_RED));
    lv_style_set_bg_grad_color(&style_indic, lv_palette_main(LV_PALETTE_BLUE));
    lv_style_set_bg_grad_dir(&style_indic, LV_GRAD_DIR_VER);

    bar1 = lv_bar_create(lv_scr_act());
    lv_obj_add_style(bar1, &style_indic, LV_PART_INDICATOR);
    lv_obj_set_size(bar1, 20, 200);
    lv_obj_center(bar1);
    lv_bar_set_range(bar1, 0, 100);
    lv_obj_set_x(bar1, -100);

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_time(&a, 3000);
    lv_anim_set_playback_time(&a, 3000);
    lv_anim_set_var(&a, bar1);
    lv_anim_set_values(&a, 0, 100);
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
    lv_anim_start(&a);
}

void lv_bar2(void)
{
    static lv_style_t style_indic;

    lv_style_init(&style_indic);
    lv_style_set_bg_opa(&style_indic, LV_OPA_COVER);
    lv_style_set_bg_color(&style_indic, lv_palette_main(LV_PALETTE_RED));
    lv_style_set_bg_grad_color(&style_indic, lv_palette_main(LV_PALETTE_BLUE));
    lv_style_set_bg_grad_dir(&style_indic, LV_GRAD_DIR_VER);

    bar2 = lv_bar_create(lv_scr_act());
    lv_obj_add_style(bar2, &style_indic, LV_PART_INDICATOR);
    lv_obj_set_size(bar2, 20, 200);
    lv_obj_center(bar2);
    lv_bar_set_range(bar2, 0, 100);
    lv_obj_set_x(bar2, 100);

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_time(&a, 3000);
    lv_anim_set_playback_time(&a, 3000);
    lv_anim_set_var(&a, bar2);
    lv_anim_set_values(&a, 0, 100);
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
    lv_anim_start(&a);
}
*/

void lv_label_score1(void)
{
    label1 = lv_label_create(lv_scr_act());
    lv_label_set_long_mode(label1, LV_LABEL_LONG_WRAP);     /*Break the long lines*/
    lv_label_set_recolor(label1, true);                      /*Enable re-coloring by commands in the text*/
    lv_label_set_text(label1, "#0000ff Re-color# #ff00ff words# #ff0000 of a# label, align the lines to the center "
                      "and wrap long text automatically.");
    lv_obj_set_width(label1, 80);  /*Set smaller width to make the lines wrap*/
    lv_obj_set_style_text_align(label1, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(label1, LV_ALIGN_CENTER, -150, -105);
}

void lv_label_score2(void)
{
    label2 = lv_label_create(lv_scr_act());
    lv_label_set_long_mode(label2, LV_LABEL_LONG_WRAP);     /*Break the long lines*/
    lv_label_set_recolor(label2, true);                      /*Enable re-coloring by commands in the text*/
    lv_label_set_text(label2, "#0000ff Re-color# #ff00ff words# #ff0000 of a# label, align the lines to the center "
                      "and wrap long text automatically.");
    lv_obj_set_width(label2, 80);  /*Set smaller width to make the lines wrap*/
    lv_obj_set_style_text_align(label2, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(label2, LV_ALIGN_CENTER, 150, -105);
    lv_obj_set_x(label2, 170);
}

static void event_handler1(lv_event_t * e)
{
    int n = 0;
    lv_event_code_t code = lv_event_get_code(e);

    input1 = 0;
    if(code == LV_EVENT_PRESSED) {
        LV_LOG_USER("Clicked");

        tBuzz1.start();
        //val2.pulsewidth(0.2f);

        if(ledChosen1==(objective1+2))
        {
            for(n=0;n<5;n++)
            {
                threadLeds.setLed(n, 0,255,0);
            }
            scorepPl1++;
            objective1 = rand()%5;
        }
        else
        {
            tDisable1.start();
            lv_obj_clear_flag(btn1,LV_OBJ_FLAG_CLICKABLE);
            pl1Disabled = 1;
        }

        input1 = 1;
        printf("%d\n",input1);
    }
}

static void event_handler2(lv_event_t * e)
{
    int n = 0;
    lv_event_code_t code = lv_event_get_code(e);

    input2 = 0;
    if(code == LV_EVENT_PRESSED) {
        LV_LOG_USER("Clicked");

        tBuzz2.start();
        //val2.pulsewidth(0.2f);

        if(ledChosen2==10-objective2)
        {
            for(n=0;n<5;n++)
            {
                threadLeds.setLed(n, 0,255,0);
            }
            scorepPl2++;
            objective2 = rand()%5;
        }
        else
        {
            tDisable2.start();
            lv_obj_clear_flag(btn2,LV_OBJ_FLAG_CLICKABLE);
            pl2Disabled = 1;
        }

        input2 = 1;
        printf("%d\n",input2);
    }
}

void lv_btn1(void)
{
    btn1 = lv_btn_create(lv_scr_act());
    lv_obj_add_event_cb(btn1, event_handler1, LV_EVENT_PRESSED, &btn1);
    lv_obj_set_size(btn1, 220, 200);
    lv_obj_align(btn1, LV_ALIGN_CENTER, -115, 20);

    label = lv_label_create(btn1);
    lv_label_set_text(label, "Action pl1");
    lv_obj_center(label);
}

void lv_btn2(void)
{
    btn2 = lv_btn_create(lv_scr_act());
    lv_obj_add_event_cb(btn2, event_handler2, LV_EVENT_PRESSED, &btn2);
    lv_obj_set_size(btn2, 220, 200);
    lv_obj_align(btn2, LV_ALIGN_CENTER, 115, 20);

    label = lv_label_create(btn2);
    lv_label_set_text(label, "Action pl2");
    lv_obj_center(label);
}

int main() {

    threadLvgl.lock();

    //lv_demo_widgets();

    //lv_bar1();
    //lv_bar2();

    lv_label_score1();
    lv_label_score2();

    lv_btn1();
    lv_btn2();

    val2.period_ms(50);
    val2.pulsewidth(0);

    threadLvgl.unlock();

    int n = 0;

    uint32_t capt1=0;
    uint32_t capt2=0;

    while (1) {
        // put your main code here, to run repeatedly:
        ThisThread::sleep_for(10ms);
    }
}

