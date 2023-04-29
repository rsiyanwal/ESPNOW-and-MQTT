# ESPNOW-and-MQTT
We are using MQTT to forward the motion sensor data received from NodeMCU to Raspberry Pi (with Linux based OS) via data cable. MQTT allows us to receive data on a terminal and use it to execute other commands, unlike ESP-NOW. Below are the commands we use to set up the Mosquitto MQTT broker:
```
sudo apt-get install mosquitto
sudo apt-get install mosquitto-clients -y
```

To make necessary changes, the configuration file of Mosquitto needs to be accessed which can be done using the following command:
```
sudo nano /etc/mosquitto/mosquitto.conf
```

Change the configurations file to
```
pid_file /var/run/mosquitto.pid
persistence true
persistence_location /var/lib/mosquitto/
log_dest file /var/log/mosquitto/mosquitto.log
allow_anonymous false
password_file /etc/mosquitto/pwfile
listener 1883
```

Next, we will generate a new username and password for MQTT, which will be used for sending and receiving messages.
```
sudo mosquitto_passwd -c /etc/mosquitto/pwfile "username"
sudo mosquitto_passwd -c /etc/mosquitto/pwfile "pass"
```

In order to get data on terminal via MQTT we can use the command
```
mosquitto_sub -d -u username -P password -t test
```
and to send data via terminal we can use
```
mosquitto_pub -d -u username -P password -t test -m "Hello Pritha!"
```

### Caveat:
The loop is designed to restart the NodeMCU after a specified time period. We have observed that when the NodeMCU is running with both ESP-NOW and MQTT protocols, it can freeze if no data is sent by the sensor for an extended period of time. To ensure the system remains operational, we restart the NodeMCU periodically. However, this process incurs an overhead of approximately 5 seconds for the NodeMCU to restart and reconnect to the WiFi network.
