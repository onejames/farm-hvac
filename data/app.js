let historyChart; // Keep a global reference to the chart instance

function fetchRealtimeData() {
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
        .finally(() => setTimeout(fetchRealtimeData, 5000));
}

function fetchChartData() {
    fetch('/api/aggregated_history')
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

document.addEventListener('DOMContentLoaded', () => {
    fetchRealtimeData();
    fetchChartData();
});