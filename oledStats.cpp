#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "ssd1306.h"

#define HOST        "hostname"
#define HOSTIP      "hostname -I | cut -d' ' -f1"
#define RAMTOTAL    "free -m | awk 'NR==2{print $2 \"MB\"}'"
#define RAMUSED     "free -m | awk 'NR==2{print $3 \"MB\"}'"
#define TEMP        "vcgencmd measure_temp | cut -d'=' -f2"
#define CLOCK       "printf \"%.3f GHz\" $(vcgencmd measure_clock arm | awk -F'=' '{print $2/1e9}')"
#define CPULOAD     "top -bn 1 |grep \"Cpu(s)\" | awk '{print $2+$6+$4+$12+$14+$16 \"%\"}'"
#define DRIVETOTAL  "lsblk -o size,fsused /dev/sda1 | awk 'NR==2{print $1 \"B\"}'"
#define DRIVEUSED   "lsblk -o size,fsused /dev/sda1 | awk 'NR==2{print $2 \"B\"}'"
#define UPTIME      "uptime -p"
#define AVAILUP     "/usr/lib/update-notifier/apt-check 2>&1 | cut -d ';' -f 1"
#define TIME        "date +'%H:%M'"


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

    char textBuffer[8][50];
	SSD1306 myDisplay;
	myDisplay.initDisplay();
    myDisplay.setDisplayMode(SSD1306::Mode::WRAP);
    myDisplay.clearDisplay();

    while(1) {

        sprintf(textBuffer[7], "%s update(s) pending", run_shell_command(AVAILUP));

        for (int k = 0; k < 200; k ++) {

            sprintf(textBuffer[0], "%s - %s - %s", run_shell_command(HOST), run_shell_command(HOSTIP), run_shell_command(TIME));
            sprintf(textBuffer[1], "------------------------------");
            sprintf(textBuffer[2], "CPU:    %s @ %s", run_shell_command(CPULOAD), run_shell_command(CLOCK));
            sprintf(textBuffer[3], "RAM:    %s / %s", run_shell_command(RAMUSED), run_shell_command(RAMTOTAL));
            sprintf(textBuffer[4], "DISK:    %s / %s", run_shell_command(DRIVEUSED), run_shell_command(DRIVETOTAL));
            sprintf(textBuffer[5], "TEMP:    %s", run_shell_command(TEMP));
            sprintf(textBuffer[6], "%s", run_shell_command(UPTIME));
            

            for (int i = 0; i < 8; i ++) {
                myDisplay.textDisplay(textBuffer[i]);
            }

            sleep(3);
        }

    }
	
}