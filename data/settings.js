document.addEventListener('DOMContentLoaded', () => {
    const form = document.getElementById('settingsForm');
    const messageDiv = document.getElementById('message');
    const rebootButton = document.getElementById('rebootButton');
    const resetButton = document.getElementById('resetButton');

    // Fetch current settings and populate the form
    fetch('/api/settings')
        .then(response => response.json())
        .then(data => {
            document.getElementById('lowDeltaTThreshold').value = data.lowDeltaTThreshold;
            document.getElementById('lowDeltaTDurationS').value = data.lowDeltaTDurationS;
            document.getElementById('noAirflowDurationS').value = data.noAirflowDurationS;
            document.getElementById('tempSensorDisconnectedDurationS').value = data.tempSensorDisconnectedDurationS;
        })
        .catch(error => console.error('Error fetching settings:', error));

    // Handle form submission
    form.addEventListener('submit', (event) => {
        event.preventDefault();
        
        const formData = new FormData(form);
        const settings = {
            lowDeltaTThreshold: parseFloat(formData.get('lowDeltaTThreshold')),
            lowDeltaTDurationS: parseInt(formData.get('lowDeltaTDurationS'), 10),
            noAirflowDurationS: parseInt(formData.get('noAirflowDurationS'), 10),
            tempSensorDisconnectedDurationS: parseInt(formData.get('tempSensorDisconnectedDurationS'), 10)
        };

        fetch('/api/settings', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
            },
            body: JSON.stringify(settings),
        })
        .then(response => {
            // Check if the request was successful. If not, parse the error.
            if (!response.ok) {
                return response.json().then(err => { throw new Error(err.message || 'Validation failed on server.'); });
            }
            return response.json();
        })
        .then(data => {
            messageDiv.textContent = data.message || 'Settings saved!';
            messageDiv.className = 'message success';
            setTimeout(() => { messageDiv.className = 'message'; }, 3000);
        })
        .catch(error => {
            messageDiv.textContent = error.message || 'Error saving settings.';
            messageDiv.className = 'message error';
            // The error is already displayed, but we log it for debugging.
            console.error('Error saving settings:', error);
        });
    });

    // Handle reboot button click
    rebootButton.addEventListener('click', () => {
        if (confirm('Are you sure you want to reboot the device?')) {
            messageDiv.textContent = 'Rebooting device... Please wait a moment before reconnecting.';
            messageDiv.className = 'message success';

            fetch('/api/reboot', { method: 'POST' })
                .catch(error => {
                    // This catch might not even fire if the server reboots before responding,
                    // but it's good practice to have it.
                    console.error('Error sending reboot command:', error);
                    messageDiv.textContent = 'Failed to send reboot command.';
                    messageDiv.className = 'message error';
                });
        }
    });

    // Handle factory reset button click
    resetButton.addEventListener('click', () => {
        if (confirm('Are you sure you want to perform a factory reset? This will erase all saved settings and reboot the device.')) {
            messageDiv.textContent = 'Performing factory reset... The device will reboot.';
            messageDiv.className = 'message success';

            fetch('/api/factory_reset', { method: 'POST' })
                .catch(error => {
                    // This catch might not even fire if the server reboots before responding,
                    // but it's good practice to have it.
                    console.error('Error sending factory reset command:', error);
                    messageDiv.textContent = 'Failed to send factory reset command.';
                    messageDiv.className = 'message error';
                });
        }
    });
});