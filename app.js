// Delay map to store timeout IDs for each axis
const delayMap = {};
const axisValues = { 1: 0, 2: 0, 3: 0, 4: 0 };

// Replace with your ESP8266's actual IP
const ESP8266_IP = "192.168.43.69";  // <- Change this to your actual ESP IP

function updateSlider(slider, axisNumber) {
    const tooltip = document.getElementById("tooltip" + axisNumber);
    tooltip.textContent = slider.value;

    const tooltipX = (slider.value - slider.min) / (slider.max - slider.min) * slider.offsetWidth;
    tooltip.style.left = tooltipX + "px";

    axisValues[axisNumber] = parseInt(slider.value);

    clearTimeout(delayMap[axisNumber]);
    delayMap[axisNumber] = setTimeout(() => {
        console.log(`Sending to ESP: Axis ${axisNumber}: ${slider.value}°`);
        sendAxisData(); 
    }, 300);
}

function sendAxisData() {

    const url = `http://${ESP8266_IP}/servoPos?x=${axisValues[1]}&y=${axisValues[2]}&z=${axisValues[3]}&g=${axisValues[4]}`;
    console.log(url);
    fetch(url)
        .then(response => {
            if (!response.ok) throw new Error("Network response was not ok");
            return response.text();
        })
        .then(data => {
            console.log("ESP Response:", data);
        })
        .catch(error => {
            console.error("Error contacting ESP8266:", error);
        });
}

// Initialize tooltips on page load
document.addEventListener("DOMContentLoaded", () => {
    document.querySelectorAll('input[type="range"]').forEach((slider, index) => {
        updateSlider(slider, index + 1);

        // Attach input event listener
        slider.addEventListener("input", () => {
            updateSlider(slider, index + 1);
        });
    });
});


function testESPLink() {
    fetch("http://192.168.43.69/ping")  // Replace with actual ESP IP
        .then(response => {
            if (response.ok) {
                alert("ESP8266 is connected!");
            } else {
                alert("ESP did not respond.");
            }
        })
        .catch(() => {
            alert("ESP8266 not reachable.");
        });
}
