document.querySelectorAll(".feature-card")
  .forEach(card => { 
    card.addEventListener("click", () => {
    const page = card.dataset.page;
    window.location.href = page;
  });
});