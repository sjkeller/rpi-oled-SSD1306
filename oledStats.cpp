#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "ssd1306.h"

#define HOST        "hostname"
#define HOSTIP      "hostname -I | cut -d' ' -f1"
#define RAMTOTAL    "free -m | awk 'NR==2{print $2}'"
#define RAMUSED     "free -m | awk 'NR==2{print $3}'"
#define TEMP        "vcgencmd measure_temp | cut -d'=' -f2"
#define CLOCK       "cat /sys/devices/system/cpu/cpu0/cpufreq/scaling_cur_freq | awk '{print $1/1e6}'"
#define CPULOAD     "top -bn 1 |grep \"Cpu(s)\" | awk '{print $2+$6+$4+$12+$14+$16 \"%\"}'"
#define DRIVETOTAL  "lsblk -o size,fsused /dev/sda1 | awk 'NR==2{print $1}'"
#define DRIVEUSED   "lsblk -o size,fsused /dev/sda1 | awk 'NR==2{print $2}'"
#define UPTIME      "uptime -p"
#define AVAILUP     "/usr/lib/update-notifier/apt-check 2>&1 | cut -d ';' -f 1"

char* run_shell_command(const char* cmd) {
    char buffer[128];
    char* result = NULL;
    size_t result_len = 0;
    FILE* pipe = popen(cmd, "r");

    if (!pipe) {
        return NULL;
    }

    while (!feof(pipe)) {
        if (fgets(buffer, sizeof(buffer), pipe) != NULL) {
            size_t len = strlen(buffer);
            char* temp = (char*) realloc(result, result_len + len);

            if (!temp) {
                free(result);
                return NULL;
            }

            result = temp;
            memcpy(result + result_len, buffer, len);
            result_len += len;
        }
    }

    pclose(pipe);

    if (result) {
        result[result_len - 1] = '\0';
    }

    return result;
}

int main( int argc, char * argv [] ) {

    char textBuffer[8][60];
	SSD1306 myDisplay;
	myDisplay.initDisplay();
    myDisplay.setDisplayMode(SSD1306::Mode::WRAP);
    myDisplay.clearDisplay();

    char time_string[6];

    char* host;
    char* hostip;

    char* totalram;
    char* totaldisk;

    sleep(5);

    host = run_shell_command(HOST);
    hostip = run_shell_command(HOSTIP);
    
    totalram = run_shell_command(RAMTOTAL);
    totaldisk = run_shell_command(DRIVETOTAL);

    sprintf(textBuffer[1], "-------------------------------");
    sprintf(textBuffer[5], "-------------------------------");

    while(1) {
        
        sprintf(textBuffer[4], "DISK:    %sB / %sB", run_shell_command(DRIVEUSED), totaldisk);
        sprintf(textBuffer[7], "%s update(s) pending", run_shell_command(AVAILUP));

        for (int k = 0; k < 60; k ++) {

            time_t current_time = time(NULL);
            struct tm *time_info = localtime(&current_time);
            strftime(time_string, sizeof(time_string), "%H:%M", time_info);

            sprintf(textBuffer[0], "%s - %s - %s", host, hostip, time_string);
            sprintf(textBuffer[2], "CPU:    %s @ %sGHz %s", run_shell_command(CPULOAD), run_shell_command(CLOCK), run_shell_command(TEMP));
            sprintf(textBuffer[3], "RAM:    %sMB / %sMB", run_shell_command(RAMUSED), totalram);
            sprintf(textBuffer[6], "%s", run_shell_command(UPTIME));
            

            for (int i = 0; i < 8; i ++) {
                myDisplay.textDisplay(textBuffer[i]);
            }

            sleep(5);
        }

    }
	
}