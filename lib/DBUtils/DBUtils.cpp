#include "DBUtils.h"
#include "config.h"

InfluxDBClient dbClient(INFLUXDB_URL, INFLUXDB_ORG, INFLUXDB_BUCKET, INFLUXDB_TOKEN);

char ar_mac_str[18];

Point sensor("sensor_data");

void DBInit() {
    timeSync(TZ_INFO, "pool.ntp.org", "time.nis.gov");
  
    sensor.addTag("device", "Ar1-Sala219");

    snprintf(ar_mac_str, sizeof(ar_mac_str), "%02X:%02X:%02X:%02X:%02X:%02X",
        arMac[0], arMac[1], arMac[2],
        arMac[3], arMac[4], arMac[5]);
        
    sensor.addTag("sender", ar_mac_str);
  
    // Check server connection
    if (dbClient.validateConnection()) {
      Serial.print("Connected to InfluxDB: ");
      Serial.println(dbClient.getServerUrl());
    } else {
      Serial.print("InfluxDB connection failed: ");
      Serial.println(dbClient.getLastErrorMessage());
    }
}

void sendCommandToInflux(String commandDataStr) {

   Point comandos("Comandos_Registrados");

   comandos.addTag("Local", ESP_NOME);

   comandos.addTag("idMódulo", ar_mac_str);
   comandos.addTag("commandData", commandDataStr);

   Serial.printf("\n[%.3lu] Enviando para Influx -> %s A\n", millis(), commandDataStr);

   if (dbClient.writePoint(comandos)) {

       Serial.println("| DB Status: Success");

   } else {

       Serial.printf("| DB Error: %s\n", dbClient.getLastErrorMessage().c_str());
   }
}
void sendPowerDataToInflux(PowerData data) {

        // Considera como zero se valor < 100 mA (offset do sensor)
    if (fabs(data.currentReading) < 0.1) {

    data.currentReading = 0.0;

    }

    sensor.clearFields();

    
    Serial.printf("%02x:%02x:%02x:%02x:%02x:%02x\n",
        arMac[0], arMac[1], arMac[2],
        arMac[3], arMac[4], arMac[5]);


    sensor.addField("value", data.currentReading);

    Serial.printf("\n[%.3lu] Enviando para Influx -> %.3f A\n", millis(), data.currentReading);
    if (dbClient.writePoint(sensor)) {
    Serial.println("| DB Status: Success");
    } else {
    Serial.printf("| DB Error: %s\n", dbClient.getLastErrorMessage().c_str());
    }
}