#include "html_builder.h"
#include <stdio.h> // For snprintf

String HtmlBuilder::build(const HVACData& data, const char* version) {
    char buffer[64]; // A reusable buffer for formatting numbers

    String html = "<!DOCTYPE html><html><head><title>HVAC Monitor</title>";
    // Use JavaScript for a smoother refresh experience than the meta tag
    html += "<script>setTimeout(function(){window.location.reload();}, 5000);</script>";
    html += "</head><body>";
    html += "<h1>HVAC Real-Time Status</h1>";
    html += "<h2>Temperatures</h2>";

    snprintf(buffer, sizeof(buffer), "%.1f", data.returnTempC);
    html += "<p>Return Air: " + String(buffer) + " &deg;C</p>";

    snprintf(buffer, sizeof(buffer), "%.1f", data.supplyTempC);
    html += "<p>Supply Air: " + String(buffer) + " &deg;C</p>";

    snprintf(buffer, sizeof(buffer), "%.1f", data.deltaT);
    html += "<p><b>Delta T: " + String(buffer) + " &deg;C</b></p>";

    html += "<h2>Component Status</h2>";
    snprintf(buffer, sizeof(buffer), "%.2f", data.fanAmps);
    html += "<p>Fan: " + data.fanStatus + " (" + String(buffer) + " A)</p>";

    snprintf(buffer, sizeof(buffer), "%.2f", data.compressorAmps);
    html += "<p>Compressor: " + data.compressorStatus + " (" + String(buffer) + " A)</p>";

    snprintf(buffer, sizeof(buffer), "%.2f", data.geoPumpsAmps);
    html += "<p>Geothermal Pumps: " + data.geoPumpsStatus + " (" + String(buffer) + " A)</p>";

    html += "<h2>System</h2>";
    html += "<p>Airflow: " + data.airflowStatus + "</p>";
    html += "<p><b>Alerts: " + data.alertStatus + "</b></p>";
    html += "<hr><p><i>Version: " + String(version) + "</i></p>";
    html += "</body></html>";
    return html;
}