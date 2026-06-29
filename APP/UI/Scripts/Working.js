document.querySelectorAll("#Home-Call").forEach(card => {
  card.addEventListener("click", () => {
    const page = card.dataset.page;
    window.location.href = page;
  });
});