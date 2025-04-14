function updateSlider(slider, axisNumber) {
    const tooltip = document.getElementById("tooltip" + axisNumber);
    tooltip.textContent = slider.value;

    // Position the tooltip
    const sliderRect = slider.getBoundingClientRect();
    const tooltipX = (slider.value - slider.min) / (slider.max - slider.min) * slider.offsetWidth;
    tooltip.style.left = tooltipX + "px";

    // Optional: send value to backend or microcontroller
}

// Initialize tooltips on page load
document.addEventListener("DOMContentLoaded", () => {
    document.querySelectorAll('input[type="range"]').forEach((slider, index) => {
        updateSlider(slider, index + 1);
    });
});
