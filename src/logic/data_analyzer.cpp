#include "data_analyzer.h"
#include "hvac_data.h" // Include the full definition here

void DataAnalyzer::process(HVACData& data, float ampsOnThreshold) {
  // Calculate Delta T
  if (data.returnTempC != -127.0 && data.supplyTempC != -127.0) {
    data.deltaT = data.returnTempC - data.supplyTempC; // Assumes cooling mode
  } else {
    data.deltaT = 0;
  }

  // Determine Component Status based on amperage
  data.fanStatus = (data.fanAmps > ampsOnThreshold) ? "ON" : "OFF";
  data.compressorStatus = (data.compressorAmps > ampsOnThreshold) ? "ON" : "OFF";
  data.geoPumpsStatus = (data.geoPumpsAmps > ampsOnThreshold) ? "ON" : "OFF";

  // Determine Airflow Status (Placeholder for actual sensor)
  if (data.fanStatus == "ON") {
      data.airflowStatus = "OK"; // Assume OK if fan is running
  } else {
      data.airflowStatus = "N/A";
  }

  // Basic Anomaly Detection (PRD FR-4.1)
  if (data.fanStatus == "ON" && data.airflowStatus != "OK") {
      data.alertStatus = "ALERT: Fan ON, No Airflow";
  } else {
      data.alertStatus = "NONE";
  }
}