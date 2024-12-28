function Open_menu() {
  document.getElementById("Main_menu").style.width = "250px";
}

function closeMenu() {
  document.getElementById("Main_menu").style.width = "0";
}

function convertSeconds(seconds) {
  const days = Math.floor(seconds / (24 * 3600));
  seconds %= 24 * 3600;

  const hours = Math.floor(seconds / 3600);
  seconds %= 3600;

  const minutes = Math.floor(seconds / 60);
  const remainingSeconds = seconds % 60;

  return {
    days: days,
    hours: hours,
    minutes: minutes,
    seconds: remainingSeconds
  };
}
