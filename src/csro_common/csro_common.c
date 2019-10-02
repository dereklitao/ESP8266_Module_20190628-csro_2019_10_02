#include "csro_common.h"
#include "./csro_device/csro_devices.h"

csro_system sysinfo;
csro_mqtt mqttinfo;
esp_mqtt_client_handle_t mqttclient;

void csro_mqtt_client_info(void)
{
#ifdef NLIGHT
    sprintf(sysinfo.dev_type, "nlight%d", NLIGHT);
#elif defined NLIGHT_4K4R
    sprintf(sysinfo.dev_type, "nlight4");
#elif defined NLIGHT_6K4R
    sprintf(sysinfo.dev_type, "nlight4");
#elif defined DLIGHT
    sprintf(sysinfo.dev_type, "dlight");
#elif defined RGBLIGHT
    sprintf(sysinfo.dev_type, "rgblight");
#elif defined MOTOR_4K4R
    sprintf(sysinfo.dev_type, "motor2");
#elif defined AIR_MONITOR
    sprintf(sysinfo.dev_type, "airmon");
#endif

    size_t len = 0;
    nvs_handle handle;
    nvs_open("system", NVS_READONLY, &handle);
    nvs_get_str(handle, "router_ssid", NULL, &len);
    nvs_get_str(handle, "router_ssid", sysinfo.router_ssid, &len);
    nvs_get_str(handle, "router_pass", NULL, &len);
    nvs_get_str(handle, "router_pass", sysinfo.router_pass, &len);
    nvs_close(handle);

    uint8_t mac[6];

    esp_wifi_get_mac(WIFI_MODE_STA, mac);
    sprintf(sysinfo.mac_str, "%02x%02x%02x%02x%02x%02x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    sprintf(sysinfo.host_name, "CSRO_%s", sysinfo.mac_str);

    sprintf(mqttinfo.lwt_topic, "csro/%s/%s/available", sysinfo.mac_str, sysinfo.dev_type);
    sprintf(mqttinfo.id, "csro/%s", sysinfo.mac_str);
    sprintf(mqttinfo.name, "csro/%s/%s", sysinfo.dev_type, sysinfo.mac_str);
    sprintf(mqttinfo.pass, "%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x", mac[1], mac[3], mac[5], mac[0], mac[2], mac[4], mac[5], mac[3], mac[1], mac[4], mac[2], mac[0]);
    printf("\r\nid = %s.\nname = %s.\npass = %s.\r\nssid = %s.\r\npass = %s.\r\n", mqttinfo.id, mqttinfo.name, mqttinfo.pass, sysinfo.router_ssid, sysinfo.router_pass);
}

void app_main(void)
{
    nvs_handle handle;
    nvs_flash_init();
    nvs_open("system", NVS_READWRITE, &handle);
    nvs_get_u32(handle, "power_cnt", &sysinfo.power_cnt);
    nvs_set_u32(handle, "power_cnt", (sysinfo.power_cnt + 1));
    nvs_get_u8(handle, "router_flag", &sysinfo.router_flag);
    nvs_commit(handle);
    nvs_close(handle);
    csro_device_init();
    if (sysinfo.router_flag == 1)
    {
        csro_start_mqtt();
    }
    else
    {
        csro_start_smart_config();
    }
}