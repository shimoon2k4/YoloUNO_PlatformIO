// ==================== WEBSOCKET ====================
var gateway = `ws://${window.location.hostname}/ws`;
var websocket;

window.addEventListener('load', onLoad);

function onLoad(event) {
    initWebSocket();
}

function onOpen(event) {
    console.log('Connection opened');
}

function onClose(event) {
    console.log('Connection closed');
    setTimeout(initWebSocket, 2000);
}

function initWebSocket() {
    console.log('Trying to open a WebSocket connection…');
    websocket = new WebSocket(gateway);
    websocket.onopen = onOpen;
    websocket.onclose = onClose;
    websocket.onmessage = onMessage;
}

function Send_Data(data) {
    if (websocket && websocket.readyState === WebSocket.OPEN) {
        websocket.send(data);
        console.log("Send:", data);
    } else {
        console.warn("WebSocket is not ready!");
        alert("WebSocket is not connected!");
    }
}

function onMessage(event) {
    console.log("Receive:", event.data);
    try {
        var data = JSON.parse(event.data);
        if (data.page === "dashboard") {
            document.getElementById("gauge_temp").innerHTML = data.temp + " &deg;C";
            document.getElementById("gauge_humi").innerHTML = data.humi + " %";
        }
    } catch (e) {
        console.warn("Invalid JSON:", event.data);
    }
}


// ==================== UI NAVIGATION ====================
let relayList = [
    { id: 1, name: "LED 1 (Living Room)", gpio: 4, state: false },
    { id: 2, name: "LED 2 (Bedroom)", gpio: 5, state: false }
];
let deleteTarget = null;

function showSection(id, event) {
    document.querySelectorAll('.section').forEach(sec => sec.style.display = 'none');
    document.getElementById(id).style.display = id === 'settings' ? 'flex' : 'block';
    document.querySelectorAll('.nav-item').forEach(i => i.classList.remove('active'));
    event.currentTarget.classList.add('active');
}


// ==================== HOME GAUGES ====================
window.onload = function () {
    renderRelays(); // Render default relay buttons
};


// ==================== DEVICE FUNCTIONS ====================
function openAddRelayDialog() {
    document.getElementById('addRelayDialog').style.display = 'flex';
}
function closeAddRelayDialog() {
    document.getElementById('addRelayDialog').style.display = 'none';
}
function saveRelay() {
    const name = document.getElementById('relayName').value.trim();
    const gpio = document.getElementById('relayGPIO').value.trim();
    if (!name || !gpio) return alert("⚠️ Please fill all fields!");
    relayList.push({ id: Date.now(), name, gpio, state: false });
    renderRelays();
    closeAddRelayDialog();
}
function renderRelays() {
    const container = document.getElementById('relayContainer');
    container.innerHTML = "";
    relayList.forEach(r => {
        const card = document.createElement('div');
        card.className = 'device-card';
        card.innerHTML = `
      <i class="fa-solid fa-bolt device-icon"></i>
      <h3>${r.name}</h3>
      <p>GPIO: ${r.gpio}</p>
      <button class="toggle-btn ${r.state ? 'on' : ''}" onclick="toggleRelay(${r.id})">
        ${r.state ? 'ON' : 'OFF'}
      </button>
      <i class="fa-solid fa-trash delete-icon" onclick="showDeleteDialog(${r.id})"></i>
    `;
        container.appendChild(card);
    });
}
function toggleRelay(id) {
    const relay = relayList.find(r => r.id === id);
    if (relay) {
        relay.state = !relay.state;
        const relayJSON = JSON.stringify({
            page: "device",
            value: {
                name: relay.name,
                status: relay.state ? "ON" : "OFF",
                gpio: relay.gpio
            }
        });
        Send_Data(relayJSON);
        renderRelays();
    }
}
function showDeleteDialog(id) {
    deleteTarget = id;
    document.getElementById('confirmDeleteDialog').style.display = 'flex';
}
function closeConfirmDelete() {
    document.getElementById('confirmDeleteDialog').style.display = 'none';
}
function confirmDelete() {
    relayList = relayList.filter(r => r.id !== deleteTarget);
    renderRelays();
    closeConfirmDelete();
}


// ==================== SETTINGS TABS ====================
function switchTab(tabId, button) {
    // Hide all tabs
    document.querySelectorAll('.settings-tab').forEach(tab => {
        tab.classList.remove('active');
    });
    
    // Remove active from all buttons
    document.querySelectorAll('.tab-btn').forEach(btn => {
        btn.classList.remove('active');
    });
    
    // Show selected tab and mark button as active
    document.getElementById(tabId).classList.add('active');
    button.classList.add('active');
}

// ==================== SETTINGS FORM ====================
document.getElementById("settingsForm").addEventListener("submit", function (e) {
    e.preventDefault();

    const ssid = document.getElementById("ssid").value.trim();
    const password = document.getElementById("password").value.trim();
    const token = document.getElementById("token").value.trim();
    const server = document.getElementById("server").value.trim();
    const port = document.getElementById("port").value.trim();

    const settingsJSON = JSON.stringify({
        page: "setting",
        value: {
            ssid: ssid,
            password: password,
            token: token,
            server: server,
            port: port
        }
    });

    Send_Data(settingsJSON);
    alert("Configuration sent to device successfully!");
});

// ==================== WIFI ONLY FORM ====================
document.getElementById("wifiOnlyForm").addEventListener("submit", function (e) {
    e.preventDefault();

    const ssid = document.getElementById("wifiSsid").value.trim();
    const password = document.getElementById("wifiPassword").value.trim();

    if (!ssid || !password) {
        alert("⚠️ Please fill all WiFi fields!");
        return;
    }

    const wifiJSON = JSON.stringify({
        page: "wifi_change",
        value: {
            ssid: ssid,
            password: password
        }
    });

    Send_Data(wifiJSON);
    alert("WiFi configuration sent to device! Device will reconnect shortly...");
    
    // Clear form
    document.getElementById("wifiOnlyForm").reset();
});

// ==================== RESET CONFIGURATION ====================
function showResetConfirm() {
    document.getElementById('resetConfirmDialog').style.display = 'flex';
}

function closeResetConfirm() {
    document.getElementById('resetConfirmDialog').style.display = 'none';
}

function confirmReset() {
    const resetJSON = JSON.stringify({
        page: "reset"
    });
    Send_Data(resetJSON);
    closeResetConfirm();
    alert("Device configuration is being reset. It will restart and enter AP mode shortly...");
}
