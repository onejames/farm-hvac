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