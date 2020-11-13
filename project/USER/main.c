#include "main.h"

int main(void)
{
    uint8_t disChina = 0;
    uint8_t cnt_sec = 0;
    uint8_t cnt_min = 0;
    uint16_t cnt_ms = 0;

    delay_init();
    led_init();
    key_init();
    lcd_init(BLACK);

    uart1_init(115200);
    uart2_init(115200);
    mem_init();

    atk_8266_wifista_config();
    build_ssl_connect(api_type, api_ip, api_port);

    get_ncov_api(api_addr, parse_ncov_data);
    while (1)
    {
        #ifndef USE_EXIT
        switch (key_get(0))
        {
        case 1:
            get_ncov_api(api_addr, parse_ncov_data);
            break;
        case 2:
            disChina = 1;
            cnt_sec = 0;
            gui_show_dataChina();
            break;
        case 3:
            disChina = 0;
            cnt_sec = 0;
            gui_show_dataGlobal();
            break;
        default:
            break;
        }
        #endif

        if(cnt_ms == 1000)
        {
            led1_toggle();
            cnt_ms = 0;
            cnt_sec++;
            if(cnt_sec == 60)
            {
                cnt_sec = 0;
                cnt_min++;
                if(cnt_min > 10)
                    cnt_min = 0;
                if(cnt_min == 10)
                {
                    printf("开始更新疫情数据\r\n");
                    get_ncov_api(api_addr, parse_ncov_data);
                }
            }

            if(cnt_sec % 10 == 1)
            {
                printf("切换显示\r\n");
                disChina = !disChina;
                disChina ? gui_show_dataChina() : gui_show_dataGlobal();
            }
            printf("%2d:%2d\r\n", cnt_min, cnt_sec);
        }
        cnt_ms++;
        delay_ms(1);
    }
}
