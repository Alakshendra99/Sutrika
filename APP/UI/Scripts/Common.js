const LoggingToggle = document.getElementById('LoggingToggle');

LoggingToggle.addEventListener('change', async () => {
  const Enabled = LoggingToggle.checked;
  if (Enabled) {
    await window.Sutrika.Log.State(true, 1);
  } else {
    await window.Sutrika.Log.State(false);
  }
});

document.querySelectorAll("#Home-Call")
  .forEach(card => { 
    card.addEventListener("click", () => {
    const page = card.dataset.page;
    window.location.href = page;
  });
});