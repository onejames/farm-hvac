let historyChart; // Keep a global reference to the chart instance

// Detect if we are running in a local development environment
const isLocal = window.location.hostname === '127.0.0.1' || window.location.hostname === 'localhost';
const apiBasePath = isLocal ? '/mock' : '/api';

function fetchRealtimeData() {
    fetch(`${apiBasePath}/data.json`)
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
        .finally(() => setTimeout(fetchRealtimeData, 5000));
}

function fetchChartData() {
    fetch(`${apiBasePath}/aggregated_history.json`)
        .then(response => response.json())
        .then(data => {
            if (data.length === 0) return;

            const labels = data.map(d => new Date(d.timestamp).toLocaleTimeString());
            const deltaTData = data.map(d => d.avgDeltaT.toFixed(1));
            const fanAmpsData = data.map(d => d.avgFanAmps.toFixed(2));
            const compressorAmpsData = data.map(d => d.avgCompressorAmps.toFixed(2));

            if (historyChart) {
                // If chart exists, just update its data
                historyChart.data.labels = labels;
                historyChart.data.datasets[0].data = deltaTData;
                historyChart.data.datasets[1].data = fanAmpsData;
                historyChart.data.datasets[2].data = compressorAmpsData;
                historyChart.update();
            } else {
                // Otherwise, create the chart for the first time
                const ctx = document.getElementById('historyChart').getContext('2d');
                historyChart = new Chart(ctx, {
                    type: 'line',
                    data: {
                        labels: labels,
                        datasets: [
                            {
                                label: 'Avg Delta T (°C)',
                                data: deltaTData,
                                borderColor: 'rgb(255, 99, 132)',
                                yAxisID: 'y',
                                tension: 0.1
                            },
                            {
                                label: 'Avg Fan Amps (A)',
                                data: fanAmpsData,
                                borderColor: 'rgb(54, 162, 235)',
                                yAxisID: 'y1',
                                tension: 0.1
                            },
                            {
                                label: 'Avg Compressor Amps (A)',
                                data: compressorAmpsData,
                                borderColor: 'rgb(75, 192, 192)',
                                yAxisID: 'y1',
                                tension: 0.1
                            }
                        ]
                    },
                    options: {
                        scales: {
                            y: { type: 'linear', display: true, position: 'left', title: { display: true, text: 'Temperature (°C)' } },
                            y1: { type: 'linear', display: true, position: 'right', title: { display: true, text: 'Amperage (A)' }, grid: { drawOnChartArea: false } }
                        }
                    }
                });
            }
        })
        .catch(error => console.error('Error fetching chart data:', error))
        .finally(() => setTimeout(fetchChartData, 60000)); // Update chart every minute
}

function formatUptime(ms) {
    if (ms < 1000) return `${ms} ms`;

    let seconds = Math.floor(ms / 1000);
    let minutes = Math.floor(seconds / 60);
    let hours = Math.floor(minutes / 60);
    let days = Math.floor(hours / 24);

    hours %= 24;
    minutes %= 60;
    seconds %= 60;

    const parts = [];
    if (days > 0) parts.push(`${days}d`);
    if (hours > 0) parts.push(`${hours}h`);
    if (minutes > 0) parts.push(`${minutes}m`);
    if (seconds > 0) parts.push(`${seconds}s`);

    return parts.join(' ') || '0s';
}

function fetchStatusData() {
    fetch(`${apiBasePath}/status.json`)
        .then(response => response.json())
        .then(data => {
            document.getElementById('uptime').innerText = formatUptime(data.uptime_ms);
        })
        .catch(error => console.error('Error fetching status data:', error))
        .finally(() => setTimeout(fetchStatusData, 15000)); // Update every 15 seconds
}

document.addEventListener('DOMContentLoaded', () => {
    fetchRealtimeData();
    fetchChartData();
    fetchStatusData();
});