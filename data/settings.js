document.addEventListener('DOMContentLoaded', () => {
    const form = document.getElementById('settingsForm');
    const messageDiv = document.getElementById('message');

    // Fetch current settings and populate the form
    fetch('/api/settings')
        .then(response => response.json())
        .then(data => {
            document.getElementById('lowDeltaTThreshold').value = data.lowDeltaTThreshold;
            document.getElementById('lowDeltaTDurationS').value = data.lowDeltaTDurationS;
            document.getElementById('noAirflowDurationS').value = data.noAirflowDurationS;
        })
        .catch(error => console.error('Error fetching settings:', error));

    // Handle form submission
    form.addEventListener('submit', (event) => {
        event.preventDefault();
        
        const formData = new FormData(form);
        const settings = {
            lowDeltaTThreshold: parseFloat(formData.get('lowDeltaTThreshold')),
            lowDeltaTDurationS: parseInt(formData.get('lowDeltaTDurationS'), 10),
            noAirflowDurationS: parseInt(formData.get('noAirflowDurationS'), 10)
        };

        fetch('/api/settings', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
            },
            body: JSON.stringify(settings),
        })
        .then(response => response.json())
        .then(data => {
            messageDiv.textContent = data.message || 'Settings saved!';
            messageDiv.className = 'message success';
            setTimeout(() => { messageDiv.className = 'message'; }, 3000);
        })
        .catch(error => {
            messageDiv.textContent = 'Error saving settings.';
            messageDiv.className = 'message error';
            console.error('Error saving settings:', error);
        });
    });
});