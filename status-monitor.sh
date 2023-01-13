#!/bin/bash

# shell commands for sourcing stats
HOST=$(hostname)
HOSTIP=$(hostname -I | cut -d' ' -f1)
RAMTOTAL=$(free -m | awk 'NR==2{print $2 "MB"}')
RAMUSED=$(free -m | awk 'NR==2{print $3 "MB"}')
TEMP=$(vcgencmd measure_temp | cut -d'=' -f2)
CLOCK=$(printf "%.3f GHz" $(vcgencmd measure_clock arm | awk -F'=' '{print $2/1e9}'))
CPULOAD=$(top -bn 1 |grep "Cpu(s)" | awk '{print $2+$6+$4+$12+$14+$16 "%"}')
DRIVETOTAL=$(lsblk -o size,fsused /dev/sda1 | awk 'NR==2{print $1 "B"}')
DRIVEUSED=$(lsblk -o size,fsused /dev/sda1 | awk 'NR==2{print $2 "B"}')
UPTIME="$(uptime -p)"
AVAILUP="$(/usr/lib/update-notifier/apt-check 2>&1 | cut -d ';' -f 1)"
TIME=$(date +'%H:%M')

# give oled executable string lines
/home/pi/pi-ssd1306-oled/oled \
	-t "$HOST - $HOSTIP - $TIME" \
    -t "------------------------------" \
    -t "CPU:    $CPULOAD @ $CLOCK" \
    -t "RAM:    $RAMUSED / $RAMTOTAL" \
    -t "DISK:   $DRIVEUSED / $DRIVETOTAL"\
    -t "TEMP:   $TEMP" \
    -t "$UPTIME" \
    -t "$AVAILUP update(s) pending"
    