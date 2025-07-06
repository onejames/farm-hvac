document.addEventListener('DOMContentLoaded', () => {
    const logContent = document.getElementById('logContent');
    const refreshButton = document.getElementById('refreshButton');
    const clearButton = document.getElementById('clearButton');

    const isLocal = window.location.hostname === '127.0.0.1' || window.location.hostname === 'localhost';
    const apiBasePath = isLocal ? '/mock' : '/api';

    function fetchLogs() {
        logContent.textContent = 'Loading logs...';
        fetch(`${apiBasePath}/logs`)
            .then(response => response.text())
            .then(text => {
                logContent.textContent = text || 'Log file is empty.';
            })
            .catch(error => {
                console.error('Error fetching logs:', error);
                logContent.textContent = 'Error fetching logs.';
            });
    }

    refreshButton.addEventListener('click', fetchLogs);

    clearButton.addEventListener('click', () => {
        if (confirm('Are you sure you want to clear all system logs? This action cannot be undone.')) {
            fetch(`${apiBasePath}/logs/clear`, { method: 'POST' })
                .then(response => {
                    if (response.ok) {
                        fetchLogs(); // Refresh logs after clearing
                    } else {
                        alert('Failed to clear logs.');
                    }
                })
                .catch(error => {
                    console.error('Error clearing logs:', error);
                    alert('Error sending clear logs command.');
                });
        }
    });

    // Initial load
    fetchLogs();
});