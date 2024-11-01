auto web = R"delimiter(

<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>Login</title>
</head>
<body>
<h1>Login</h1>
<div>
    <label for="wifi-info-items">选择要连接的WiFi</label>
    <select id="wifi-info-items">
    </select>
    <br>
    <label for="password">输入WiFi密码</label>
    <input type="password" id="password">
</div>
<button onclick="scan()">Scan</button>
<button onclick="connect()">Connect</button>
</body>
<script>
    async function scan() {
        try {
            const response = await fetch('/scan', {
                method: 'GET',
                headers: {
                    'Content-Type': 'application/json'
                }
            });
            if (response.ok) {
                const data = await response.json();
                console.log('Scan result:', data);
                const wifiInfoItems = document.getElementById('wifi-info-items');
                wifiInfoItems.innerHTML = ''; // Clear existing items
                data.forEach(item => {
                    const option = document.createElement('option');
                    option.textContent = `SSID: ${item.ssid}, Signal Strength: ${item.rssi}, encryption: ${item.encryption}`;
                    wifiInfoItems.appendChild(option);
                });
            } else {
                console.error('Scan failed:', response.statusText);
            }
        } catch (error) {
            console.error('Error:', error);
        }
    }

    async function connect() {
        const wifiInfoItems = document.getElementById('wifi-info-items');
        const selectedWifi = wifiInfoItems.options[wifiInfoItems.selectedIndex].text;
        const ssid = selectedWifi.split(',')[0].split(':')[1].trim();
        const password = document.getElementById('password').value;
        try {
            const response = await fetch('/connect', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json'
                },
                body: JSON.stringify({ ssid, password })
            });
            if (response.ok) {
                const data = await response.json();
                console.log('Connect result:', data);
            } else {
                console.error('Connect failed:', response.statusText);
            }
        } catch (error) {
            console.error('Error:', error);
        }
    }
</script>
</html>

)delimiter";