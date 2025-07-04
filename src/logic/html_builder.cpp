#include "html_builder.h"

String HtmlBuilder::build() {
    String html = "<!DOCTYPE html><html><head><title>HVAC Monitor</title>";
    html += R"rawliteral(<style>
        body { font-family: sans-serif; background-color: #f4f4f4; color: #333; }
        .container { max-width: 600px; margin: auto; background: white; padding: 20px; border-radius: 8px; box-shadow: 0 2px 4px rgba(0,0,0,0.1); }
        h1, h2 { color: #0056b3; }
        .data-point { font-weight: bold; color: #d9534f; }
        .footer { font-size: 0.8em; color: #777; text-align: center; margin-top: 20px; }
    </style>)rawliteral";
    html += R"rawliteral(<script>
        function fetchData() {
            fetch('/api/data')
                .then(response => response.json())
                .then(data => {
                    document.getElementById('returnTemp').innerText = data.returnTempC.toFixed(1);
                    document.getElementById('supplyTemp').innerText = data.supplyTempC.toFixed(1);
                    document.getElementById('deltaT').innerText = data.deltaT.toFixed(1);
                    document.getElementById('fanStatus').innerText = data.fanStatus;
                    document.getElementById('fanAmps').innerText = data.fanAmps.toFixed(2);
                    document.getElementById('compressorStatus').innerText = data.compressorStatus;
                    document.getElementById('compressorAmps').innerText = data.compressorAmps.toFixed(2);
                    document.getElementById('pumpsStatus').innerText = data.geoPumpsStatus;
                    document.getElementById('pumpsAmps').innerText = data.geoPumpsAmps.toFixed(2);
                    document.getElementById('airflow').innerText = data.airflowStatus;
                    document.getElementById('alerts').innerText = data.alertStatus;
                    document.getElementById('version').innerText = data.version;
                    document.getElementById('buildDate').innerText = data.buildDate;
                })
                .catch(error => console.error('Error fetching data:', error))
                .finally(() => setTimeout(fetchData, 5000));
        }
        document.addEventListener('DOMContentLoaded', fetchData);
    </script>)rawliteral";
    html += "</head><body>";
    html += R"rawliteral(<div class="container">
        <h1>HVAC Real-Time Status</h1>
        <h2>Temperatures</h2>
        <p>Return Air: <span id="returnTemp" class="data-point">--</span> &deg;C</p>
        <p>Supply Air: <span id="supplyTemp" class="data-point">--</span> &deg;C</p>
        <p><b>Delta T: <span id="deltaT" class="data-point">--</span> &deg;C</b></p>
        <h2>Component Status</h2>
        <p>Fan: <span id="fanStatus" class="data-point">--</span> (<span id="fanAmps" class="data-point">--</span> A)</p>
        <p>Compressor: <span id="compressorStatus" class="data-point">--</span> (<span id="compressorAmps" class="data-point">--</span> A)</p>
        <p>Geothermal Pumps: <span id="pumpsStatus" class="data-point">--</span> (<span id="pumpsAmps" class="data-point">--</span> A)</p>
        <h2>System</h2>
        <p>Airflow: <span id="airflow" class="data-point">--</span></p>
        <p><b>Alerts: <span id="alerts" class="data-point">--</span></b></p>
        <div class="footer">
            <p><i>Version: <span id="version">--</span> (Built: <span id="buildDate">--</span>)</i></p>
        </div>
    </div>)rawliteral";
    html += "</body></html>";
    return html;
}